#include "ksets/k1.hpp"

#include <cmath>
#include <stdexcept>

using ksets::K0, ksets::K1, ksets::numeric;

K1::K1(K1Config config):
    K0Collection(2, std::nullopt, config.k0config)
{
    if (copysign(1.0, config.wPrimarySecondary) != copysign(1.0, config.wSecondaryPrimary))
        throw std::invalid_argument("Weights must both be positive or both be negative");

    secondaryNode()->addInboundConnection(primaryNode(), config.wPrimarySecondary);
    primaryNode()->addInboundConnection(secondaryNode(), config.wSecondaryPrimary);
}

K1::K1(const K1& other) noexcept: K0Collection(other) {}