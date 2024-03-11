#include "ksets/k3.hpp"
#include <random>

using ksets::K0, ksets::K2, ksets::K2Layer, ksets::K3, ksets::numeric;

namespace {
    std::function<numeric()> createGaussianRng(numeric stdDev) {
        std::random_device rd {};
        std::mt19937 engine {rd()};
        std::normal_distribution<numeric> dist {0.00, stdDev};
        return [=]() mutable {return dist(engine);};
    }
}

explicit K3::K3(std::size_t olfactoryBulbNumUnits, numeric initialRestMilliseconds, std::function<numeric()> rng, ksets::K3Config config):
    primaryOlfactoryNerve(olfactoryBulbNumUnits),
    olfactoryBulb(olfactoryBulbNumUnits, config.wOB_intra),
    anteriorOlfactoryNucleus(config.wAON_intra),
    prepiriformCortex(config.wPC_intra),
    deepPyramidCells(new K0(olfactoryBulbNumUnits))
{
    if (!config.checkWeightsValidity())
        throw std::invalid_argument("One or more K3 weights were invalid.");
    for (std::size_t i = 0; i < olfactoryBulbNumUnits; i++)
        primaryOlfactoryNerve[i] = std::make_shared<K0>(i);
    connectAllSubcomponents(config);
    randomizeK0States(rng);

    aonStimulusRng = createGaussianRng(config.wAON_noise);
    advanceAonNoise();

    // TODO: enable activity tracking in relevant nodes
    rest(initialRestMilliseconds);
}

explicit K3::K3(std::size_t olfactoryBulbNumUnits, numeric initialRestMilliseconds, ksets::K3Config config)
    : K3(
        olfactoryBulbNumUnits,
        initialRestMilliseconds,
        createGaussianRng(ksets::defaultK3RandomK0InitializationStdDev),
        config
    ) {}

void K3::randomizeK0States(std::function<numeric()> rng) {
    for (auto pnUnit : primaryOlfactoryNerve)
        pnUnit->randomizeState(rng);
    olfactoryBulb.randomizeK0States(rng);
    anteriorOlfactoryNucleus.randomizeK0States(rng);
    prepiriformCortex.randomizeK0States(rng);
    deepPyramidCells->randomizeState(rng);
}

void K3::calculateNextState() noexcept {
    for (auto pnUnit : primaryOlfactoryNerve)
        pnUnit->calculateNextState();
    olfactoryBulb.calculateNextState();
    anteriorOlfactoryNucleus.calculateNextState();
    prepiriformCortex.calculateNextState();
    deepPyramidCells->calculateNextState();
}

void K3::commitNextState() noexcept {
    for (auto pnUnit : primaryOlfactoryNerve)
        pnUnit->commitNextState();
    olfactoryBulb.commitNextState();
    anteriorOlfactoryNucleus.commitNextState();
    prepiriformCortex.commitNextState();
    deepPyramidCells->commitNextState();
    advanceAonNoise();
}

void K3::calculateAndCommitNextState() noexcept {
    calculateNextState();
    commitNextState();
}

void K3::advanceAonNoise() {
    anteriorOlfactoryNucleus.setExternalStimulus(aonStimulusRng());
}

void K3::eraseExternalStimulus() noexcept {
    auto pnIter = primaryOlfactoryNerve.begin();
    auto obIter = olfactoryBulb.begin();
    while (pnIter != primaryOlfactoryNerve.end()) {
        (*pnIter)->setExternalStimulus(0);
        obIter->setExternalStimulus(0);
        pnIter++;
        obIter++;
    }
}

void K3::run(numeric milliseconds) noexcept {
    std::size_t iterations = ksets::odeMillisecondsToIters(milliseconds);
    for (std::size_t i = 0; i < iterations; i++)
        calculateAndCommitNextState();
}

void K3::rest(numeric milliseconds) noexcept {
    eraseExternalStimulus();
    run(milliseconds);
}

void K3::connectAllSubcomponents(const K3Config& config) {
    connectPrimaryOlfactoryNerveLaterally(config.wPON_interUnit);
    olfactoryBulb.connectPrimaryNodes(config.wOB_inter[0]);
    olfactoryBulb.connectAntipodalNodes(config.wOB_inter[1]);
    connectLayers(config);
}

void K3::connectPrimaryOlfactoryNerveLaterally(numeric weight, std::size_t delay) {
    for (auto it1 = primaryOlfactoryNerve.begin(); it1 != primaryOlfactoryNerve.end(); it1++) {
        for (auto it2 = it1 + 1; it2 != primaryOlfactoryNerve.end(); it2++) {
            (*it1)->addInboundConnection(*it2, weight, delay);
            (*it2)->addInboundConnection(*it1, weight, delay);
        }
    }
}

void K3::connectLayers(const K3Config& config) {
    auto pnIter = primaryOlfactoryNerve.begin();
    auto obIter = olfactoryBulb.begin();
    while (pnIter != primaryOlfactoryNerve.end()) {
        assert(obIter != olfactoryBulb.end());
        auto pnUnit = *pnIter;
        auto obUnit = *obIter;

        obUnit.primaryNode()->addInboundConnection(pnUnit, 1.0, 0);

        // AON -> PON connections
        pnUnit->addInboundConnection(
            anteriorOlfactoryNucleus.primaryNode(),
            config.wAON_PON_mot,
            config.dAON_PON_mot);

        // LOT connections, left on the diagram
        anteriorOlfactoryNucleus.primaryNode()->addInboundConnection(
            obUnit.primaryNode(), config.wOB_AON_lot, config.dOB_AON_lot);
        prepiriformCortex.primaryNode()->addInboundConnection(
            obUnit.primaryNode(), config.wOB_PC_lot, config.dOB_PC_lot);

        // MOT connections, right on the diagram
        obUnit.antipodalNode()->addInboundConnection(
            anteriorOlfactoryNucleus.primaryNode(),
            config.wAON_OB_toAntipodal,
            config.dAON_OB_toAntipodal);
        obUnit.antipodalNode()->addInboundConnection(
            deepPyramidCells, config.wDPC_OB_toAntipodal, config.dDPC_OB_toAntipodal);

    }
    // Singled out connections, not part of any major chain
    anteriorOlfactoryNucleus.antipodalNode()->addInboundConnection(
        prepiriformCortex.primaryNode(),
        config.wPC_AON_toAntipodal,
        config.dPC_AON_toAntipodal);
}