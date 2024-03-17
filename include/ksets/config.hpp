#pragma once

#include <cstddef>
#include <cmath>
#include <array>

namespace ksets {
    using numeric = float;
    using rngseed = uint64_t;

    constexpr std::size_t DEFAULT_HISTORY_SIZE = 1'000;
    constexpr numeric DEFAULT_LEARN_RATE = 0.05;

    constexpr numeric ODE_STEP_SIZE = 0.5;
    constexpr std::size_t odeMillisecondsToIters(numeric milliseconds) {
        return std::ceil(milliseconds / ODE_STEP_SIZE);
    }
    constexpr numeric odeItersToMilliseconds(std::size_t nIter) {
        return nIter * ODE_STEP_SIZE;
    }

    constexpr numeric ODE_A_DECAY_RATE = 0.22;
    constexpr numeric ODE_B_RISE_RATE = 0.72;

    constexpr numeric SIGMOID_Q = 5;

    constexpr numeric sigmoid(numeric x) {
        return std::max(
            SIGMOID_Q * (1 - std::exp(-(std::exp(x)-1) / SIGMOID_Q)),
            static_cast<numeric>(-1.0)
        );
    }

    constexpr numeric K3_RANDOM_K0_INIT_STD_DEV = 0.20;
    constexpr std::size_t K3_NOISE_RANDOM_SEED_GEN_BATCH_SIZE = 32;
    constexpr numeric K3_PG_NOISE_STD_DEV = 0.025;
    constexpr numeric K3_OB_NOISE_STD_DEV = 0.025;
    constexpr numeric K3_AON_NOISE_STD_DEV = 0.025;

    // Default weights for the K3 set were moved to K3Config in k3.hpp
}