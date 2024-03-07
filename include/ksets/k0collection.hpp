#pragma once

#include "ksets/k0.hpp"
#include <vector>

namespace ksets {
    class K0Collection {
        std::vector<K0> nodes;
    public:
        K0Collection(std::size_t nNodes): nodes(nNodes, K0()) {}
        K0& primaryNode() { return node(0); }
        const K0& primaryNode() const { return node(0); }
        K0& node(std::size_t index);
        const K0& node(std::size_t index) const;

        auto begin() {
            return nodes.begin();
        }

        auto end() {
            return nodes.end();
        }

        const auto begin() const {
            return nodes.begin();
        }

        const auto end() const {
            return nodes.end();
        }

        void setExternalStimulus(numeric newExternalStimulus);
        void calculateNextState();
        void calculateNextState(numeric newExternalStimulus);
        void commitNextState();
        void calculateAndCommitNextState();
        void calculateAndCommitNextState(numeric newExternalStimulus);
    };
}