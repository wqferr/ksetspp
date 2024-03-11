#pragma once

#include <stdexcept>

#include "ksets/k0.hpp"

namespace ksets {
    struct K2Weights {
        numeric wee, wei, wie, wii;
        K2Weights(numeric wee, numeric wei, numeric wie, numeric wii)
            : wee(wee), wei(wei), wie(wie), wii(wii)
        {
            checkWeights();
        }

        K2Weights(std::array<numeric, 4> values): K2Weights(values[0], values[1], values[2], values[3]) {}

        bool checkWeights() const {
            return wee > 0 && wei > 0 && wie < 0 && wii < 0;
        }
    };

    class K2: public K0Collection {
    public:
        K2(const K2Weights weights) noexcept;
        K2(const K2& other) noexcept;

        // TODO: if this gets readded to the spec, make it receive a std::function<numeric()> instead of
        // being a member template function
        // template<typename RNG>
        // void perturbWeights(RNG& rng) noexcept {
        //     for (auto& node : *this) {
        //         for (
        //             auto connection = node->iterInboundConnections();
        //             connection != node->endInboundConnections();
        //             connection++
        //         ) {
        //             while (!connection->perturbWeight(rng())) {}
        //         }
        //     }
        // }

        // Yes, I know this isn't a sphere, I just like the name
        std::shared_ptr<K0> antipodalNode() noexcept { return node(3); }
        const std::shared_ptr<K0> antipodalNode() const noexcept { return node(3); }
    };
}