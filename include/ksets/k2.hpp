#pragma once

#include <stdexcept>

#include "ksets/k0.hpp"

namespace ksets {
    struct K2Weights {
        numeric wee, wei, wie, wii;
        K2Weights(numeric wee, numeric wei, numeric wie, numeric wii)
            : wee(wee), wei(wei), wie(wie), wii(wii)
        {
            if (wee < 0)
                throw std::invalid_argument("Weight from excitatory to excitatory units cannot be negative");
            if (wei < 0)
                throw std::invalid_argument("Weight from excitatory to inhibitory units cannot be negative");
            if (wie > 0)
                throw std::invalid_argument("Weight from inhibitory to excitatory units cannot be positive");
            if (wii > 0)
                throw std::invalid_argument("Weight from inhibitory to inhibitory units cannot be positive");
        }
    };

    class K2: public K0Collection {
    public:
        K2(const K2Weights weights) noexcept;
        K2(const K2& other) noexcept;

        template<typename RNG>
        void perturbWeights(RNG& rng) noexcept {
            for (auto& node : *this) {
                for (
                    auto connection = node->iterInboundConnections();
                    connection != node->endInboundConnections();
                    connection++
                ) {
                    while (!connection->perturbWeight(rng())) {}
                }
            }
        }

        // Yes, I know this isn't a sphere, I just like the name
        std::shared_ptr<K0> antipodalNode() noexcept { return node(3); }
        const std::shared_ptr<K0> antipodalNode() const noexcept { return node(3); }
    };
}