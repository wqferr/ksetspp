#include "ksets/k3.hpp"

using ksets::K3Config, ksets::K3, ksets::numeric, ksets::rngseed;

// CONFIG
constexpr int NUM_UNITS = 5;
constexpr numeric INITIAL_REST = 500;
constexpr rngseed SEED = 1997'12'02;
constexpr int HISTORY_SIZE_MS = 4'000;
// END CONFIG

#include <iostream>
#include <random>

using std::strtoul, std::strtof;

enum Arg {
    PROGNAME = 0,

    W_OB_AON,
    W_OB_PC,
    W_AON_OB, // D1
    W_AON_PG, // D2
    W_PC_AON, // D3
    W_DPC_OB, // D4
    W_OB_LAT_E, // lateral excitatory weight
    W_OB_LAT_I, // lateral inhibitory weight
    NARGS
};

std::function<rngseed()> randomDeviceSeedGenerator(rngseed seed) {
    std::deque<rngseed> batch(32, 0);
    std::size_t i = batch.size() - 1;
    std::random_device rd {};
    return [batch = std::move(batch), seed, i]() mutable {
        static std::seed_seq seedGen({ seed });

        i++;
        if (i == batch.size()) {
            seedGen.generate(batch.begin(), batch.end());
            i = 0;
        }
        return batch[i];
    };
}

struct Args {
    std::size_t numUnits;
    rngseed seed;
    K3Config config;
};

K3Config parseArgs(int argc, char *argv[]) {
    char *errptr;
    assert(argc >= NARGS);

    K3Config config;

    config.wOB_AON_lot = strtof(argv[W_OB_AON], &errptr);
    assert(!*errptr);

    config.wOB_PC_lot = strtof(argv[W_OB_PC], &errptr);
    assert(!*errptr);

    config.wAON_OB_toAntipodal = strtof(argv[W_AON_OB], &errptr);
    assert(!*errptr);

    config.wAON_PG_mot = strtof(argv[W_AON_PG], &errptr);
    assert(!*errptr);

    config.wPC_AON_toAntipodal = strtof(argv[W_PC_AON], &errptr);
    assert(!*errptr);

    config.wDPC_OB_toAntipodal = strtof(argv[W_DPC_OB], &errptr);
    assert(!*errptr);

    config.wOB_inter[0] = strtof(argv[W_OB_LAT_E], &errptr);
    assert(!*errptr);

    config.wOB_inter[1] = strtof(argv[W_OB_LAT_I], &errptr);
    assert(!*errptr);

    assert(config.checkWeightsValidity());

    return config;
}

int main(int argc, char *argv[]) {
    K3Config config = parseArgs(argc, argv);
    K3 model(NUM_UNITS, INITIAL_REST, randomDeviceSeedGenerator(SEED), config);
    // TODO do simulation
}