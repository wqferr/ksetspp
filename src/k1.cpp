#include "ksets/k1.hpp"

using ksets::K0, ksets::K1, ksets::numeric;

K1::K1(
    numeric wPrimarySecondary,
    numeric wSecondaryPrimary,
    std::size_t delayPrimarySecondary,
    std::size_t delaySecondaryPrimary
)
    : K0Collection(2)
{
    secondaryNode().addInboundConnection(primaryNode(), wPrimarySecondary, delayPrimarySecondary);
    primaryNode().addInboundConnection(secondaryNode(), wSecondaryPrimary, delaySecondaryPrimary);
}