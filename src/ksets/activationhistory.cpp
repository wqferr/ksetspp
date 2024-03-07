#include <ksets/activationhistory.hpp>

using ksets::ActivationHistory;
using ksets::numeric;

ActivationHistory::ActivationHistory(std::size_t historySize)
    : history(historySize)
{}

void ActivationHistory::put(numeric value) {
    history.pop_front();
    history.push_back(value);
}

numeric ActivationHistory::get(std::size_t offset=0) const {
    std::size_t index = history.size() - offset - 1;
    return history.at(index);
}
