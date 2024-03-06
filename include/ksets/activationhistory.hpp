#pragma once

#include <vector>
#include <deque>
#include <optional>

#include "ksets/config.hpp"

namespace ksets {
    class ActivationHistory {
        std::deque<numeric> history;

    public:
        ActivationHistory(std::size_t historySize=HISTORY_SIZE);

        void put(numeric value);
        numeric get(std::size_t offset) const;
        std::vector<numeric> tail(std::size_t n) const;

        auto begin() const {
            return history.begin();
        }

        auto end() const {
            return history.end();
        }
    };
}