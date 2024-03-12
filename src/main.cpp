#include <cstdio>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <random>

#include "ksets/k3.hpp"

void writeCsv(std::ofstream& ofs, std::size_t len, const ksets::ActivationHistory& history) {
    for (auto iter = history.tail(len); iter != history.end(); iter++)
        ofs << *iter << ',';
    ofs << '\n';
}

void writeCsv(std::ofstream& ofs, std::size_t len, const ksets::K0& node) {
    writeCsv(ofs, len, node.getActivationHistory());
}

using ksets::K3;

int main(void) {
    // Seeded RNG
    std::mt19937 gen {0};
    std::normal_distribution dist {0.0, 0.02};
    auto rng = [&gen, &dist]() {return static_cast<ksets::numeric>(dist(gen));};
    K3 model(4, 3000, rng);

    std::vector<ksets::numeric> pattern = {0, 1, 1, 0};
    model.present(2000, pattern.begin(), pattern.end());
    model.rest(500);

    std::size_t fileHistSize = 2000;
    std::ofstream ofs("data.csv");
    // writeCsv(ofs, fileHistSize, *model.getDeepPyramidCells());
    writeCsv(ofs, fileHistSize, model.getOlfactoryBulb().getAveragePrimaryActivationHistory());
    writeCsv(ofs, fileHistSize, model.getOlfactoryBulb().getAverageAntipodalActivationHistory());
    // for (auto& node : model.getOlfactoryBulbPrimaryNodes())
    //     writeCsv(ofs, fileHistSize, *node);
    // for (auto& node : model.getOlfactoryBulbAntipodalNodes())
    //     writeCsv(ofs, fileHistSize, *node);
}
