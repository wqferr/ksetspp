#include "ksets/k2layer.hpp"

using ksets::K2, ksets::K2Layer, ksets::numeric;

K2Layer::K2Layer(
    std::size_t nUnits,
    const K2Weights intraUnitWeights
)
    // : units(nUnits, K2(intraUnitWeights)) --> cant copy construct K2s
{
    for (std::size_t i = 0; i < nUnits; i++) {
        units.emplace_back(intraUnitWeights);
    }
}

bool K2Layer::connectPrimaryNodes(numeric weight, std::size_t delay) {
    if (weight < 0) return false;
    auto it1 = begin();
    for (auto it1 = begin(); it1 < end(); it1++) {
        auto it2 = it1 + 1;
        while (it2 < end()) {
            it1->primaryNode()->addInboundConnection(it2->primaryNode(), weight, delay);
            it2->primaryNode()->addInboundConnection(it1->primaryNode(), weight, delay);
            it2++;
        }
    }
    return true;
}

bool K2Layer::connectAntipodalNodes(numeric weight, std::size_t delay) {
    if (weight > 0) return false;
    auto it1 = begin();
    for (auto it1 = begin(); it1 < end(); it1++) {
        auto it2 = it1 + 1;
        while (it2 < end()) {
            it1->antipodalNode()->addInboundConnection(it2->antipodalNode(), weight, delay);
            it2->antipodalNode()->addInboundConnection(it1->antipodalNode(), weight, delay);
            it2++;
        }
    }
    return true;
}

std::size_t K2Layer::size() const {
    return units.size();
}

K2& K2Layer::unit(std::size_t index) {
    return units.at(index);
}

const K2& K2Layer::unit(std::size_t index) const {
    return units.at(index);
}

bool K2Layer::setExternalStimulus(std::initializer_list<numeric> values) {
    return setExternalStimulus(values.begin(), values.end());
}

void K2Layer::calculateNextState() {
    for (auto& unit : units)
        unit.calculateNextState();
}

bool K2Layer::calculateNextState(std::initializer_list<numeric> newExternalStimulus) {
    return calculateNextState(newExternalStimulus.begin(), newExternalStimulus.end());
}

void K2Layer::commitNextState() {
    for (auto& unit : units)
        unit.commitNextState();
}

void K2Layer::calculateAndCommitNextState() {
    calculateNextState();
    commitNextState();
}

bool K2Layer::calculateAndCommitNextState(std::initializer_list<numeric> newExternalStimulus) {
    return calculateAndCommitNextState(newExternalStimulus.begin(), newExternalStimulus.end());
}
