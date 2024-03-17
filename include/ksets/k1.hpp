#pragma once

#include <array>

#include "ksets/k0.hpp"

namespace ksets {
    struct K1Config {
        numeric wPrimarySecondary;
        numeric wSecondaryPrimary;
        K0Config k0config = K0Config();

        K1Config(numeric wps, numeric wsp): wPrimarySecondary(wps), wSecondaryPrimary(wsp) {}
    };
    class K1: public K0Collection {
    public:
        // throws if signs of the weights are different
        K1(K1Config config);
        K1(const K1& other) noexcept;

        std::shared_ptr<K0> secondaryNode() noexcept { return node(1); }
        const std::shared_ptr<K0> secondaryNode() const noexcept { return node(1); }
    };
}