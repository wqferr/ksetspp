#include "ksets/k3.hpp"
#include <random>
#include <sstream>

using ksets::K0, ksets::K2, ksets::K2Layer, ksets::K3, ksets::K0Config;
using ksets::K2Config, ksets::K3Config, ksets::numeric;

namespace {
    std::function<numeric()> createGaussianRng(numeric stdDev) {
        std::random_device rd {};
        std::mt19937 engine {rd()};
        std::normal_distribution<numeric> dist {0.00, stdDev};
        return [=]() mutable {return dist(engine);};
    }

    K0Config ponConfig(const K3Config& k3config) {
        return {k3config.nonOutputHistorySize};
    }

    K2Config obConfig(const K3Config& k3config) {
        auto k2config = k3config.wOB_unitConfig;
        k2config.k0config = {k3config.nonOutputHistorySize};
        return k2config;
    }

    K2Config aonConfig(const K3Config& k3config) {
        K2Config k2config = k3config.wAON_unitConfig;
        k2config.k0config = {k3config.nonOutputHistorySize};
        return k2config;
    }

    K2Config pcConfig(const K3Config& k3config) {
        K2Config k2config = k3config.wPC_unitConfig;
        k2config.k0config = {k3config.nonOutputHistorySize};
        return k2config;
    }

    K0Config dpcConfig(const K3Config& k3config) {
        return {k3config.outputHistorySize};
    }
}

K3::K3(std::size_t olfactoryBulbNumUnits, numeric initialRestMilliseconds, std::function<numeric()> rng, ksets::K3Config config):
    periglomerularCells(olfactoryBulbNumUnits, std::nullopt, ponConfig(config)),
    olfactoryBulb(olfactoryBulbNumUnits, obConfig(config)),
    anteriorOlfactoryNucleus(aonConfig(config)),
    prepiriformCortex(pcConfig(config)),
    deepPyramidCells(new K0(dpcConfig(config))),
    obPrimaryNodes(olfactoryBulbNumUnits),
    obAntipodalNodes(olfactoryBulbNumUnits)
{
    if (!config.checkWeightsValidity())
        throw std::invalid_argument("One or more K3 weights were invalid.");
    nameAllSubcomponents();
    connectAllSubcomponents(config);
    randomizeK0States(rng);

    aonStimulusRng = createGaussianRng(config.wAON_noise);
    advanceAonNoise();

    cachePrimaryAndAntipodalOlfactoryBulbNodes();
    olfactoryBulb.setPrimaryHistorySize(config.outputHistorySize);
    olfactoryBulb.setPrimaryActivityMonitoring(config.outputActivityMonitoring);
    olfactoryBulb.setAntipodalHistorySize(config.outputHistorySize);
    olfactoryBulb.setAntipodalActivityMonitoring(config.outputActivityMonitoring);
    prepiriformCortex.primaryNode()->setHistorySize(config.outputHistorySize);
    prepiriformCortex.primaryNode()->setActivityMonitoring(config.outputActivityMonitoring);

    rest(initialRestMilliseconds);
}

K3::K3(std::size_t olfactoryBulbNumUnits, numeric initialRestMilliseconds, ksets::K3Config config)
    : K3(
        olfactoryBulbNumUnits,
        initialRestMilliseconds,
        createGaussianRng(ksets::defaultK3RandomK0InitializationStdDev),
        config
    ) {}

void K3::cachePrimaryAndAntipodalOlfactoryBulbNodes() noexcept {
    for (std::size_t i = 0; i < olfactoryBulb.size(); i++) {
        obPrimaryNodes[i] = olfactoryBulb.unit(i).primaryNode();
        obAntipodalNodes[i] = olfactoryBulb.unit(i).antipodalNode();
    }
}

void K3::randomizeK0States(std::function<numeric()>& rng) noexcept {
    for (auto pnUnit : periglomerularCells)
        pnUnit->randomizeState(rng);
    olfactoryBulb.randomizeK0States(rng);
    anteriorOlfactoryNucleus.randomizeK0States(rng);
    prepiriformCortex.randomizeK0States(rng);
    deepPyramidCells->randomizeState(rng);
}

void K3::calculateNextState() noexcept {
    // TODO: add noise to input nodes
    for (auto pnUnit : periglomerularCells)
        pnUnit->calculateNextState();
    olfactoryBulb.calculateNextState();
    anteriorOlfactoryNucleus.calculateNextState();
    prepiriformCortex.calculateNextState();
    deepPyramidCells->calculateNextState();
}

void K3::commitNextState() noexcept {
    for (auto pnUnit : periglomerularCells)
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

void K3::advanceAonNoise() noexcept {
    anteriorOlfactoryNucleus.setExternalStimulus(aonStimulusRng());
}

void K3::eraseExternalStimulus() noexcept {
    auto pnIter = periglomerularCells.begin();
    auto obIter = olfactoryBulb.begin();
    while (pnIter != periglomerularCells.end()) {
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

void K3::nameAllSubcomponents() noexcept {
    periglomerularCells.setName("Primary olfactory nerve (input layer)");
    for (std::size_t i = 0; i < olfactoryBulb.size(); i++) {
        std::stringstream unitName("Olfactory bulb (K2 layer 1) unit ");
        unitName << i;
        olfactoryBulb.unit(i).setName(unitName.str());
    }
    anteriorOlfactoryNucleus.setName("Anterior olfactory nucleus (K2 layer 2)");
    prepiriformCortex.setName("Prepiriform cortex (K2 layer 3)");
}

void K3::connectAllSubcomponents(const K3Config& config) noexcept {
    connectPrimaryOlfactoryNerveLaterally(config.wPG_interUnit);
    olfactoryBulb.connectPrimaryNodesLaterally(config.wOB_inter[0]);
    olfactoryBulb.connectAntipodalNodesLaterally(config.wOB_inter[1]);
    connectLayers(config);
}

void K3::connectPrimaryOlfactoryNerveLaterally(numeric weight, std::size_t delay) noexcept {
    for (auto it1 = periglomerularCells.begin(); it1 != periglomerularCells.end(); it1++) {
        for (auto it2 = it1 + 1; it2 != periglomerularCells.end(); it2++) {
            (*it1)->addInboundConnection(*it2, weight, delay);
            (*it2)->addInboundConnection(*it1, weight, delay);
        }
    }
}

void K3::connectLayers(const K3Config& config) noexcept {
    auto pnIter = periglomerularCells.begin();
    auto obIter = olfactoryBulb.begin();
    while (pnIter != periglomerularCells.end()) {
        assert(obIter != olfactoryBulb.end());
        auto& pnUnit = *pnIter;
        auto& obUnit = *obIter;

        obUnit.primaryNode()->addInboundConnection(pnUnit, 1.0, 0);

        // AON -> PON connections
        pnUnit->addInboundConnection(
            anteriorOlfactoryNucleus.primaryNode(),
            config.wAON_PG_mot,
            config.dAON_PG_mot);

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

        pnIter++;
        obIter++;
    }

    // Singled out connections, not part of any major chain
    anteriorOlfactoryNucleus.antipodalNode()->addInboundConnection(
        prepiriformCortex.primaryNode(),
        config.wPC_AON_toAntipodal,
        config.dPC_AON_toAntipodal);

    // Prepiriform cortex -> deep pyramid cells
    deepPyramidCells->addInboundConnection(
        prepiriformCortex.antipodalNode(),
        config.wPC_DPC,
        config.dPC_DPC);

    // Deep pyramid cells -> prepiriform cortex
    prepiriformCortex.antipodalNode()->addInboundConnection(
        deepPyramidCells,
        config.wDPC_PC,
        config.dDPC_PC);
}

const K2Layer& K3::getOlfactoryBulb() const noexcept {
    return olfactoryBulb;
}

const std::vector<std::shared_ptr<const K0>>& K3::getOlfactoryBulbPrimaryNodes() const noexcept {
    return obPrimaryNodes;
}

const std::vector<std::shared_ptr<const K0>>& K3::getOlfactoryBulbAntipodalNodes() const noexcept {
    return obAntipodalNodes;
}

const std::shared_ptr<const K0> K3::getPrepiriformCortexPrimary() const noexcept {
    return prepiriformCortex.primaryNode();
}


const std::shared_ptr<const K0> K3::getDeepPyramidCells() const noexcept {
    return deepPyramidCells;
}
