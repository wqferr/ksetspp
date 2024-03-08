#pragma once

#include <vector>
#include <memory>
#include "ksets/k0.hpp"

namespace ksets {
    class K0Collection {
        // TODO: fix circular reference shared pointer loop
        std::vector<std::shared_ptr<K0>> nodes;
    public:
        K0Collection(std::size_t nNodes);
        K0Collection(const K0Collection& other);

        std::size_t size() const;

        std::shared_ptr<K0> primaryNode() { return node(0); }
        const std::shared_ptr<K0> primaryNode() const { return node(0); }
        std::shared_ptr<K0> node(std::size_t index);
        const std::shared_ptr<K0> node(std::size_t index) const;

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