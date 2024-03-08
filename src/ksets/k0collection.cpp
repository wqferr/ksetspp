#include "ksets/k0collection.hpp"

using ksets::K0Collection, ksets::K0;

K0Collection::K0Collection(std::size_t nNodes) {
    for (std::size_t i = 0; i < nNodes; i++)
        nodes.push_back(std::make_shared<K0>(i));
}

K0Collection::K0Collection(const K0Collection& other) {
    std::map<const K0 *, std::shared_ptr<K0>> oldToNew = other.primaryNode()->cloneSubgraph();
    for (const std::shared_ptr<K0> oldNode : other) {
        if (oldToNew.find(oldNode.get()) == oldToNew.end()) {
            std::map<const K0*, std::shared_ptr<K0>> extension = oldNode->cloneSubgraph();
            oldToNew.insert(extension.begin(), extension.end());
        }
        nodes.push_back(oldToNew.at(oldNode.get()));
    }
}

K0Collection::~K0Collection() {
    for (auto node : nodes)
        node->clearInboundConnections();
}

std::shared_ptr<K0> K0Collection::node(std::size_t index) {
    return nodes.at(index);
}

const std::shared_ptr<K0> K0Collection::node(std::size_t index) const {
    return nodes.at(index);
}

std::size_t K0Collection::size() const {
    return nodes.size();
}

void K0Collection::setExternalStimulus(numeric newExternalStimulus) {
    primaryNode()->setExternalStimulus(newExternalStimulus);
}

void K0Collection::calculateNextState() {
    for (auto node : nodes)
        node->calculateNextState();
}

void K0Collection::calculateNextState(numeric newExternalStimulus) {
    setExternalStimulus(newExternalStimulus);
    calculateNextState();
}

void K0Collection::commitNextState() {
    for (auto node : nodes)
        node->commitNextState();
}

void K0Collection::calculateAndCommitNextState() {
    calculateNextState();
    commitNextState();
}

void K0Collection::calculateAndCommitNextState(numeric newExternalStimulus) {
    calculateNextState(newExternalStimulus);
    commitNextState();
}
