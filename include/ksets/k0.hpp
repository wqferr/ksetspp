#pragma once

#include <vector>
#include <utility>
#include <array>
#include <memory>
#include <map>

#include "ksets/config.hpp"
#include "ksets/activationhistory.hpp"

namespace ksets {
    class K0;

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

    public:
        K0();
        K0(const K0& other);
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
}