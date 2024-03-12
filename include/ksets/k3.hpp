#pragma once

#include <array>
#include <vector>
#include <cassert>
#include <memory>
#include <functional>

#include "ksets/k0.hpp"
#include "ksets/k2.hpp"
#include "ksets/k2layer.hpp"

namespace ksets {
    struct K3Config {
        /// Weight between primary olfactory nerve (PON, the input K0 array) units.
        /// This will be divided by the number of units in the primary input nerve.
        /// Must be positive.
        numeric wPON_interUnit = 0.10;

        /// Delay for wPON_interUnit connection. See wPON_interUnit for more information.
        std::size_t dPON_interUnit = 1;

        /// Weight between the primary olfactory nerve (PON, the input K0 array) and the
        /// excitatory K0 in the olfactory bulb (OB, layer 1 of K2 sets). Must be positive.
        numeric wPON_OB = 1.00;

        /// Delay for wPON_OB connection. See wPON_OB for more information.
        std::size_t dPON_OB = 1;

        /// Weight of the lateral olfactory tract (LOT) between the olfactory bulb (OB, layer 1 of K2 sets)
        /// and the anterior olfactory nucleus (AON, layer 2). It connects all primary K0 in OB to the single
        /// primary K0 in AON. Must be positive.
        numeric wOB_AON_lot = 0.15;

        /// Delay for wOB_AON_lot connection. See wOB_AON_lot for more information.
        std::size_t dOB_AON_lot = 1;

        /// Weight of the lateral olfactory tract (LOT) between the olfactory bulb (OB, layer 1 of K2 sets)
        /// and the prepiriform cortex (PC, layer 3 of K2 sets). It connects all primary K0 in OB to the single
        /// primary K0 in PC. Must be positive.
        numeric wOB_PC_lot = 0.60;

        /// Delay for wOB_PC_lot. See wOB_PC_lot for more information.
        std::size_t dOB_PC_lot = 1;

        /// Weight of the medium olfactory tract (MOT) between the anterior olfactory nucleus (AON, layer 2 of K2 sets)
        /// and the primary olfactory nerve (PON, the input K0 array). It connects the single primary K0 in AON to all
        /// K0 in PON. Must be positive.
        numeric wAON_PON_mot = 0.05;

        /// Delay for wAON_PON_mot. See wAON_PON_mot for more information.
        std::size_t dAON_PON_mot = 17;

        /// Weight of the medium olfactory tract (MOT) between the anterior olfactory nucleus (AON, layer 2 of K2 sets)
        /// and the olfactory bulb (OB, layer 1 of K2 sets). It connects the single primary K0 in AON to all antipodal
        /// K0 in OB. Must be positive.
        numeric wAON_OB_toAntipodal = 0.25;

        /// Delay for wAON_OB_toAntipodal. See wAON_OB_toAntipodal for more information.
        std::size_t dAON_OB_toAntipodal = 25;

        /// Weight between the prepiriform cortex (PC, layer 3 of K2 sets) and the anterior olfactory nucleus (AON,
        /// layer 2 of K2 sets). It connects the primary node of PC to the antipodal node of AON. Must be positive.
        numeric wPC_AON_toAntipodal = 0.20;

        /// Delay for wPC_AON_toAntipodal. See wPC_AON_toAntipodal for more information.
        std::size_t dPC_AON_toAntipodal = 25;

        /// Weight between the prepiriform cortex (PC, layer 3 of K2 sets) and the deep pyramid cells (DPC,
        /// <unknown function>). Must be negative.
        numeric wPC_DPC = -0.05;

        /// Delay for wPC_DPC. See wPC_DPC for more information.
        std::size_t dPC_DPC = 1;

        /// Weight between the deep pyramid cells (DPC, <unknown function>) back to the prepiriform cortex
        /// (PC, layer 3 of K2 sets). Must be positive. THIS DEFAULT VALUE IS A GUESS! I DON'T KNOW THE ACTUAL VALUES
        /// OF THIS WEIGHT OR DELAY WITHOUT ACCESS TO THE ARTICLE.
        numeric wDPC_PC = 0.20;

        /// Delay for wDPC_PC. See wDPC_PC for more information.
        std::size_t dDPC_PC = 1;

        /// Weight between the deep pyramid cells (DPC, <unknown function>) and the olfactory bulb (OB, layer 1
        /// of K2 sets). It connects the single K0 set of DPC to all antipodal K0 sets in OB. Must be positive.
        /// THIS DEFAULT VALUE IS A GUESS! I DON'T KNOW THE ACTUAL VALUES OF THIS WEIGHT OR DELAY WITHOUT
        /// ACCESS TO THE ARTICLE.
        numeric wDPC_OB_toAntipodal = 0.50;

        /// Delay for wDPC_OB_toAntipodal. See wDPC_OB_toAntipodal for more information.
        std::size_t dDPC_OB_toAntipodal = 40;


        /// Scaling factor for noise injected into the primary K0 of the anterior olfactory nucleus (AON, layer 2 of
        /// K2 sets). The injected noise follows a gaussian distribution with mean 0 and standard deviation wnoise_AON.
        /// Must be positive.
        numeric wAON_noise = 0.10;

        /// Intra unit weights for each of the K2 sets in the olfactory bulb (OB, layer 1 of K2 sets).
        /// Also controls history size for
        /// See K2Config for more information.
        K2Config OB_unitConfig = {1.8, 1.0, -2.0, -0.8};

        /// Inter unit weights between each pair of K2 sets in the olfactory bulb (OB, layer 1 of K2 sets).
        /// See K2Layer for more information.
        std::array<numeric, 2> wOB_inter = {0.15, -0.10};

        /// Intra unit weights for the single K2 set in the anterior olfactory nucleus (AON, layer 2 of K2 sets).
        /// See K2Config for more information.
        K2Config AON_unitConfig = {1.6, 1.6, -1.5, -2.0};

        /// Intra unit weights for the single K2 set in the prepiriform cortex (PC, layer 3 of K2 sets).
        /// See K2Config for more information.
        K2Config PC_unitConfig = {1.6, 1.9, -0.2, -1.0};

        /// Length of history tracking for output nodes (primary and antipodal nodes of the olfactory bulb,
        /// layer 1 of K2 sets).
        std::size_t outputHistorySize = 5'000;

        /// Number of latest iterations for which output nodes (primary and antipodal nodes of the olfactory bulb,
        /// layer 1 of K2 sets) variance and standard deviation will be tracked.
        std::size_t outputNodeActivityMonitoring = 1'000;

        /// Length of history tracking for non-output nodes. See outputHistorySize for more information.
        std::size_t nonOutputHistorySize = 100;

        K3Config() {
            // assert default weights are valid
            assert(checkWeightsValidity());
        }

        bool checkWeightsValidity() const {
            return pos(wPON_interUnit) && pos(wPON_OB) && pos(wOB_AON_lot) && pos(wOB_PC_lot)
                && pos(wAON_PON_mot) && pos(wAON_OB_toAntipodal) && pos(wPC_AON_toAntipodal)
                && neg(wPC_DPC) && pos(wDPC_PC) && pos(wDPC_OB_toAntipodal) && pos(wAON_noise)
                && OB_unitConfig.checkWeights() && AON_unitConfig.checkWeights() && PC_unitConfig.checkWeights();
        }
    private:
        bool pos(numeric value) const { return value > 0; }
        bool neg(numeric value) const { return value < 0; }
    };

    class K3 {
        K0Collection primaryOlfactoryNerve;
        K2Layer olfactoryBulb;
        K2 anteriorOlfactoryNucleus;
        K2 prepiriformCortex;
        std::shared_ptr<K0> deepPyramidCells;

        std::function<numeric()> aonStimulusRng;

        // these 2 are redundant, just cached for easy retrieval of model output
        std::vector<std::shared_ptr<const K0>> obPrimaryNodes;
        std::vector<std::shared_ptr<const K0>> obAntipodalNodes;

        void connectPrimaryOlfactoryNerveLaterally(numeric weight, std::size_t delay=0) noexcept;
        void connectLayers(const K3Config& config) noexcept;

        void nameAllSubcomponents() noexcept;
        void connectAllSubcomponents(const K3Config& config) noexcept;

        void randomizeK0States(std::function<numeric()>& rng) noexcept;

        void calculateNextState() noexcept;
        void commitNextState() noexcept;
        void calculateAndCommitNextState() noexcept;

        void eraseExternalStimulus() noexcept;

        void cachePrimaryAndAntipodalOlfactoryBulbNodes() noexcept;

        template<typename Iterator>
        void setPattern(Iterator patternFirst, Iterator patternEnd) {
            if (patternEnd - patternFirst != primaryOlfactoryNerve.size())
                throw std::invalid_argument("Pattern length does not match input layer size");
            auto pnIter = primaryOlfactoryNerve.begin();
            auto obIter = olfactoryBulb.begin();
            auto patternIter = patternFirst;
            while (patternIter != patternEnd) {
                assert(pnIter != primaryOlfactoryNerve.end());
                assert(obIter != olfactoryBulb.end());
                (*pnIter)->setExternalStimulus(*patternIter);
                obIter->setExternalStimulus(*patternIter);
                patternIter++;
                pnIter++;
                obIter++;
            }
        }

        void advanceAonNoise() noexcept;

        void run(numeric milliseconds) noexcept;

    public:
        explicit K3(std::size_t olfactoryBulbNumUnits, numeric initialRestMilliseconds, K3Config config=K3Config());
        explicit K3(std::size_t olfactoryBulbNumUnits, numeric initialRestMilliseconds, std::function<numeric()> rng, K3Config config=K3Config());

        void rest(numeric milliseconds) noexcept;

        template<typename Iterator>
        void present(numeric milliseconds, Iterator patternFirst, Iterator patternLast) {
            setPattern(patternFirst, patternLast);
            run(milliseconds);
        }

        const std::vector<std::shared_ptr<const K0>>& getOlfactoryBulbPrimaryNodes() const noexcept;
        const std::vector<std::shared_ptr<const K0>>& getOlfactoryBulbAntipodalNodes() const noexcept;
        const std::shared_ptr<const K0> getDeepPyramidCells() const noexcept;
    };
}