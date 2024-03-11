#pragma once

#include <vector>
#include <deque>
#include <optional>

#include "config.hpp"

namespace ksets {
    class ActivationHistory {
        struct MonitoringStats {
            std::size_t windowSize;
            numeric sum;
            numeric varianceNumerator;
            MonitoringStats(std::size_t windowSize): windowSize(windowSize), sum(0), varianceNumerator(0) {};
        };
        std::deque<numeric> history;
        std::optional<MonitoringStats> monitoredWindow;

        // returns variance numerator and sum of window, in that order
        std::pair<numeric, numeric> varianceNumeratorAndSum(std::size_t window) const;
        void initMonitoring();
        void doMonitoring(numeric newestValue);
    public:
        ActivationHistory(std::size_t historySize=DEFAULT_HISTORY_SIZE);

        // pass 0 to disable
        void setActivityMonitoring(std::size_t windowSize);

        void put(numeric value);
        numeric get(std::size_t offset) const;
        void resize(std::size_t newSize);

        numeric variance() const;
        numeric variance(std::size_t window) const;

        numeric stddev() const;
        numeric stddev(std::size_t window) const;

        // this existed solely to debug the monitoring system
        // numeric varianceSlow(std::size_t window) const;

        auto begin() const {
            return history.begin();
        }

        auto end() const {
            return history.end();
        }

        auto tail(std::size_t n) const {
            if (n > history.size())
                throw std::invalid_argument("Tail length must be less than or equal to history size.");
            return end() - n;
        }

        template<typename RNG>
        void fillWithNoise(std::size_t nSamples, RNG& rng) {
            for (std::size_t i = 0; i < nSamples; i++)
                put(rng());
        }
    };
}