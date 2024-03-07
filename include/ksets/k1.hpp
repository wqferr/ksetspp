#pragma once

#include <array>

#include "ksets/k0.hpp"
#include "ksets/k0collection.hpp"

namespace ksets {
    class K1: public K0Collection {
    public:
        K1(numeric wPrimarySecondary, numeric wSecondaryPrimary);

        K0 *secondaryNode() { return node(1); }
        const K0 *secondaryNode() const { return node(1); }
    };
}