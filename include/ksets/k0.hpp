#pragma once

#include <vector>
#include <utility>
#include <array>
#include <memory>
#include <map>
#include <optional>

#include "ksets/config.hpp"
#include "ksets/activationhistory.hpp"

namespace ksets {
    class K0;
    class K0Collection;

    struct K0Connection {
        std::shared_ptr<K0> source;
        K0 *target;
        numeric weight;
        std::size_t delay;

        K0Connection(std::shared_ptr<K0> source, K0 *target, numeric weight, std::size_t delay)
            : source(source), target(target), weight(weight), delay(delay) {}

        bool perturbWeight(numeric delta);
    };

    class K0 {
        std::vector<K0Connection> inboundConnections;
        numeric currentExternalStimulus = 0;

        numeric calculateNetInput();
        void pushOutputToHistory();

        // [0] = current output (pre-sigmoid)
        // [1] = dout_dt
        using OdeState = std::array<numeric, 2>;

        OdeState odeState = {0, 0};
        OdeState nextOdeState = {0, 0};
        ActivationHistory activationHistory;

        std::optional<std::size_t> id = std::nullopt;
        std::optional<std::reference_wrapper<K0Collection>> collection = std::nullopt;

        void swap(K0& other);
    public:
        K0();
        K0(std::size_t id);
        K0(const K0& other);
        K0(K0&& other);
        K0& operator=(const K0& other);
        K0& operator=(K0&& other);

        std::optional<std::reference_wrapper<K0Collection>> getCollection() { return collection; }
        std::optional<std::size_t> getId() { return id; }

        std::map<const K0 *, std::shared_ptr<K0>> cloneSubgraph() const;

        void addInboundConnection(std::shared_ptr<K0> source, numeric weight, std::size_t delay=0);
        void clearInboundConnections();

        numeric getCurrentOutput() const;
        numeric getDelayedOutput(std::size_t delay) const;

        void setExternalStimulus(numeric newExternalStimulus);
        void calculateNextState();
        void calculateNextState(numeric newExternalStimulus);
        void commitNextState();
        void calculateAndCommitNextState();
        void calculateAndCommitNextState(numeric newExternalStimulus);

        const ActivationHistory& getActivationHistory() const;

        auto iterInboundConnections() {
            return inboundConnections.begin();
        }

        auto endInboundConnections() {
            return inboundConnections.end();
        }

        const auto iterInboundConnections() const {
            return inboundConnections.begin();
        }

        const auto endInboundConnections() const {
            return inboundConnections.end();
        }
    };

    class K0Collection {
        std::vector<std::shared_ptr<K0>> nodes;
        std::optional<std::string> name;

        void initNodes(std::size_t nNodes);
    public:
        K0Collection(std::size_t nNodes);
        K0Collection(std::size_t nNodes, std::string name);
        K0Collection(const K0Collection& other);
        ~K0Collection();

        std::optional<std::string>& getName() { return name; }

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