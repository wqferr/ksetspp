#pragma once

#include <array>

#include "ksets/k0.hpp"

namespace ksets {
    class K1: public K0Collection {
    public:
        K1(numeric wPrimarySecondary, numeric wSecondaryPrimary);
        K1(const K1& other);

        std::shared_ptr<K0> secondaryNode() { return node(1); }
        const std::shared_ptr<K0> secondaryNode() const { return node(1); }
    };
}