#include <cstdio>
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <random>

#include "ksets/k3.hpp"

void writeCsv(std::ofstream& ofs, std::size_t len, const ksets::ActivationHistory& history) {
    ofs << history.get(len);
    for (auto iter = history.tail(len-1); iter != history.end(); iter++)
        ofs << ',' << *iter;
    ofs << '\n';
}

void writeCsv(std::ofstream& ofs, std::size_t len, const ksets::K0& node) {
    writeCsv(ofs, len, node.getActivationHistory());
}


int main(void) {
    std::ofstream ofs("data.csv");
    // std::size_t fileHistSize = 1000;

    // Seeded RNG
    std::mt19937 gen {0};
    std::normal_distribution<ksets::numeric> dist {0.0, 0.02};
    std::function<ksets::numeric()> rng = [&gen, &dist]() mutable {return dist(gen);};
    // ------------------
    // ksets::K2 k2({1.8, 1.0, -2.0, -0.8});
    // k2.randomizeK0States(rng);
    // k2.primaryNode()->setHistorySize(20000);
    // for (std::size_t i = 0; i < fileHistSize; i++)
    //     k2.calculateAndCommitNextState();
    // writeCsv(ofs, 500, *k2.primaryNode());
    // ------------------
    ksets::K3 model(4, 5000, rng);

    std::vector<ksets::numeric> pattern = {2, 1, 0, 3};
    model.present(400, pattern.begin(), pattern.end());
    model.rest(200);

    std::size_t fileHistSize = ksets::odeMillisecondsToIters(1000);
    writeCsv(ofs, fileHistSize, model.getOlfactoryBulb().getAveragePrimaryActivationHistory());
    // writeCsv(ofs, fileHistSize, model.getOlfactoryBulb().getAverageAntipodalActivationHistory());
    writeCsv(ofs, fileHistSize, *model.getDeepPyramidCells());
    writeCsv(ofs, fileHistSize, *model.getPrepiriformCortexPrimary());
    // for (auto& node : model.getOlfactoryBulbPrimaryNodes())
    //     writeCsv(ofs, fileHistSize, *node);
    // for (auto& node : model.getOlfactoryBulbAntipodalNodes())
    //     writeCsv(ofs, fileHistSize, *node);
}
