#include "ksets/k0.hpp"

#include <numeric>
#include <ctgmath>
#include <sstream>
#include <memory>
#include <stdexcept>

using ksets::K0, ksets::K0Connection, ksets::K0Collection, ksets::numeric, ksets::conntag;

bool K0Connection::perturbWeight(numeric delta) noexcept {
    numeric newWeight = weight + delta;
    if (copysign(1.0, weight) != copysign(1.0, newWeight))
        return false;
    weight = newWeight;
    return true;
}

void K0::swap(K0& other) noexcept {
    activationHistory = std::exchange(other.activationHistory, activationHistory);
    sigmoidQ = std::exchange(other.sigmoidQ, sigmoidQ);
    inboundConnections = std::exchange(other.inboundConnections, inboundConnections);
    // FIXME: hanging references that need outgoingConnections to be tracked
    for (auto& connection : inboundConnections)
        connection.target = std::addressof(other);
    odeState = std::exchange(other.odeState, odeState);
    nextOdeState = std::exchange(other.nextOdeState, nextOdeState);
    currentExternalStimulus = std::exchange(other.currentExternalStimulus, currentExternalStimulus);
    id.swap(other.id);
}

K0::K0(K0Config config) noexcept:
    activationHistory(config.historySize), sigmoidQ(config.sigmoidQ) {}

K0::K0(K0Collection& collection, std::size_t id, K0Config config) noexcept:
    activationHistory(config.historySize), sigmoidQ(config.sigmoidQ), collection(collection), id(id) {}

K0::K0(const K0& other) noexcept:
    activationHistory(other.activationHistory),
    sigmoidQ(other.sigmoidQ),
    inboundConnections(),
    odeState(other.odeState),
    nextOdeState(other.nextOdeState),
    currentExternalStimulus(other.currentExternalStimulus),
    // collection is ommited on purpose! this requires extra care
    // when cloning since you want to clone the entire subgraph and create
    // a collection based on that
    id(other.id) {}

K0::K0(K0&& other) noexcept {
    swap(other);
    // destructor for other (old *this) runs here
}

K0& K0::operator=(const K0& other) noexcept {
    K0 clone(other);
    swap(clone);
    return *this;
}

K0& K0::operator=(K0&& other) noexcept {
    swap(other);
    return *this;
    // destructor for other (old *this) runs here
}

void K0::setHistorySize(std::size_t nIter) {
    activationHistory.resize(nIter);
}

void K0::setActivityMonitoring(std::size_t nIter) {
    activationHistory.setActivityMonitoring(nIter);
}

void K0::setCollection(K0Collection& collection) noexcept {
    this->collection = collection;
}

void K0::setId(std::size_t id) noexcept {
    this->id = id;
}

// translation unit "private" function
namespace {
    void doCloneSubgraph(std::map<const K0 *, std::shared_ptr<K0>>& oldToNew, const K0 *current) noexcept {
        std::shared_ptr<K0> newCurrent = std::shared_ptr<K0>(new K0(*current));
        oldToNew.insert(std::make_pair(current, newCurrent));
        for (auto& conn : *current) {
            std::shared_ptr<K0> other = conn.source;
            if (oldToNew.find(other.get()) == oldToNew.end())
                doCloneSubgraph(oldToNew, other.get());
            std::shared_ptr<K0> newOther = oldToNew.at(other.get());
            newCurrent->addInboundConnection(newOther, conn.weight, conn.delay, conn.tag);
        }
    }
}

std::map<const K0 *, std::shared_ptr<K0>> K0::cloneSubgraph() const noexcept {
    std::map<const K0 *, std::shared_ptr<K0>> oldToNew;
    cloneSubgraph(oldToNew);
    return oldToNew;
}

void K0::cloneSubgraph(std::map<const K0 *, std::shared_ptr<K0>>& partialMapping) const noexcept {
    doCloneSubgraph(partialMapping, this);
}

std::string K0::repr() const noexcept {
    std::stringstream s;
    s << "node";
    if (id.has_value()) {
        s << ' ' << id.value();
    }

    if (collection.has_value()) {
        s << " @ ";
        if (collection.value().get().getName().has_value())
            s << collection.value().get().getName().value();
        else
            s << "<nameless collection>";
    }

    return s.str();
}


numeric K0::calculateNetInput() noexcept {
    numeric accumulation = currentExternalStimulus;
    accumulation += currentInputNoise;
    for (auto& connection : inboundConnections)
        accumulation += connection.weight * connection.source->getDelayedOutput(connection.delay);
    return accumulation;
}

void K0::addInboundConnection(
    std::shared_ptr<K0> source,
    numeric weight,
    std::size_t delay,
    std::optional<conntag> tag
) noexcept {
    inboundConnections.emplace_back(source, this, weight, delay, tag);
}

void K0::clearInboundConnections() noexcept {
    inboundConnections.clear();
}

numeric K0::getCurrentOutput() const noexcept {
    return getDelayedOutput(0);
}

numeric K0::getDelayedOutput(std::size_t delay) const noexcept {
    return activationHistory.get(delay);
}

void K0::setExternalStimulus(numeric newExternalStimulus) noexcept {
    currentExternalStimulus = newExternalStimulus;
}

void K0::setRngEngine(std::function<numeric()> newEngine) {
    noiseRng = newEngine;
    advanceNoise();
}

void K0::advanceNoise() {
    if (!noiseRng.has_value()) {
        std::stringstream errMsg("Tried to advance noise on a node with no noise engine");
        if (collection.has_value() && collection.value().get().hasName())
            errMsg << ": collection \"" << collection.value().get().getName().value() << "\"";
        if (id.has_value())
            errMsg << " (node ID: " << id.value() << ")";
        throw std::logic_error(errMsg.str());
    }
    auto engine = noiseRng.value();
    currentInputNoise = engine();
}

numeric odeF1(numeric x, numeric dx_dt, numeric totalStimulus) noexcept {
    (void) x;
    (void) totalStimulus;
    return dx_dt;
}

numeric odeF2(numeric x, numeric dx_dt, numeric totalStimulus) noexcept {
    using ksets::ODE_A_DECAY_RATE, ksets::ODE_B_RISE_RATE;
    return (-(ODE_A_DECAY_RATE+ODE_B_RISE_RATE)*dx_dt) + (ODE_A_DECAY_RATE*ODE_B_RISE_RATE*(totalStimulus - x));
}

void K0::calculateNextState() noexcept {
    numeric totalStimulus = calculateNetInput();
    numeric k1 = odeF1(odeState[0], odeState[1], totalStimulus) * ODE_STEP_SIZE;
    numeric l1 = odeF2(odeState[0], odeState[1], totalStimulus) * ODE_STEP_SIZE;

    numeric k2 = odeF1(odeState[0] + k1/2, odeState[1] + l1/2, totalStimulus) * ODE_STEP_SIZE;
    numeric l2 = odeF2(odeState[0] + k1/2, odeState[1] + l1/2, totalStimulus) * ODE_STEP_SIZE;

    numeric k3 = odeF1(odeState[0] + k2/2, odeState[1] + l2/2, totalStimulus) * ODE_STEP_SIZE;
    numeric l3 = odeF2(odeState[0] + k2/2, odeState[1] + l2/2, totalStimulus) * ODE_STEP_SIZE;

    numeric k4 = odeF1(odeState[0] + k3, odeState[1] + l3, totalStimulus) * ODE_STEP_SIZE;
    numeric l4 = odeF2(odeState[0] + k3, odeState[1] + l3, totalStimulus) * ODE_STEP_SIZE;

    nextOdeState = odeState;
    nextOdeState[0] += (k1 + 2*k2 + 2*k3 + k4) / 6;
    nextOdeState[1] += (l1 + 2*l2 + 2*l3 + l4) / 6;
}

void K0::calculateNextState(numeric newExternalStimulus) noexcept {
    setExternalStimulus(newExternalStimulus);
    calculateNextState();
}

void K0::calculateAndCommitNextState() noexcept {
    calculateNextState();
    commitNextState();
}

void K0::calculateAndCommitNextState(numeric newExternalStimulus) noexcept {
    calculateNextState(newExternalStimulus);
    commitNextState();
}

void K0::commitNextState() noexcept {
    odeState = nextOdeState;
    pushOutputToHistory();
}

void K0::pushOutputToHistory() noexcept {
    activationHistory.put(sigmoid(odeState[0], sigmoidQ));
}

const ksets::ActivationHistory& K0::getActivationHistory() const noexcept {
    return activationHistory;
}

void K0::randomizeState(std::function<numeric()>& rng) noexcept {
    odeState[0] = rng();
}

void K0Collection::initNodes(std::size_t nNodes, const K0Config& config) {
    if (nNodes == 0)
        throw std::invalid_argument("Number of nodes cannot be 0");
    for (std::size_t i = 0; i < nNodes; i++)
        nodes.push_back(std::make_shared<K0>(*this, i, config));
}

K0Collection::K0Collection(
    std::size_t nNodes,
    std::optional<std::string> name,
    K0Config config
) {
    this->name.swap(name);
    initNodes(nNodes, config);
}

K0Collection::K0Collection(const K0Collection& other) noexcept {
    std::map<const K0 *, std::shared_ptr<K0>> oldToNew = other.primaryNode()->cloneSubgraph();
    for (const std::shared_ptr<K0> oldNode : other) {
        if (oldToNew.find(oldNode.get()) == oldToNew.end())
            oldNode->cloneSubgraph(oldToNew);
        nodes.push_back(oldToNew.at(oldNode.get()));
    }
}

K0Collection::~K0Collection() noexcept {
    for (auto node : nodes)
        node->clearInboundConnections();
}

std::shared_ptr<K0> K0Collection::node(std::size_t index) {
    return nodes.at(index);
}

const std::shared_ptr<K0> K0Collection::node(std::size_t index) const {
    return nodes.at(index);
}

std::size_t K0Collection::size() const noexcept {
    return nodes.size();
}

void K0Collection::updateNodeCollectionReferenceAndId() noexcept {
    for (std::size_t i = 0; i < size(); i++) {
        nodes[i]->setCollection(*this);
        nodes[i]->setId(i);
    }
}

void K0Collection::setExternalStimulus(numeric newExternalStimulus) noexcept {
    primaryNode()->setExternalStimulus(newExternalStimulus);
}

void K0Collection::calculateNextState() noexcept {
    for (auto node : nodes)
        node->calculateNextState();
}

void K0Collection::calculateNextState(numeric newExternalStimulus) noexcept {
    setExternalStimulus(newExternalStimulus);
    calculateNextState();
}

void K0Collection::commitNextState() noexcept {
    for (auto node : nodes)
        node->commitNextState();
}

void K0Collection::calculateAndCommitNextState() noexcept {
    calculateNextState();
    commitNextState();
}

void K0Collection::calculateAndCommitNextState(numeric newExternalStimulus) noexcept {
    calculateNextState(newExternalStimulus);
    commitNextState();
}

void K0Collection::randomizeK0States(std::function<numeric()>& rng) {
    for (auto& k0 : nodes)
        k0->randomizeState(rng);
}
