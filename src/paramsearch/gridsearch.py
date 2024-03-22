#!/usr/bin/env python3

# must be executed from this folder, not project root
from sklearn.model_selection import GridSearchCV
from sklearn.base import BaseEstimator
import code
from scipy.stats import normaltest
from scipy.signal import hilbert
from statsmodels.tsa.stattools import adfuller
import numpy as np
from numpy.fft import fft, fftfreq
import subprocess
from matplotlib import pyplot as plt

NORMAL_TEST_P_VALUE = 0.05
NORMAL_SCORE_BONUS = 50
AMP_MOD_WEIGHT = 1/2500
TIME_STEP = 1/2000

# K3Weights = namedtuple(
#     "K3Weights",
#     [
#         "wOB_AON",
#         "wOB_PC",
#         "wAON_OB",
#         "wAON_PG",
#         "wPC_AON",
#         "wDPC_OB",
#         "wDPC_PC",
#         "wPC_DPC",
#         "wOB_LAT_E",
#         "wOB_LAT_I",
#     ]
# )

class K3Model(BaseEstimator):
    wOB_AON: float
    wOB_PC: float
    wAON_OB: float
    wAON_PG: float
    wPC_AON: float
    wDPC_OB: float
    wDPC_PC: float
    wPC_DPC: float
    wOB_LAT_E: float
    wOB_LAT_I: float

    def __init__(
        self,
        *,
        wOB_AON=1,
        wOB_PC=1,
        wAON_OB=1,
        wAON_PG=1,
        wPC_AON=1,
        wDPC_OB=1,
        wDPC_PC=1,
        wPC_DPC=-1,
        wOB_LAT_E=1,
        wOB_LAT_I=-1
    ):
        self.wOB_AON = wOB_AON
        self.wOB_PC = wOB_PC
        self.wAON_OB = wAON_OB
        self.wAON_PG = wAON_PG
        self.wPC_AON = wPC_AON
        self.wDPC_OB = wDPC_OB
        self.wDPC_PC = wDPC_PC
        self.wPC_DPC = wPC_DPC
        self.wOB_LAT_E = wOB_LAT_E
        self.wOB_LAT_I = wOB_LAT_I
        self.cached_score = None

    def fit(self, *args):
        return self

    def get_params(self, deep=True):
        return {
            "wOB_AON": self.wOB_AON,
            "wOB_PC": self.wOB_PC,
            "wAON_OB": self.wAON_OB,
            "wAON_PG": self.wAON_PG,
            "wPC_AON": self.wPC_AON,
            "wDPC_OB": self.wDPC_OB,
            "wDPC_PC": self.wDPC_PC,
            "wPC_DPC": self.wPC_DPC, # negative
            "wOB_LAT_E": self.wOB_LAT_E,
            "wOB_LAT_I": self.wOB_LAT_I # negative
        }

    def set_params(self, **params):
        self.wOB_AON = params.get("wOB_AON", self.wOB_AON)
        self.wOB_PC = params.get("wOB_PC", self.wOB_PC)
        self.wAON_OB = params.get("wAON_OB", self.wAON_OB)
        self.wAON_PG = params.get("wAON_PG", self.wAON_PG)
        self.wPC_AON = params.get("wPC_AON", self.wPC_AON)
        self.wDPC_OB = params.get("wDPC_OB", self.wDPC_OB)
        self.wDPC_PC = params.get("wDPC_PC", self.wDPC_PC)
        self.wPC_DPC = params.get("wPC_DPC", self.wPC_DPC)
        self.wOB_LAT_E = params.get("wOB_LAT_E", self.wOB_LAT_E)
        self.wOB_LAT_I = params.get("wOB_LAT_I", self.wOB_LAT_I)
        self.cached_score = None
        return self


    def score_minimize_weights(self):
        s = 0
        for w in self.get_params().values():
            s += w*w
        return -s

    def score_resting_normal(self, avg):
        _, pvalue = normaltest(avg)
        if pvalue <= NORMAL_TEST_P_VALUE:
            return NORMAL_SCORE_BONUS
        else:
            return 0

    def score_resting_spectrum_pink_noise(self, resting_avg_fft, resting_fft_freqs):
        # From this post's accepted answer:
        # https://stackoverflow.com/questions/15382076/plotting-power-spectrum-in-python

        power = np.abs(resting_avg_fft) ** 2
        resting_fft_freqs = resting_fft_freqs[resting_fft_freqs > 0]
        idx = np.argsort(resting_fft_freqs)

        power = power[idx]
        power /= np.max(power)
        log_power = np.log(power)
        stddev = np.std(log_power)

        # Lower standard deviation is better, since it means the
        # log plot is flatter and thus closer to a constant
        return -stddev

    def score_amp_modulation_40_100_hz(self, active_segment_avg):
        """Check for 40-100Hz amplitude modulating waves in the signal while active."""
        z = hilbert(active_segment_avg)
        mod_wave = np.abs(z)  # Extract envelope
        mod_fft = fft(mod_wave)  # Get frequency domain of envelope
        mod_freq = fftfreq(len(mod_wave), TIME_STEP)
        undesired_freq_idx = (mod_freq < 40) | (mod_freq > 100)
        undesired_fft = np.abs(mod_fft[undesired_freq_idx]) ** 2
        desired_freq_idx = ~undesired_freq_idx
        desired_fft = np.abs(mod_fft[desired_freq_idx]) ** 2
        return AMP_MOD_WEIGHT * (np.average(desired_fft) + np.average(undesired_fft) / 4)

    def run_with_params(self):
        cmd = ["../../build/testparam"]
        cmd += [str(w) for w in self.get_params().values()]
        proc = subprocess.run(cmd, text=True, capture_output=True)

        # So now we can check its return code
        if proc.returncode > 0:
            # On error, return lowest possible score
            return -float("inf")

        # If it's fine, we do the proper processing

        # Skip last "\n"
        lines = proc.stdout.strip().split("\n")
        data = [np.fromstring(line, sep=",") for line in lines]
        return np.array(data)


    def calc_score(self):
        data = self.run_with_params()
        segments = np.split(data, axis=1, indices_or_sections=5)
        concat_resting_segments = np.concatenate((segments[0], segments[2], segments[4]), axis=1)
        concat_resting_segments_avg = np.average(concat_resting_segments, axis=0)
        resting_avg_fft = fft(concat_resting_segments_avg)
        resting_fft_freqs = fftfreq(len(concat_resting_segments_avg), TIME_STEP)

        total_score = 0
        total_score += self.score_minimize_weights()
        total_score += self.score_resting_normal(concat_resting_segments_avg)
        total_score += self.score_resting_spectrum_pink_noise(resting_avg_fft, resting_fft_freqs)
        total_score += self.score_amp_modulation_40_100_hz(np.average(segments[1], axis=0))
        total_score += self.score_amp_modulation_40_100_hz(np.average(segments[3], axis=0))
        return total_score

    def score(self, *args):
        if not self.cached_score:
            self.cached_score = self.calc_score()
        return self.cached_score

def main():
    model = K3Model()
    gs = GridSearchCV(
        model,
        param_grid={
            "wOB_AON": np.arange(1, 8, 0.5),
            "wOB_PC": np.arange(1, 8, 0.5),
            "wAON_OB": np.arange(1, 8, 0.5),
            "wAON_PG": np.arange(1, 8, 0.5),
            "wPC_AON": np.arange(1, 8, 0.5),
            "wDPC_OB": np.arange(1, 8, 0.5),
            "wDPC_PC": np.arange(1, 8, 0.5),
            "wPC_DPC": np.arange(-1, -5, -0.5),
            "wOB_LAT_E": np.arange(1, 8, 0.5),
            "wOB_LAT_I": np.arange(-1, -5, -0.5)
        },
        n_jobs=4,
        verbose=2,
        cv=2
    )
    gs.fit(np.zeros((2, 1)), np.zeros((2, 1)))
    code.interact(local=locals())


if __name__ == "__main__":
    main()
