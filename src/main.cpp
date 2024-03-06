#include <cstdio>
#include <string>
#include <fstream>

#include "ksets/activationhistory.hpp"
#include "ksets/k0.hpp"

void writeCsv(std::string filename, const ksets::ActivationHistory& history, std::size_t len) {
    std::ofstream ofs(filename);
    for (auto iter = history.tail(len); iter != history.end(); iter++) {
        ofs << *iter << '\n';
    }
}

int main(void) {
    ksets::K0 myFirstK0;
    myFirstK0.calculateAndCommitNextState(1);
    for (int i = 0; i < 98; i++)
        myFirstK0.calculateAndCommitNextState(0);
    writeCsv("data.csv", myFirstK0.getActivationHistory(), 100);
}
