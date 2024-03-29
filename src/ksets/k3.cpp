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

    std::function<rngseed()> randomDeviceSeedGenerator(std::size_t batchSize) {
        std::deque<rngseed> batch(batchSize, 0);
        std::size_t i = batch.size() - 1;
        std::random_device rd {};
        std::array<rngseed, 4> s = {rd(), rd(), rd(), rd()};
        return [batch = std::move(batch), s = std::move(s), i]() mutable {
            static std::seed_seq seedGen(s.begin(), s.end());

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
        k1config.k0config = {k3config.nonOutputHistorySize, 1.824};
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
    deepPyramidCells(1, "Deep pyramid cells", dpcConfig(config))
{
    if (!config.checkWeightsValidity())
        throw std::invalid_argument("One or more K3 weights were invalid.");
    nameAndSetCollectionForAllSubcomponents();
    connectAllSubcomponents(config);
    perturbObPrimaryLateralWeights(olfactoryBulbNumUnits, config, seedGen);

    randomizeK0States(config, seedGen);
    setupInputAndAonNoise(config, seedGen);

    olfactoryBulb.setPrimaryHistorySize(config.outputHistorySize);
    olfactoryBulb.setPrimaryActivityMonitoring(config.outputActivityMonitoring);
    olfactoryBulb.setAntipodalHistorySize(config.outputHistorySize);
    olfactoryBulb.setAntipodalActivityMonitoring(config.outputActivityMonitoring);
    anteriorOlfactoryNucleus.primaryNode()->setHistorySize(config.outputHistorySize);
    anteriorOlfactoryNucleus.primaryNode()->setActivityMonitoring(config.outputActivityMonitoring);
    prepiriformCortex.primaryNode()->setHistorySize(config.outputHistorySize);
    prepiriformCortex.primaryNode()->setActivityMonitoring(config.outputActivityMonitoring);

    rest(initialRestMilliseconds);
}

K3::K3(std::size_t olfactoryBulbNumUnits, numeric initialRestMilliseconds, ksets::K3Config config)
    : K3(
        olfactoryBulbNumUnits,
        initialRestMilliseconds,
        randomDeviceSeedGenerator(config.rngSeedGenBatchSize),
        config
    ) {}

void K3::randomizeK0States(const K3Config& config, std::function<ksets::rngseed()>& seedGen) noexcept {
    auto rng = createGaussianRng(config.noiseInitialK0States, seedGen());
    for (auto& pgUnit : periglomerularCells)
        pgUnit.randomizeK0States(rng);
    olfactoryBulb.randomizeK0States(rng);
    anteriorOlfactoryNucleus.randomizeK0States(rng);
    prepiriformCortex.randomizeK0States(rng);
    deepPyramidCells.randomizeK0States(rng);
}

void K3::setupInputAndAonNoise(const K3Config& config, std::function<rngseed()>& seedGen) noexcept {
    auto aonNoise = createGaussianRng(config.noiseAON, seedGen());
    anteriorOlfactoryNucleus.primaryNode()->setRngEngine(std::move(aonNoise));

    for (auto& pgUnit : periglomerularCells) {
        auto engine = createGaussianRng(config.noisePG, seedGen());
        pgUnit.primaryNode()->setRngEngine(std::move(engine));
    }

    for (auto& obUnit : olfactoryBulb) {
        auto engine = createGaussianRng(config.noiseOB, seedGen());
        obUnit.primaryNode()->setRngEngine(std::move(engine));
    }
}

void K3::perturbObPrimaryLateralWeights(std::size_t numObUnits, const K3Config& config, std::function<rngseed()>& seedGen) noexcept {
    auto weight = config.noiseObLateralWeights;
    if (numObUnits > 1) weight /= numObUnits - 1;
    auto rng = createGaussianRng(weight, seedGen());
    for (auto& unit : olfactoryBulb) {
        for (auto& connection : *unit.primaryNode()) {
            if (connection.tag.has_value() && connection.tag.value() == TAG_OB_PRIMARY_LATERAL)
                connection.perturbWeight(rng());
        }
    }
}


void K3::calculateNextState() noexcept {
    for (auto& pgUnit : periglomerularCells)
        pgUnit.calculateNextState();
    olfactoryBulb.calculateNextState();
    anteriorOlfactoryNucleus.calculateNextState();
    prepiriformCortex.calculateNextState();
    deepPyramidCells.calculateNextState();
}

void K3::commitNextState() noexcept {
    for (auto& pgUnit : periglomerularCells)
        pgUnit.commitNextState();
    olfactoryBulb.commitNextState();
    anteriorOlfactoryNucleus.commitNextState();
    prepiriformCortex.commitNextState();
    deepPyramidCells.commitNextState();
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

void K3::nameAndSetCollectionForAllSubcomponents() noexcept {
    for (std::size_t i = 0; i < periglomerularCells.size(); i++) {
        std::stringstream unitName;
        unitName << "Periglomerular cells (input layer) unit ";
        unitName << i;
        periglomerularCells[i].setName(unitName.str());
        periglomerularCells[i].updateNodeCollectionReferenceAndId();
    }
    for (std::size_t i = 0; i < olfactoryBulb.size(); i++) {
        std::stringstream unitName;
        unitName << "Olfactory bulb (K2 layer 1) unit ";
        unitName << i;
        olfactoryBulb.unit(i).setName(unitName.str());
        olfactoryBulb.unit(i).updateNodeCollectionReferenceAndId();
    }
    anteriorOlfactoryNucleus.setName("Anterior olfactory nucleus (K2 layer 2)");
    anteriorOlfactoryNucleus.updateNodeCollectionReferenceAndId();
    prepiriformCortex.setName("Prepiriform cortex (K2 layer 3)");
    prepiriformCortex.updateNodeCollectionReferenceAndId();
}

void K3::connectAllSubcomponents(const K3Config& config) noexcept {
    connectPeriglomerularCellsLaterally(config.wPG_interUnit);
    olfactoryBulb.connectPrimaryNodesLaterally(config.wOB_inter[0], 0, TAG_OB_PRIMARY_LATERAL);
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
            deepPyramidCells.primaryNode(), config.wDPC_OB_toAntipodal, config.dDPC_OB_toAntipodal);

        pnIter++;
        obIter++;
    }

    // Singled out connections, not part of any major chain
    anteriorOlfactoryNucleus.antipodalNode()->addInboundConnection(
        prepiriformCortex.primaryNode(),
        config.wPC_AON_toAntipodal,
        config.dPC_AON_toAntipodal);

    // Prepiriform cortex -> deep pyramid cells
    deepPyramidCells.primaryNode()->addInboundConnection(
        prepiriformCortex.antipodalNode(),
        config.wPC_DPC,
        config.dPC_DPC);

    // Deep pyramid cells -> prepiriform cortex
    prepiriformCortex.antipodalNode()->addInboundConnection(
        deepPyramidCells.primaryNode(),
        config.wDPC_PC,
        config.dDPC_PC);
}

const K2Layer& K3::getOlfactoryBulb() const noexcept {
    return olfactoryBulb;
}

const K2& K3::getAnteriorOlfactoryNucleus() const noexcept {
    return anteriorOlfactoryNucleus;
}

const std::shared_ptr<const K0> K3::getPrepiriformCortexPrimary() const noexcept {
    return prepiriformCortex.primaryNode();
}


const std::shared_ptr<const K0> K3::getDeepPyramidCells() const noexcept {
    return deepPyramidCells.primaryNode();
}
