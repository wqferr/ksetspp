#include "ksets/activationhistory.hpp"

#include <stdexcept>
#include <cassert>

using ksets::ActivationHistory;
using ksets::numeric;

ActivationHistory::ActivationHistory(std::size_t historySize):
    history(historySize, 0),
    monitoredWindow(std::nullopt) {}

void ActivationHistory::put(numeric value) {
    if (monitoredWindow.has_value())
        doMonitoring(value);
    history.pop_front();
    history.push_back(value);
}

numeric ActivationHistory::get(std::size_t offset=0) const {
    std::size_t index = history.size() - offset - 1;
    return history.at(index);
}

void ActivationHistory::initMonitoring() {
    assert(monitoredWindow.has_value());
    auto m = monitoredWindow.value();
    std::pair<numeric, numeric> result = varianceNumeratorAndSum(m.windowSize);
    m.varianceNumerator = result.first;
    m.sum = result.second;
}

void ActivationHistory::setActivityMonitoring(std::size_t windowSize) {
    if (windowSize > history.size())
        throw std::invalid_argument("Monitoring window must be less than or equal to history size");

    if (windowSize == 0) {
        monitoredWindow.reset();
    } else {
        monitoredWindow.emplace(windowSize);
        initMonitoring();
    }
}

// This was adapted from the best response (and specifically its commenst) from
// this Stack Overflow question:
// https://stackoverflow.com/questions/5147378/rolling-variance-algorithm
void ActivationHistory::doMonitoring(numeric newestValue) {
    auto& m = monitoredWindow.value();
    auto oldestValue = get(m.windowSize+1);
    auto oldMean = m.sum / m.windowSize;
    auto newSum = (m.sum - oldestValue + newestValue);
    auto newMean = newSum / m.windowSize;
    m.varianceNumerator += (newestValue + oldestValue - oldMean - newMean) * (newestValue - oldestValue);
    m.sum = newSum;
}

std::pair<numeric, numeric> ActivationHistory::varianceNumeratorAndSum(std::size_t window) const {
    if (window == 0) return {0, 0};
    if (window == 1) return {get(0), 0};
    numeric sum = get(window);
    numeric varianceNum = 0;
    std::size_t n = 1;
    for (auto iter = tail(window-1); iter != end(); iter++) {
        numeric mean = sum / n;
        numeric newMean = (sum + *iter) / (n + 1);
        varianceNum += (*iter - mean) * (*iter - newMean);
        sum += *iter;
        n++;
    }
    return {varianceNum, sum};
}

numeric ActivationHistory::variance() const {
    if (!monitoredWindow.has_value())
        throw std::runtime_error("Cannot infer variance window size without activity monitoring enabled");
    return variance(monitoredWindow.value().windowSize);
}

numeric ActivationHistory::variance(std::size_t window) const {
    if (window < 2) return 0;

    if (monitoredWindow.has_value() && monitoredWindow.value().windowSize == window)
        return monitoredWindow.value().varianceNumerator / (window - 1);
    else
        return varianceNumeratorAndSum(window).first / (window - 1);
}

// numeric ActivationHistory::varianceSlow(std::size_t window) const {
//     numeric mean = 0;
//     for (std::size_t i = 0; i < window; i++)
//         mean += get(i);
//     mean /= window;
//     numeric var = 0;
//     for (std::size_t i = 0; i < window; i++)
//         var += (get(i) - mean) * (get(i) - mean);
//     return var / (window - 1);
// }

numeric ActivationHistory::stddev() const {
    if (!monitoredWindow.has_value())
        throw std::runtime_error("Cannot infer stddev window size without activity monitoring enabled");
    return stddev(monitoredWindow.value().windowSize);
}

numeric ActivationHistory::stddev(std::size_t window) const {
    if (window < 2)
        return 0;
    else
        return std::sqrt(variance(window) / (window - 1));
}
