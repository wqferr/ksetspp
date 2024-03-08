#include "ksets/k2.hpp"

#include <stdexcept>

using ksets::K2, ksets::K2Weights, ksets::K0, ksets::numeric;

K2::K2(const K2Weights weights)
    : K0Collection(4)
{
    if (weights.wee < 0)
        throw std::invalid_argument("Weight from excitatory to excitatory units cannot be negative");
    if (weights.wei < 0)
        throw std::invalid_argument("Weight from excitatory to inhibitory units cannot be negative");
    if (weights.wie > 0)
        throw std::invalid_argument("Weight from inhibitory to excitatory units cannot be positive");
    if (weights.wii > 0)
        throw std::invalid_argument("Weight from inhibitory to inhibitory units cannot be positive");

    node(0)->addInboundConnection(node(1), weights.wee);
    node(0)->addInboundConnection(node(2), weights.wie);
    node(0)->addInboundConnection(node(3), weights.wie);

    node(1)->addInboundConnection(node(0), weights.wee);
    node(1)->addInboundConnection(node(3), weights.wie);

    node(2)->addInboundConnection(node(0), weights.wei);
    node(2)->addInboundConnection(node(3), weights.wii);

    node(3)->addInboundConnection(node(2), weights.wii);
}

K2::K2(const K2& other): K0Collection(other) {}
