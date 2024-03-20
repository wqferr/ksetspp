#pragma once

#include <vector>
#include <deque>
#include <optional>
#include <stdexcept>

#include "config.hpp"

namespace ksets {
    class ActivationHistory {
        struct MonitoringStats {
            std::size_t windowSize;
            numeric sum;
            numeric varianceNumerator;
            MonitoringStats(std::size_t windowSize): windowSize(windowSize), sum(0), varianceNumerator(0) {};
        };
        std::deque<ksets::numeric> history;
        std::optional<MonitoringStats> monitoredWindow;
        std::size_t numPuts = 0;

        // returns variance numerator and sum of window, in that order
        std::pair<numeric, numeric> varianceNumeratorAndSum(std::size_t window) const;
        void initMonitoring();
        void doMonitoring(numeric newestValue);
    public:

        ActivationHistory(std::size_t historySize=DEFAULT_HISTORY_SIZE);

        // pass 0 to disable
        void setActivityMonitoring(std::size_t windowSize);

        std::size_t getNumPutsMade() const noexcept;
        void put(numeric rawValue);
        numeric get(std::size_t offset=0) const;
        std::size_t size() const noexcept;
        void resize(std::size_t newSize);

        struct Slice {
            const std::size_t creationNumPuts;
            std::size_t offsetStart;
            std::size_t length;
            const ActivationHistory& fullHistory;

            Slice(const ActivationHistory& history, std::size_t numPuts, std::size_t start, std::size_t length);
            bool isValid() const noexcept;
            numeric get(std::size_t offset=0) const;
        private:
            std::size_t transformIndex(std::size_t sliceIndex) const noexcept;
        };
        const Slice slice(std::size_t offsetStart, std::size_t length);

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
