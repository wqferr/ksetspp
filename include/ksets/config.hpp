#pragma once

#include <cstddef>
#include <cmath>
#include <array>

namespace ksets {
    using numeric = float;
    using rngseed = uint64_t;
    using conntag = int32_t;

    constexpr std::size_t DEFAULT_HISTORY_SIZE = 1'000;

    constexpr numeric ODE_STEP_SIZE = 0.5;
    constexpr numeric ODE_STEP_RECIPROC = 1 / ODE_STEP_SIZE;
    constexpr std::size_t odeMillisecondsToIters(numeric milliseconds) {
        return std::ceil(milliseconds * ODE_STEP_RECIPROC);
    }
    constexpr numeric odeItersToMilliseconds(std::size_t nIter) {
        return nIter * ODE_STEP_SIZE;
    }

    constexpr numeric ODE_A_DECAY_RATE = 0.22;
    constexpr numeric ODE_B_RISE_RATE = 0.72;

    constexpr numeric sigmoid(numeric x, numeric q) {
        return std::max(
            q * (1 - std::exp(-(std::exp(x)-1) / q)),
            static_cast<numeric>(-1.0)
        );
    }
}