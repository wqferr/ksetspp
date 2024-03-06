#pragma once

#include <array>

#include "ksets/k0.hpp"
#include "ksets/k0collection.hpp"

namespace ksets {
    class K1: public K0Collection {
        std::array<K0, 2> nodes;
    public:
        K1(
            numeric wPrimarySecondary,
            numeric wSecondaryPrimary,
            std::size_t delayPrimarySecondary=0,
            std::size_t delaySecondaryPrimary=0
        );

        K0& secondaryNode() { return node(1); }
        const K0& secondaryNode() const { return node(1); }
    };
}