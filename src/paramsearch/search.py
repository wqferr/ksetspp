from pygad import pygad
import random
from collections import namedtuple

SEED = 1997_12_02
random.seed(SEED)

CHROMOSOME = dict(
    wOB_AON=0,
    wOB_PC=1,
    wAON_OB=2,
    wAON_PG=3,
    wPC_AON=4,
    wDPC_OB=5,
    # wDPC_PC
    wOB_LAT_E=6,
    wOB_LAT_I=7,
)

RANGES = [
    {"min": 0.2, "max": 10}, # wOB_AON
    {"min": 0.2, "max": 10}, # wOB_PC
    {"min": 0.2, "max": 10}, # wAON_OB
    {"min": 0.2, "max": 10}, # wAON_PG
    {"min": 0.2, "max": 10}, # wPC_AON
    {"min": 0.2, "max": 10}, # wDPC_OB
    {"min": 0.2, "max": 10}, # wDPC_PC
    {"min": -8, "max": -0.2}, # wPC_DPC
    {"min": 0.2, "max": 10}, # wOB_lateral_exc
    {"min": -8, "max": -0.2}, # wOB_lateral_inhib
]

def fitness(chromosome: list[float]) -> float:
    return 0

def main():
    ga = pygad.GA(
        fitness_func=fitness,
        sol_per_pop=20,
        gene_type=float,
        num_genes=8,
        gene_space=RANGES,
        random_seed=SEED
    )

if __name__ == "__main__":
    main()