#include "ksets/k2layer.hpp"

using ksets::K2, ksets::K2Layer, ksets::numeric;

explicit K2Layer::K2Layer(
    std::size_t nUnits,
    const K2Weights intraUnitWeights
)
    // : units(nUnits, K2(intraUnitWeights)) --> cant copy construct K2s
{
    if (nUnits == 0)
        throw std::invalid_argument("Number of units cannot be 0");
    for (std::size_t i = 0; i < nUnits; i++)
        units.emplace_back(intraUnitWeights);
}

bool K2Layer::connectPrimaryNodes(numeric weight, std::size_t delay) noexcept {
    if (weight < 0) return false;
    if (size() > 1) weight /= size() - 1;
    for (auto it1 = begin(); it1 != end(); it1++) {
        for (auto it2 = it1 + 1; it2 != end(); it2++) {
            it1->primaryNode()->addInboundConnection(it2->primaryNode(), weight, delay);
            it2->primaryNode()->addInboundConnection(it1->primaryNode(), weight, delay);
        }
    }
    return true;
}

bool K2Layer::connectAntipodalNodes(numeric weight, std::size_t delay) noexcept {
    if (weight > 0) return false;
    if (size() > 1) weight /= size() - 1;
    for (auto it1 = begin(); it1 != end(); it1++) {
        for (auto it2 = it1 + 1; it2 != end(); it2++) {
            it1->antipodalNode()->addInboundConnection(it2->antipodalNode(), weight, delay);
            it2->antipodalNode()->addInboundConnection(it1->antipodalNode(), weight, delay);
        }
    }
    return true;
}

std::size_t K2Layer::size() const noexcept {
    return units.size();
}

K2& K2Layer::unit(std::size_t index) {
    return units.at(index);
}

const K2& K2Layer::unit(std::size_t index) const {
    return units.at(index);
}

bool K2Layer::setExternalStimulus(std::initializer_list<numeric> values) noexcept {
    return setExternalStimulus(values.begin(), values.end());
}

void K2Layer::calculateNextState() noexcept {
    for (auto& unit : units)
        unit.calculateNextState();
}

bool K2Layer::calculateNextState(std::initializer_list<numeric> newExternalStimulus) noexcept {
    return calculateNextState(newExternalStimulus.begin(), newExternalStimulus.end());
}

void K2Layer::commitNextState() noexcept {
    for (auto& unit : units)
        unit.commitNextState();
}

void K2Layer::calculateAndCommitNextState() noexcept {
    calculateNextState();
    commitNextState();
}

bool K2Layer::calculateAndCommitNextState(std::initializer_list<numeric> newExternalStimulus) noexcept {
    return calculateAndCommitNextState(newExternalStimulus.begin(), newExternalStimulus.end());
}

void K2Layer::randomizeK0States(const std::function<numeric()>& rng) {
    for (auto &unit : units)
        unit.randomizeK0States(rng);
}