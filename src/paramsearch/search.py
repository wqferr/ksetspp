from pygad import pygad
import random
from collections import namedtuple
from typing import List

SEED = 1997_12_02
random.seed(SEED)

WEIGHT_SIGNS = [
    +1,
    +1,
    +1,
    +1,
    +1,
    +1,
    +1,
    -1,
    +1,
    -1
]

RANGES = []
for sign in WEIGHT_SIGNS:
    if sign > 0:
        RANGES.append({"low": 0.2, "high": 10})
    else:
        RANGES.append({"low": -8, "high": -0.2})

def weight_minimizing_component(chromosome: List[float]):
    total = 0
    for value in chromosome:
        # this is meant to be smaller (more negative) the larger
        # the absolute value of the weights are
        total -= value ** 2
    return total

def fitness(ga: pygad.GA, chromosome: List[float], index: int) -> float:
    total = 0
    total += weight_minimizing_component(chromosome)
    return total

def main():
    ga = pygad.GA(
        fitness_func=fitness,
        num_generations=50,
        num_parents_mating=2,
        sol_per_pop=20,
        gene_type=float,
        num_genes=10,
        gene_space=RANGES,
        random_seed=SEED
    )
    ga.run()
    ga.plot_fitness()

if __name__ == "__main__":
    main()