#include "ksets/k2layer.hpp"

using ksets::K2, ksets::K2Layer, ksets::ActivationHistory, ksets::numeric;

K2Layer::K2Layer(
    std::size_t nUnits,
    K2Config k2config
):
    avgPrimaryActivation(k2config.k0config.historySize),
    avgAntipodalActivation(k2config.k0config.historySize)
{
    if (nUnits == 0)
        throw std::invalid_argument("Number of units cannot be 0");
    for (std::size_t i = 0; i < nUnits; i++)
        units.emplace_back(k2config);
}

bool K2Layer::connectPrimaryNodesLaterally(numeric weight, std::size_t delay) noexcept {
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

bool K2Layer::connectAntipodalNodesLaterally(numeric weight, std::size_t delay) noexcept {
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

void K2Layer::setPrimaryHistorySize(std::size_t newSize) {
    avgPrimaryActivation.resize(newSize);
    for (auto& unit : *this)
        unit.primaryNode()->setHistorySize(newSize);
}

void K2Layer::setAntipodalHistorySize(std::size_t newSize) {
    avgAntipodalActivation.resize(newSize);
    for (auto& unit : *this)
        unit.antipodalNode()->setHistorySize(newSize);
}

void K2Layer::setPrimaryActivityMonitoring(std::size_t newSize) {
    avgPrimaryActivation.setActivityMonitoring(newSize);
    for (auto& unit : *this)
        unit.primaryNode()->setActivityMonitoring(newSize);
}

void K2Layer::setAntipodalActivityMonitoring(std::size_t newSize) {
    avgAntipodalActivation.setActivityMonitoring(newSize);
    for (auto& unit : *this)
        unit.antipodalNode()->setActivityMonitoring(newSize);
}

const ActivationHistory& K2Layer::getAveragePrimaryActivationHistory() const noexcept {
    return avgPrimaryActivation;
}

const ActivationHistory& K2Layer::getAverageAntipodalActivationHistory() const noexcept {
    return avgAntipodalActivation;
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
    numeric primarySum = 0;
    numeric antipodalSum = 0;
    for (auto& unit : units) {
        unit.commitNextState();
        primarySum += unit.primaryNode()->getActivationHistory().get(0);
        antipodalSum += unit.antipodalNode()->getActivationHistory().get(0);
    }
    avgPrimaryActivation.put(primarySum / size());
    avgAntipodalActivation.put(antipodalSum / size());
}

void K2Layer::calculateAndCommitNextState() noexcept {
    calculateNextState();
    commitNextState();
}

bool K2Layer::calculateAndCommitNextState(std::initializer_list<numeric> newExternalStimulus) noexcept {
    return calculateAndCommitNextState(newExternalStimulus.begin(), newExternalStimulus.end());
}

void K2Layer::randomizeK0States(std::function<numeric()>& rng) {
    for (auto &unit : units)
        unit.randomizeK0States(rng);
}