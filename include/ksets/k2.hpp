#pragma once

#include <stdexcept>

#include "ksets/k0.hpp"

namespace ksets {
    struct K2Config {
        numeric wee, wei, wie, wii;
        K0Config k0config;

        K2Config(numeric wee, numeric wei, numeric wie, numeric wii):
            K2Config(std::array<numeric, 4>{wee, wei, wie, wii}) {}

        K2Config(std::array<numeric, 4> weights, K0Config k0config=K0Config()):
            wee(weights[0]), wei(weights[1]), wie(weights[2]), wii(weights[3]), k0config(k0config)
        {
            checkWeights();
        }

        bool checkWeights() const {
            return wee > 0 && wei > 0 && wie < 0 && wii < 0;
        }
    };

    class K2: public K0Collection {
    public:
        explicit K2(const K2Config config, std::optional<std::string> name=std::nullopt) noexcept;
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