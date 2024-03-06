#include "ksets/k0.hpp"

#include <numeric>
#include <tgmath.h>
#include <cassert>

#ifdef DEBUG
#include <iostream>
#endif

using ksets::K0;
using ksets::numeric;

K0::K0(numeric learningRate): learningRate(learningRate) {}

numeric K0::calculateNetInput() {
    numeric accumulation = currentExternalStimulus;
    for (auto& connection : inboundConnections)
        accumulation += connection.weight * connection.source.getDelayedOutput(connection.delay);
    return accumulation;
}

void K0::addInboundConnection(const K0& source, numeric weight, std::size_t delay) {
    inboundConnections.emplace_back(source, weight, delay);
}

numeric K0::getCurrentOutput() const {
    return getDelayedOutput(0);
}

numeric K0::getDelayedOutput(std::size_t delay) const {
    return activationHistory.get(delay);
}

void K0::hebbianReinforcementIteration() {
    // TODO: stuff with the learning rate and hebbian learning
}

void K0::setExternalStimulus(numeric newStimulus) {
    currentExternalStimulus = newStimulus;
}

numeric odeF1(numeric x, numeric dx_dt, numeric totalStimulus) {
    (void) x;
    (void) totalStimulus;
    return dx_dt;
}

numeric odeF2(numeric x, numeric dx_dt, numeric totalStimulus) {
    using ksets::ODE_A_DECAY_RATE, ksets::ODE_B_RISE_RATE;
    return (-(ODE_A_DECAY_RATE+ODE_B_RISE_RATE)*dx_dt) + (ODE_A_DECAY_RATE*ODE_B_RISE_RATE*(totalStimulus - x));
}

void K0::calculateNextState() {
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

void K0::calculateNextState(numeric newExternalStimulus) {
    setExternalStimulus(newExternalStimulus);
    calculateNextState();
}

void K0::calculateAndCommitNextState() {
    calculateNextState();
    commitNextState();
}

void K0::calculateAndCommitNextState(numeric newExternalStimulus) {
    calculateNextState(newExternalStimulus);
    commitNextState();
}

void K0::commitNextState() {
    odeState = nextOdeState;
    pushOutputToHistory();
}

void K0::pushOutputToHistory() {
    activationHistory.put(ksets::sigmoid(odeState[0]));
}

const ksets::ActivationHistory& K0::getActivationHistory() const {
    return activationHistory;
}
