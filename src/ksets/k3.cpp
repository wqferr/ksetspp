#include "ksets/k3.hpp"
#include <random>
#include <sstream>
#include <utility>
#include <memory>

using ksets::K0, ksets::K1, ksets::K2, ksets::K2Layer, ksets::K3;
using ksets::K0Config, ksets::K1Config, ksets::K2Config, ksets::K3Config;
using ksets::rngseed, ksets::numeric;

namespace {
    std::function<numeric()> createGaussianRng(numeric stdDev, rngseed seed) {
        std::mt19937 engine {seed};
        std::normal_distribution<numeric> dist {0.00, stdDev};
        return [engine = std::move(engine), dist]() mutable {return dist(engine);};
    }

    std::function<rngseed()> randomDeviceSeedGenerator() {
        std::deque<rngseed> batch(32, 0);
        std::size_t i = batch.size() - 1;
        return [batch = std::move(batch), i]() mutable {
            static std::random_device rd {};
            static std::seed_seq seedGen {rd(), rd(), rd(), rd()};

            i++;
            if (i == batch.size()) {
                seedGen.generate(batch.begin(), batch.end());
                i = 0;
            }
            return batch[i];
        };
    }

    K1Config pgConfig(const K3Config& k3config) {
        K1Config k1config = {k3config.wPG_intraUnit};
        k1config.k0config = {k3config.nonOutputHistorySize};
        return k1config;
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

K3::K3(std::size_t olfactoryBulbNumUnits, numeric initialRestMilliseconds, std::function<rngseed()> seedGen, ksets::K3Config config):
    periglomerularCells(olfactoryBulbNumUnits, K1(pgConfig(config))),
    olfactoryBulb(olfactoryBulbNumUnits, obConfig(config)),
    anteriorOlfactoryNucleus(aonConfig(config)),
    prepiriformCortex(pcConfig(config)),
    deepPyramidCells(new K0(dpcConfig(config)))
{
    if (!config.checkWeightsValidity())
        throw std::invalid_argument("One or more K3 weights were invalid.");
    nameAllSubcomponents();
    connectAllSubcomponents(config);

    auto initRng = createGaussianRng(K3_RANDOM_K0_INIT_STD_DEV, seedGen());
    randomizeK0States(initRng);

    setupInputAndAonNoise(seedGen);

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
        randomDeviceSeedGenerator(),
        config
    ) {}

void K3::randomizeK0States(std::function<numeric()>& rng) noexcept {
    for (auto& pgUnit : periglomerularCells)
        pgUnit.randomizeK0States(rng);
    olfactoryBulb.randomizeK0States(rng);
    anteriorOlfactoryNucleus.randomizeK0States(rng);
    prepiriformCortex.randomizeK0States(rng);
    deepPyramidCells->randomizeState(rng);
}

void K3::setupInputAndAonNoise(std::function<rngseed()>& seedGen) noexcept {
    auto aonNoise = createGaussianRng(K3_AON_NOISE_STD_DEV, seedGen());
    anteriorOlfactoryNucleus.primaryNode()->setRngEngine(std::move(aonNoise));

    for (auto& pgUnit : periglomerularCells) {
        auto engine = createGaussianRng(K3_PG_NOISE_STD_DEV, seedGen());
        pgUnit.primaryNode()->setRngEngine(std::move(engine));
    }

    for (auto& obUnit : olfactoryBulb) {
        auto engine = createGaussianRng(K3_OB_NOISE_STD_DEV, seedGen());
        obUnit.primaryNode()->setRngEngine(std::move(engine));
    }
}


void K3::calculateNextState() noexcept {
    for (auto& pgUnit : periglomerularCells)
        pgUnit.calculateNextState();
    olfactoryBulb.calculateNextState();
    anteriorOlfactoryNucleus.calculateNextState();
    prepiriformCortex.calculateNextState();
    deepPyramidCells->calculateNextState();
}

void K3::commitNextState() noexcept {
    for (auto& pgUnit : periglomerularCells)
        pgUnit.commitNextState();
    olfactoryBulb.commitNextState();
    anteriorOlfactoryNucleus.commitNextState();
    prepiriformCortex.commitNextState();
    deepPyramidCells->commitNextState();
    advanceSystemNoise();
}

void K3::calculateAndCommitNextState() noexcept {
    calculateNextState();
    commitNextState();
}

void K3::advanceSystemNoise() noexcept {
    anteriorOlfactoryNucleus.primaryNode()->advanceNoise();
    for (auto& pgUnit : periglomerularCells)
        pgUnit.primaryNode()->advanceNoise();
    for (auto& obUnit : olfactoryBulb)
        obUnit.primaryNode()->advanceNoise();
}

void K3::eraseExternalStimulus() noexcept {
    auto pnIter = periglomerularCells.begin();
    auto obIter = olfactoryBulb.begin();
    while (pnIter != periglomerularCells.end()) {
        pnIter->setExternalStimulus(0);
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
    for (std::size_t i = 0; i < periglomerularCells.size(); i++) {
        std::stringstream unitName("Periglomerular cells (input layer) unit ");
        unitName << i;
        periglomerularCells[i].setName(unitName.str());
    }
    for (std::size_t i = 0; i < olfactoryBulb.size(); i++) {
        std::stringstream unitName("Olfactory bulb (K2 layer 1) unit ");
        unitName << i;
        olfactoryBulb.unit(i).setName(unitName.str());
    }
    anteriorOlfactoryNucleus.setName("Anterior olfactory nucleus (K2 layer 2)");
    prepiriformCortex.setName("Prepiriform cortex (K2 layer 3)");
}

void K3::connectAllSubcomponents(const K3Config& config) noexcept {
    connectPeriglomerularCellsLaterally(config.wPG_interUnit);
    olfactoryBulb.connectPrimaryNodesLaterally(config.wOB_inter[0]);
    olfactoryBulb.connectAntipodalNodesLaterally(config.wOB_inter[1]);
    connectLayers(config);
}

void K3::connectPeriglomerularCellsLaterally(numeric weight, std::size_t delay) noexcept {
    for (auto it1 = periglomerularCells.begin(); it1 != periglomerularCells.end(); it1++) {
        for (auto it2 = it1 + 1; it2 != periglomerularCells.end(); it2++) {
            it1->primaryNode()->addInboundConnection(it2->primaryNode(), weight, delay);
            it2->primaryNode()->addInboundConnection(it1->primaryNode(), weight, delay);
        }
    }
}

void K3::connectLayers(const K3Config& config) noexcept {
    auto pnIter = periglomerularCells.begin();
    auto obIter = olfactoryBulb.begin();
    while (pnIter != periglomerularCells.end()) {
        assert(obIter != olfactoryBulb.end());
        auto& pgUnit = *pnIter;
        auto& obUnit = *obIter;

        obUnit.primaryNode()->addInboundConnection(pgUnit.primaryNode(), config.wPG_OB, config.dPG_OB);

        // AON -> PON connections
        pgUnit.primaryNode()->addInboundConnection(
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

const std::shared_ptr<const K0> K3::getPrepiriformCortexPrimary() const noexcept {
    return prepiriformCortex.primaryNode();
}


const std::shared_ptr<const K0> K3::getDeepPyramidCells() const noexcept {
    return deepPyramidCells;
}
