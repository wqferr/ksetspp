#pragma once

#include <cstddef>
#include <cmath>
#include <array>

namespace ksets {
    typedef float numeric;
    constexpr std::size_t HISTORY_SIZE = 20'000;
    constexpr numeric DEFAULT_LEARN_RATE = 0.05;

    constexpr numeric ODE_STEP_SIZE = 0.5;
    constexpr std::size_t ODE_ITERS_PER_MILLISECOND(numeric milliseconds) {
        return std::ceil(milliseconds / ODE_STEP_SIZE);
    }

    constexpr numeric ODE_A_DECAY_RATE = 0.22;
    constexpr numeric ODE_B_RISE_RATE = 0.72;

    constexpr numeric SIGMOID_Q = 5;

    constexpr numeric sigmoid(numeric x) {
        return SIGMOID_Q * (1 - std::exp(-(std::exp(x)-1) / SIGMOID_Q));
    }

    // Default weights for the K3 set were moved to K3Config in k3.hpp
}