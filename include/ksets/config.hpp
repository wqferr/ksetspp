#pragma once

#include <cstddef>
#include <cmath>
#include <array>

namespace ksets {
    typedef float numeric;
    constexpr std::size_t HISTORY_SIZE = 20'000;
    constexpr numeric DEFAULT_LEARN_RATE = 0.05;

    constexpr numeric ODE_STEP_SIZE = 0.5;
    constexpr std::size_t ODE_ITERS_PER_MILLISECOND = std::ceil(1 / ODE_STEP_SIZE);
    constexpr numeric ODE_A_DECAY_RATE = 0.22;
    constexpr numeric ODE_B_RISE_RATE = 0.72;

    constexpr numeric SIGMOID_Q = 5;

    constexpr numeric sigmoid(numeric x) {
        return SIGMOID_Q * (1 - std::exp(-(std::exp(x)-1) / SIGMOID_Q));
    }

    constexpr std::array<numeric, 4> K3_LAYER_1_DEFAULT_INTRA_WEIGHTS = {1.8, 1.0, -2.0, -0.8};
    constexpr std::array<numeric, 4> K3_LAYER_2_DEFAULT_INTRA_WEIGHTS = {1.6, 1.6, -1.5, -2.0};
    constexpr std::array<numeric, 4> K3_LAYER_3_DEFAULT_INTRA_WEIGHTS = {1.6, 1.9, -0.2, -1.0};

    constexpr std::array<numeric, 2> K3_LAYER_1_DEFAULT_INTER_WEIGHTS = {0.15, -0.1};
    constexpr std::array<numeric, 2> K3_LAYER_2_DEFAULT_INTER_WEIGHTS = {0.02, -0.2};
    constexpr std::array<numeric, 2> K3_LAYER_3_DEFAULT_INTER_WEIGHTS = {0.15, -0.1};
}