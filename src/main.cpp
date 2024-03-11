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

void writeCsv(std::ofstream& ofs, std::size_t len, const ksets::K0& node) {
    auto history = node.getActivationHistory();
    for (auto iter = history.tail(len); iter != history.end(); iter++) {
        ofs << *iter << ',';
    }
    ofs << '\n';
}

int main(void) {
    // Seeded RNG
    std::mt19937 gen {0};
    std::normal_distribution dist {0.0, 0.02};
    auto rng = [&gen, &dist]() {return static_cast<ksets::numeric>(dist(gen));};

    std::size_t fileHistSize = 1500;
    std::ofstream ofs("data.csv");
    // writeCsv(ofs, fileHistSize, k0);
    // writeCsv(ofs, fileHistSize, *k1.primaryNode());
    // writeCsv(ofs, fileHistSize, *k1.secondaryNode());
    // writeCsv(ofs, fileHistSize, *k2.primaryNode());
    // writeCsv(ofs, fileHistSize, *k2.antipodalNode());
    // writeCsv(ofs, fileHistSize, *otherk.antipodalNode());
    // for (auto& unit : k2l) {
    //     writeCsv(ofs, fileHistSize, *unit.primaryNode());
    //     writeCsv(ofs, fileHistSize, *unit.antipodalNode());
    // }
}
