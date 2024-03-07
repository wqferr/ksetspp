#pragma once

#include <stdexcept>

#include "ksets/k0collection.hpp"

namespace ksets {
    struct K2Weights {
        numeric wee, wei, wie, wii;
        K2Weights(numeric wee, numeric wei, numeric wie, numeric wii)
            : wee(wee), wei(wei), wie(wie), wii(wii) {}
    };

    class K2: public K0Collection {
    public:
        K2(const K2Weights weights);

        template<typename RNG>
        void perturbWeights(RNG& rng) {
            for (auto& node : *this) {
                for (
                    auto connection = node.iterInboundConnections();
                    connection != node.endInboundConnections();
                    connection++
                )
                    connection->perturbWeight(rng());
            }
        }

        // Yes, I know this isn't a sphere, I just like the name
        K0& antipodalNode() { return node(3); }
        const K0& antipodalNode() const { return node(3); }
    };
}