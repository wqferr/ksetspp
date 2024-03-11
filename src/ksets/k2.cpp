#include "ksets/k2.hpp"

#include <stdexcept>

using ksets::K2, ksets::K2Config, ksets::K0, ksets::numeric;

K2::K2(const K2Config config, std::optional<std::string> name) noexcept:
    K0Collection(4, name, config.k0config)
{
    node(0)->addInboundConnection(node(1), config.wee);
    node(0)->addInboundConnection(node(2), config.wie);
    node(0)->addInboundConnection(node(3), config.wie);

    node(1)->addInboundConnection(node(0), config.wee);
    node(1)->addInboundConnection(node(3), config.wie);

    node(2)->addInboundConnection(node(0), config.wei);
    node(2)->addInboundConnection(node(3), config.wii);

    node(3)->addInboundConnection(node(0), config.wei);
    node(3)->addInboundConnection(node(1), config.wei);
    node(3)->addInboundConnection(node(2), config.wii);
}

K2::K2(const K2& other) noexcept: K0Collection(other) {}
