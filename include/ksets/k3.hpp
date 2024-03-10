#pragma once

#include <array>
#include <cassert>

#include "ksets/k2.hpp"

namespace ksets {
    struct K3InterLayerWeights {
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
        std::size_t wDPC_OB_toAntipodal = 40;


        /// Scaling factor for noise injected into the primary K0 of the anterior olfactory nucleus (AON, layer 2 of
        /// K2 sets). The injected noise follows a gaussian distribution with mean 0 and standard deviation wnoise_AON.
        /// Must be positive.
        numeric wAON_noise = 0.10;

        K3InterLayerWeights() {
            // assert default weights are valid
            assert(checkWeightsValidity());
        }

        constexpr bool checkWeightsValidity() const {
            return pos(wPON_interUnit) && pos(wPON_OB) && pos(wOB_AON_lot) && pos(wOB_PC_lot)
                && pos(wAON_PON_mot) && pos(wAON_OB_toAntipodal) && neg(wPC_DPC) && pos(wDPC_PC)
                && pos(wDPC_OB_toAntipodal) && pos(wAON_noise);
        }
    private:
        constexpr bool pos(numeric value) const { return value > 0; }
        constexpr bool neg(numeric value) const { return value < 0; }
    };

    class K3 {
    public:
        K3(std::array<K2Weights, 3> layerIntraUnitWeights, K3InterLayerWeights interLayerWeights=K3InterLayerWeights());
    };
}