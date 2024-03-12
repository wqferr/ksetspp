#include "ksets/activationhistory.hpp"

#include <stdexcept>
#include <cassert>

using ksets::ActivationHistory;
using ksets::numeric;

ActivationHistory::ActivationHistory(std::size_t historySize):
    history(historySize, {0, 0}),
    monitoredWindow(std::nullopt) {}

void ActivationHistory::put(numeric rawValue) {
    if (monitoredWindow.has_value())
        doMonitoring(rawValue);
    history.pop_front();
    history.push_back({rawValue, ksets::sigmoid(rawValue)});
}

numeric ActivationHistory::get(std::size_t offset) const {
    std::size_t index = history.size() - offset - 1;
    return history.at(index).raw;
}

numeric ActivationHistory::getSigmoid(std::size_t offset) const {
    std::size_t index = history.size() - offset - 1;
    return history.at(index).sigmoid;
}

void ActivationHistory::resize(std::size_t newSize) {
    history.resize(newSize);
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

// Welford's algorithm allows us to compute the RMS with a single pass and *more* numerical stability
// https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
std::pair<numeric, numeric> ActivationHistory::varianceNumeratorAndSum(std::size_t window) const {
    if (window == 0) return {0, 0};
    if (window == 1) return {0, get(0)};
    numeric sum = get(window);
    numeric varianceNum = 0;
    std::size_t n = 1;
    for (auto iter = tail(window-1); iter != end(); iter++) {
        numeric mean = sum / n;
        numeric newMean = (sum + iter->raw) / (n + 1);
        varianceNum += (iter->raw - mean) * (iter->raw - newMean);
        sum += iter->raw;
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
