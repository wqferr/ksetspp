#include "ksets/k1.hpp"

#include <cmath>
#include <stdexcept>

using ksets::K0, ksets::K1, ksets::numeric;

// FIXME: I never actually use K1... Should I remove it?
K1::K1(numeric wPrimarySecondary, numeric wSecondaryPrimary)
    : K0Collection(2)
{
    if (copysign(1.0, wPrimarySecondary) != copysign(1.0, wSecondaryPrimary))
        throw std::invalid_argument("Weights must both be positive or both be negative");

    secondaryNode()->addInboundConnection(primaryNode(), wPrimarySecondary);
    primaryNode()->addInboundConnection(secondaryNode(), wSecondaryPrimary);
}

K1::K1(const K1& other) noexcept: K0Collection(other) {}