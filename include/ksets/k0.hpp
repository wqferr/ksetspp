#pragma once

#include <vector>
#include <utility>
#include <array>
#include <memory>
#include <map>
#include <optional>
#include <functional>

#include "ksets/config.hpp"
#include "ksets/activationhistory.hpp"

/*
   This file technically abuses noexcept, because there are places where
   failed allocations *would* throw an exception.
   My philosophy is that if that happens, we're already F*#$d anyway,
   so might as well terminate the program.
*/

namespace ksets {
    struct K0Config {
        std::size_t historySize = 100;
    };

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

        std::optional<std::reference_wrapper<K0Collection>> collection = std::nullopt;
        std::optional<std::size_t> id = std::nullopt;

        void swap(K0& other) noexcept;
    public:
        explicit K0(K0Config config=K0Config()) noexcept;
        explicit K0(K0Collection& collection, std::size_t id, K0Config config=K0Config()) noexcept;

        K0(const K0& other) noexcept;
        K0(K0&& other) noexcept;
        K0& operator=(const K0& other) noexcept;
        K0& operator=(K0&& other) noexcept;

        std::optional<std::reference_wrapper<K0Collection>> getCollection() noexcept { return collection; }
        std::optional<std::size_t> getId() noexcept { return id; }

        void setHistorySize(std::size_t nIter);
        void setActivityMonitoring(std::size_t nIter);

        std::map<const K0 *, std::shared_ptr<K0>> cloneSubgraph() const noexcept;
        void cloneSubgraph(std::map<const K0 *, std::shared_ptr<K0>>& partialMapping) const noexcept;

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

        void randomizeState(std::function<numeric()>& rng) noexcept;

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

        void initNodes(std::size_t nNodes, const K0Config& config);
    public:
        // throws if nNodes == 0
        explicit K0Collection(std::size_t nNodes, std::optional<std::string> name=std::nullopt, K0Config config=K0Config());

        K0Collection(const K0Collection& other) noexcept;
        ~K0Collection();

        void setName(std::string name) { this->name = name; }
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

        void randomizeK0States(std::function<numeric()>& rng);
    };
}