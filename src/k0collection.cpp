#include "ksets/k0collection.hpp"

using ksets::K0Collection, ksets::K0;

K0& K0Collection::node(std::size_t index) {
    return nodes.at(index);
}

const K0& K0Collection::node(std::size_t index) const {
    return nodes.at(index);
}

void K0Collection::setExternalStimulus(numeric newExternalStimulus) {
    primaryNode().setExternalStimulus(newExternalStimulus);
}

void K0Collection::calculateNextState() {
    for (auto& node : nodes)
        node.calculateNextState();
}

void K0Collection::calculateNextState(numeric newExternalStimulus) {
    setExternalStimulus(newExternalStimulus);
    calculateNextState();
}

void K0Collection::commitNextState() {
    for (auto& node : nodes)
        node.commitNextState();
}

void K0Collection::calculateAndCommitNextState() {
    calculateNextState();
    commitNextState();
}

void K0Collection::calculateAndCommitNextState(numeric newExternalStimulus) {
    calculateNextState(newExternalStimulus);
    commitNextState();
}
