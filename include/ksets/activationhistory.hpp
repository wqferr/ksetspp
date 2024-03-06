#pragma once

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
    };
}