#pragma once

#include <array>

#include "ksets/k0.hpp"

namespace ksets {
    class K1: public K0Collection {
    public:
        // throws if signs of the weights are different
        K1(numeric wPrimarySecondary, numeric wSecondaryPrimary);
        K1(const K1& other) noexcept;

        std::shared_ptr<K0> secondaryNode() noexcept { return node(1); }
        const std::shared_ptr<K0> secondaryNode() const noexcept { return node(1); }
    };
}