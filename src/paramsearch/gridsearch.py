#!/usr/bin/env python3

# must be executed from this folder, not project root
from sklearn.model_selection import GridSearchCV
from scipy.stats import normaltest
from scipy.signal import hilbert
from statsmodels.tsa.stattools import adfuller
import numpy as np
from numpy.fft import fft, fftfreq
import subprocess
from matplotlib import pyplot as plt

NORMAL_TEST_P_VALUE = 0.05
NORMAL_SCORE_BONUS = 50
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

class K3Model:
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

    def __init__(self, wOB_AON, wOB_PC, wAON_OB, wAON_PG, wPC_AON, wDPC_OB, wDPC_PC, wPC_DPC, wOB_LAT_E, wOB_LAT_I):
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

    def fit(self, data):
        pass

    def to_tuple(self):
        return (
            self.wOB_AON,
            self.wOB_PC,
            self.wAON_OB,
            self.wAON_PG,
            self.wPC_AON,
            self.wDPC_OB,
            self.wDPC_PC,
            self.wPC_DPC, # negative
            self.wOB_LAT_E,
            self.wOB_LAT_I # negative
        )

    def score_minimize_weights(self):
        s = 0
        for w in self.to_tuple():
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
        z = hilbert(active_segment_avg)
        mod_wave = np.abs(z)  # Extract envelope
        mod_fft = fft(mod_wave)  # Get frequency domain of envelope
        mod_freq = fftfreq(len(mod_wave), TIME_STEP)
        undesired_freq_idx = np.concatenate(mod_freq < 40, mod_freq > 100)
        undesired_freq = mod_freq[undesired_freq_idx]
        undesired_fft = mod_fft[undesired_freq_idx]
        desired_freq_idx = ~undesired_freq_idx
        desired_freq = mod_freq[desired_freq_idx]
        desired_fft = mod_fft[desired_freq_idx]
        # TODO: analyze desired and undesired FFT

    def run_with_params(self):
        cmd = ["../../build/testparam"]
        cmd += [str(w) for w in self.to_tuple()]
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
        return total_score

    def score(self, data):
        if not self.cached_score:
            self.cached_score = self.calc_score()
        return self.cached_score

def main():
    model = K3Model(1, 1, 1, 1, 1, 1, 1, -1, 1, -1)
    print(model.score(None))

if __name__ == "__main__":
    main()
