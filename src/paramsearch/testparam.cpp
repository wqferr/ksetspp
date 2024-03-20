#include "ksets/k3.hpp"

using ksets::K3Config, ksets::K3, ksets::K0, ksets::numeric, ksets::rngseed;

// CONFIG
constexpr int NUM_UNITS = 5;
constexpr numeric INITIAL_REST = 500;
constexpr rngseed SEED = 1997'12'02;
constexpr int PROCEDURE_DURATION_MS = 5'000;
// END CONFIG

constexpr int PROCEDURE_DURATION_ITERS = ksets::odeMillisecondsToIters(PROCEDURE_DURATION_MS);

#define assertm(exp, msg) assert(((void)msg, exp))

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
    W_DPC_PC,
    W_PC_DPC,
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
    assertm(argc >= NARGS, "Not enough arguments");

    K3Config config;
    config.outputActivityMonitoring = 0;
    config.outputHistorySize = PROCEDURE_DURATION_MS;

    config.wOB_AON_lot = strtof(argv[W_OB_AON], &errptr);
    assertm(!*errptr, "Invalid wOB_AON");

    config.wOB_PC_lot = strtof(argv[W_OB_PC], &errptr);
    assertm(!*errptr, "Invalid wOB_PC");

    config.wAON_OB_toAntipodal = strtof(argv[W_AON_OB], &errptr);
    assertm(!*errptr, "Invalid wAON_OB");

    config.wAON_PG_mot = strtof(argv[W_AON_PG], &errptr);
    assertm(!*errptr, "Invalid wAON_PG");

    config.wPC_AON_toAntipodal = strtof(argv[W_PC_AON], &errptr);
    assertm(!*errptr, "Invalid wPC_AON");

    config.wDPC_OB_toAntipodal = strtof(argv[W_DPC_OB], &errptr);
    assertm(!*errptr, "Invalid wDPC_OB");

    config.wDPC_PC = strtof(argv[W_DPC_PC], &errptr);
    assertm(!*errptr, "Invalid wDPC_PC");

    config.wPC_DPC = strtof(argv[W_PC_DPC], &errptr);
    assertm(!*errptr, "Invalid wPC_DPC");

    config.wOB_inter[0] = strtof(argv[W_OB_LAT_E], &errptr);
    assertm(!*errptr, "Invalid wOB_inter[0]");

    config.wOB_inter[1] = strtof(argv[W_OB_LAT_I], &errptr);
    assertm(!*errptr, "Invalid wOB_inter[1]");

    assertm(config.checkWeightsValidity(), "One or more weights had the wrong sign");

    return config;
}

void doSimulation(K3& model) {
    // STEP 1: rest for 1000ms
    model.rest(1'000);

    // STEP 2: present 1 in the first unit, 0 in the rest
    std::vector<numeric> pattern(NUM_UNITS, 0);
    pattern[0] = 1;
    model.present(1'000, pattern.begin(), pattern.end());

    // STEP 3: rest for 1000ms
    model.rest(1'000);

    // STEP 4: present 1 in the last unit, 0 in the rest
    pattern[0] = 0;
    pattern[NUM_UNITS-1] = 1;
    model.present(1'000, pattern.begin(), pattern.end());

    // STEP 5: rest for 1000ms
    model.rest(1'000);

    // TOTAL PROCEDURE DURATION: 5000ms
}

void writeCsv(const ksets::ActivationHistory& history) {
    // yes, both get and tail take len-1 as argument
    std::cout << history.get(PROCEDURE_DURATION_ITERS-1);
    for (auto iter = history.tail(PROCEDURE_DURATION_ITERS-1); iter != history.end(); iter++)
        std::cout << ',' << *iter;
    std::cout << '\n';
}

void writeToStdout(const K3& model) {
    for (auto& unit : model.getOlfactoryBulb())
        writeCsv(unit.primaryNode()->getActivationHistory());
}

int main(int argc, char *argv[]) {
    K3Config config = parseArgs(argc, argv);
    K3 model(NUM_UNITS, INITIAL_REST, randomDeviceSeedGenerator(SEED), config);
    doSimulation(model);
    writeToStdout(model);
}