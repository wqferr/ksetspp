#include <cstdio>
#include <string>
#include <fstream>
#include <vector>
#include <utility>

#include "ksets/activationhistory.hpp"
#include "ksets/k1.hpp"

void writeCsv(std::ofstream& ofs, std::size_t len, const ksets::K0& node) {
    auto history = node.getActivationHistory();
    for (auto iter = history.tail(len); iter != history.end(); iter++) {
        ofs << *iter << ',';
    }
    ofs << '\n';
}

int main(void) {
    ksets::K0 k0;
    ksets::K1 k1(0.9, 0.8);
    for (int i = 0; i < 5; i++) {
        k0.calculateAndCommitNextState(1);
        k1.calculateAndCommitNextState(1);
    }
    for (int i = 0; i < 190; i++) {
        k0.calculateAndCommitNextState(0);
        k1.calculateAndCommitNextState(0);
    }
    std::size_t fileHistSize = 250;
    std::ofstream ofs("data.csv");
    writeCsv(ofs, fileHistSize, k0);
    writeCsv(ofs, fileHistSize, k1.primaryNode());
    writeCsv(ofs, fileHistSize, k1.secondaryNode());
}
