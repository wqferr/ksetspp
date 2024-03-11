#include <cstdio>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <random>

#include "ksets/k3.hpp"

void writeCsv(std::ofstream& ofs, std::size_t len, const ksets::K0& node) {
    auto history = node.getActivationHistory();
    for (auto iter = history.tail(len); iter != history.end(); iter++)
        ofs << *iter << ',';
    ofs << '\n';
}

using ksets::K3;

int main(void) {
    // Seeded RNG
    std::mt19937 gen {0};
    std::normal_distribution dist {0.0, 0.02};
    auto rng = [&gen, &dist]() {return static_cast<ksets::numeric>(dist(gen));};
    K3 model(4, 3000, rng);

    std::size_t fileHistSize = ksets::odeMillisecondsToIters(100);
    std::ofstream ofs("data.csv");
    for (auto& node : model.getOlfactoryBulbPrimaryNodes())
        writeCsv(ofs, fileHistSize, *node);
    for (auto& node : model.getOlfactoryBulbAntipodalNodes())
        writeCsv(ofs, fileHistSize, *node);
}
