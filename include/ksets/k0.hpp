#pragma once

#include <vector>
#include <utility>
#include <array>
#include <memory>
#include <map>
#include <optional>

#include "ksets/config.hpp"
#include "ksets/activationhistory.hpp"

/*
   This file technically abuses noexcept, because there are places where
   failed allocations *would* throw an exception.
   My philosophy is that if that happens, we're already F*#$d anyway,
   so might as well terminate the program.
*/

namespace ksets {
    class K0;
    class K0Collection;

    struct K0Connection {
        std::shared_ptr<K0> source;
        K0 *target;
        numeric weight;
        std::size_t delay;

        K0Connection(std::shared_ptr<K0> source, K0 *target, numeric weight, std::size_t delay) noexcept
            : source(source), target(target), weight(weight), delay(delay) {}

        bool perturbWeight(numeric delta) noexcept;
    };

    class K0 {
        std::vector<K0Connection> inboundConnections;
        numeric currentExternalStimulus = 0;

        numeric calculateNetInput() noexcept;
        void pushOutputToHistory() noexcept;

        // [0] = current output (pre-sigmoid)
        // [1] = dout_dt
        using OdeState = std::array<numeric, 2>;

        OdeState odeState = {0, 0};
        OdeState nextOdeState = {0, 0};
        ActivationHistory activationHistory;

        std::optional<std::size_t> id = std::nullopt;
        std::optional<std::reference_wrapper<K0Collection>> collection = std::nullopt;

        void swap(K0& other) noexcept;
    public:
        K0() noexcept;
        K0(K0Collection& collection, std::size_t id) noexcept;
        K0(std::size_t id) noexcept;
        K0(const K0& other) noexcept;
        K0(K0&& other) noexcept;
        K0& operator=(const K0& other) noexcept;
        K0& operator=(K0&& other) noexcept;

        std::optional<std::reference_wrapper<K0Collection>> getCollection() noexcept { return collection; }
        std::optional<std::size_t> getId() noexcept { return id; }

        std::map<const K0 *, std::shared_ptr<K0>> cloneSubgraph() const noexcept;

        void addInboundConnection(std::shared_ptr<K0> source, numeric weight, std::size_t delay=0) noexcept;
        void clearInboundConnections() noexcept;

        numeric getCurrentOutput() const noexcept;
        numeric getDelayedOutput(std::size_t delay) const noexcept;

        void setExternalStimulus(numeric newExternalStimulus) noexcept;
        void calculateNextState() noexcept;
        void calculateNextState(numeric newExternalStimulus) noexcept;
        void commitNextState() noexcept;
        void calculateAndCommitNextState() noexcept;
        void calculateAndCommitNextState(numeric newExternalStimulus) noexcept;

        const ActivationHistory& getActivationHistory() const noexcept;

        auto iterInboundConnections() noexcept {
            return inboundConnections.begin();
        }

        auto endInboundConnections() noexcept {
            return inboundConnections.end();
        }

        const auto iterInboundConnections() const noexcept {
            return inboundConnections.begin();
        }

        const auto endInboundConnections() const noexcept {
            return inboundConnections.end();
        }
    };

    class K0Collection {
        std::vector<std::shared_ptr<K0>> nodes;
        std::optional<std::string> name;

        void initNodes(std::size_t nNodes);
    public:
        // throws if nNodes == 0
        K0Collection(std::size_t nNodes);

        // throws if nNodes == 0
        K0Collection(std::size_t nNodes, std::string name);

        K0Collection(const K0Collection& other) noexcept;
        ~K0Collection();

        std::optional<std::string>& getName() noexcept { return name; }

        std::size_t size() const noexcept;

        std::shared_ptr<K0> primaryNode() noexcept { return node(0); }
        const std::shared_ptr<K0> primaryNode() const noexcept { return node(0); }

        // throws if index >= size()
        std::shared_ptr<K0> node(std::size_t index);

        // throws if index >= size()
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

        void setExternalStimulus(numeric newExternalStimulus) noexcept;
        void calculateNextState() noexcept;
        void calculateNextState(numeric newExternalStimulus) noexcept;
        void commitNextState() noexcept;
        void calculateAndCommitNextState() noexcept;
        void calculateAndCommitNextState(numeric newExternalStimulus) noexcept;
    };
}