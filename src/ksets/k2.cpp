#include "ksets/k2.hpp"

#include <stdexcept>

using ksets::K2, ksets::K2Weights, ksets::K0, ksets::numeric;

K2::K2(const K2Weights weights) noexcept
    : K0Collection(4)
{
    node(0)->addInboundConnection(node(1), weights.wee);
    node(0)->addInboundConnection(node(2), weights.wie);
    node(0)->addInboundConnection(node(3), weights.wie);

    node(1)->addInboundConnection(node(0), weights.wee);
    node(1)->addInboundConnection(node(3), weights.wie);

    node(2)->addInboundConnection(node(0), weights.wei);
    node(2)->addInboundConnection(node(3), weights.wii);

    node(3)->addInboundConnection(node(0), weights.wei);
    node(3)->addInboundConnection(node(1), weights.wei);
    node(3)->addInboundConnection(node(2), weights.wii);
}

K2::K2(const K2& other) noexcept: K0Collection(other) {}
