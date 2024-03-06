#pragma once

#include <vector>
#include <utility>
#include <array>

#include "ksets/config.hpp"
#include "ksets/activationhistory.hpp"

namespace ksets {
    class K0;

    class K0 {
        struct K0Connection {
            const K0& source;
            numeric weight;
            std::size_t delay;

            K0Connection(const K0& source, numeric weight, std::size_t delay): source(source), weight(weight), delay(delay) {}
        };

        std::vector<K0Connection> inboundConnections;
        numeric currentExternalStimulus;
        numeric learningRate;

        numeric calculateNetInput();
        void pushOutputToHistory();

        // [0] = current output (pre-sigmoid)
        // [1] = dout_dt
        using OdeState = std::array<numeric, 2>;

        OdeState odeState = {0, 0};
        OdeState nextOdeState = {0, 0};
        ActivationHistory activationHistory;

    public:
        K0(numeric learningRate=DEFAULT_LEARN_RATE);

        void addInboundConnection(const K0& source, numeric weight, std::size_t delay);

        numeric getCurrentOutput() const;
        // throws exception if delay is greater than history size
        numeric getDelayedOutput(std::size_t delay) const;

        void hebbianReinforcementIteration();
        void setExternalStimulus(numeric newStimulus);
        void calculateNextState();
        void calculateNextState(numeric newExternalStimulus);
        void commitNextState();
        void calculateAndCommitNextState();
        void calculateAndCommitNextState(numeric newExternalStimulus);

        const ActivationHistory& getActivationHistory() const;
    };
}