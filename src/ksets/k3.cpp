#include "ksets/k3.hpp"

using ksets::K0, ksets::K2, ksets::K2Layer, ksets::K3, ksets::numeric;

explicit K3::K3(std::size_t olfactoryBulbNumUnits, ksets::K3Config config):
    primaryOlfactoryNerve(olfactoryBulbNumUnits),
    olfactoryBulb(olfactoryBulbNumUnits, config.wOB_intra),
    anteriorOlfactoryNucleus(config.wAON_intra),
    prepiriformCortex(config.wPC_intra),
    deepPyramidCells(new K0(olfactoryBulbNumUnits + 1))
{
    if (!config.checkWeightsValidity())
        throw std::invalid_argument("One or more K3 weights were invalid.");
    for (std::size_t i = 0; i < olfactoryBulbNumUnits; i++)
        primaryOlfactoryNerve[i] = std::make_shared<K0>();
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