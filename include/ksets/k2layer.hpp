#pragma once

#include <initializer_list>

#include "ksets/k2.hpp"

namespace ksets {
    class K2Layer {
        std::vector<K2> units;
    public:
        K2Layer(std::size_t nUnits, const K2Weights intraUnitWeights);
        K2Layer(const K2Layer&) = delete;

        bool connectPrimaryNodes(numeric interUnitWeight, std::size_t delay);
        bool connectAntipodalNodes(numeric interUnitWeight, std::size_t delay);

        std::size_t size() const;

        template<typename RNG>
        void perturbIntraUnitWeights(RNG& rng) {
            for (auto& unit : units)
                unit.perturbWeights(rng);
        }

        K2& unit(std::size_t index);
        const K2& unit(std::size_t index) const;

        auto begin() {
            return units.begin();
        }

        auto end() {
            return units.end();
        }

        const auto begin() const {
            return units.begin();
        }

        const auto end() const {
            return units.end();
        }

        bool setExternalStimulus(std::initializer_list<numeric> values);
        void calculateNextState();
        bool calculateNextState(std::initializer_list<numeric> newExternalStimulus);
        void commitNextState();
        void calculateAndCommitNextState();
        bool calculateAndCommitNextState(std::initializer_list<numeric> newExternalStimulus);

        template<typename T>
        bool setExternalStimulus(T valuesBegin, T valuesEnd) {
            auto unitIter = this->begin();
            auto valueIter = valuesBegin;

            while (unitIter != end() && valueIter != valuesEnd) {
                unitIter->setExternalStimulus(*valueIter);
                unitIter++;
                valueIter++;
            }
            return unitIter == end() && valueIter == valuesEnd;
        }

        template<typename T>
        bool calculateNextState(T stimulusBegin, T stimulusEnd) {
            if (!setExternalStimulus(stimulusBegin, stimulusEnd))
                return false;
            calculateNextState();
            return true;
        }

        template<typename T>
        bool calculateAndCommitNextState(T stimulusBegin, T stimulusEnd) {
            if (!calculateNextState(stimulusBegin, stimulusEnd))
                return false;
            commitNextState();
            return true;
        }
    };
}