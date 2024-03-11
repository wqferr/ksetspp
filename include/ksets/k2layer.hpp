#pragma once

#include <initializer_list>

#include "ksets/k2.hpp"

namespace ksets {
    class K2Layer {
        std::vector<K2> units;
    public:
        // throws if nUnits is 0
        explicit K2Layer(std::size_t nUnits, const K2Config intraUnitWeights);

        // why did I delete this?
        // K2Layer(const K2Layer&) = delete;

        // these return true if weight was valid => operation succeeded
        bool connectPrimaryNodes(numeric interUnitWeight, std::size_t delay=0) noexcept;
        bool connectAntipodalNodes(numeric interUnitWeight, std::size_t delay=0) noexcept;

        std::size_t size() const noexcept;

        // template<typename RNG>
        // void perturbIntraUnitWeights(RNG& rng) noexcept {
        //     for (auto& unit : units)
        //         unit.perturbWeights(rng);
        // }

        // these throw if index >= size()
        K2& unit(std::size_t index);
        const K2& unit(std::size_t index) const;

        auto begin() noexcept {
            return units.begin();
        }

        auto end() noexcept {
            return units.end();
        }

        const auto begin() const noexcept {
            return units.begin();
        }

        const auto end() const noexcept {
            return units.end();
        }

        // the functions that return bool signal whether the operation succeeded
        // via their return value
        bool setExternalStimulus(std::initializer_list<numeric> values) noexcept;
        void calculateNextState() noexcept;
        bool calculateNextState(std::initializer_list<numeric> newExternalStimulus) noexcept;
        void commitNextState() noexcept;
        void calculateAndCommitNextState() noexcept;
        bool calculateAndCommitNextState(std::initializer_list<numeric> newExternalStimulus) noexcept;

        template<typename Iterator>
        bool setExternalStimulus(Iterator valuesBegin, Iterator valuesEnd) noexcept {
            if (valuesEnd - valuesBegin != end() - begin())
                return false;
            auto unitIter = this->begin();
            auto valueIter = valuesBegin;

            while (unitIter != end() && valueIter != valuesEnd) {
                unitIter->setExternalStimulus(*valueIter);
                unitIter++;
                valueIter++;
            }
            return true;
        }

        template<typename Iterator>
        bool calculateNextState(Iterator stimulusBegin, Iterator stimulusEnd) {
            if (!setExternalStimulus(stimulusBegin, stimulusEnd))
                return false;
            calculateNextState();
            return true;
        }

        template<typename Iterator>
        bool calculateAndCommitNextState(Iterator stimulusBegin, Iterator stimulusEnd) {
            if (!calculateNextState(stimulusBegin, stimulusEnd))
                return false;
            commitNextState();
            return true;
        }

        void randomizeK0States(const std::function<numeric()>& rng);
    };
}