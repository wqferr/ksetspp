#include <cstdio>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <random>

#include "ksets/activationhistory.hpp"
#include "ksets/k0.hpp"
#include "ksets/k1.hpp"
#include "ksets/k2.hpp"
#include "ksets/k2layer.hpp"

void writeCsv(std::ofstream& ofs, std::size_t len, const ksets::K0 *node) {
    auto history = node->getActivationHistory();
    for (auto iter = history.tail(len); iter != history.end(); iter++) {
        ofs << *iter << ',';
    }
    ofs << '\n';
}

int main(void) {
    ksets::K0 k0;
    ksets::K1 k1(0.8, 1.1);
    ksets::K2 k2({ 1.8, 1.0, -2.0, -0.8 });
    // ksets::K2Layer k2l(4, {1.8, 1.0, -2.0, -0.8});

    // k2l.connectPrimaryNodes(1.0, 4);
    // k2l.connectAntipodalNodes(-1.0, 4);

    // seeded RNG
    // std::mt19937 gen {0};
    // std::normal_distribution dist {0.0, 0.005};
    // auto rng = [&gen, &dist]() {return dist(gen);};
    // k2.perturbWeights(rng);

    for (int i = 0; i < 1; i++) {
        // k0.calculateAndCommitNextState(1);
        // k1.calculateAndCommitNextState(1);
        k2.calculateAndCommitNextState(1);
        // k2l.calculateAndCommitNextState({ 0, 1, 0, 2 });
    }
    for (int i = 0; i < 190; i++) {
        // k0.calculateAndCommitNextState(0);
        // k1.calculateAndCommitNextState(0);
        k2.calculateAndCommitNextState(0);
        // k2l.calculateAndCommitNextState({ 0, 0, 0, 0 });
    }
    auto otherk = ksets::K2(k2);
    for (int i = 0; i < 30; i++) {
        otherk.calculateAndCommitNextState(0);
    }

    std::size_t fileHistSize = 210;
    std::ofstream ofs("data.csv");
    // writeCsv(ofs, fileHistSize, k0);
    // writeCsv(ofs, fileHistSize, k1.primaryNode());
    writeCsv(ofs, fileHistSize, k2.primaryNode());
    writeCsv(ofs, fileHistSize, otherk.primaryNode());
    // writeCsv(ofs, fileHistSize, k2.antipodalNode());
    // for (auto& unit : k2l) {
        // writeCsv(ofs, fileHistSize, unit.primaryNode());
        // writeCsv(ofs, fileHistSize, unit.antipodalNode());
    // }
}
