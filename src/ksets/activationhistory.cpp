#include <ksets/activationhistory.hpp>

using ksets::ActivationHistory;
using ksets::numeric;

ActivationHistory::ActivationHistory(std::size_t historySize): history(historySize, 0) {}

void ActivationHistory::put(numeric value) {
    history.pop_back();
    // push at the front so accessing with an larger
    // index goes backwards in time
    history.push_front(value);
}

numeric ActivationHistory::get(std::size_t offset=0) const {
    return history.at(offset);
}