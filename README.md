# Basic templated genetic algorithm in C++ 

This project proposes a single header with 3 possible ways to call a genetic algorithm.

## Overview of the algorithm

Independently of the actual problem, a genetic algorithm is an iterative process defined by 3 methods:
* Stopping criteria: this algorithm stops after a maximum number of iterations, or if the same individual keeps being the best for a given number of iterations.
* Selection: the selection aims at keeping half of the current population in the next one. In our approach, this is done in two steps
  + elitism: the best individuals of the current population are forced in the next one.
  + tournament: 2 individuals are randomly selected and the best one is kept for the next generation.
  + after these two steps, half of the new generation is defined.
* Crossing and mutation: the other half of the new generation is defined by randomly crossing 2 individuals from the first half. The resulting individual is then mutated and added to the new generation.

The performance greatly depends on the initial random population, that is why it is usually better to stop quickly and to restart with a new population. This can be done sequentially or with multi-threading.

## Available calls to genetic algorithm solver

`solveSingleRun(T &indiv, YAML::Node config) ` 
  + Runs 1 instance of genetic algorithm based on the T class and optional configuration. 

`solveMultiRun(T &indiv, int _runs, YAML::Node config, bool display)  `
  + Runs several GA's based on T class and optional configuration or display.

`solveMultiThread(T &indiv, int _runs, int _n_threads, YAML::Node config, bool display)`  
  + Runs several GA's across several threads with optional configuration or display.

At the end of any of these functions, indiv is the best found solution.


## Yaml configuration

An example of YAML file configuration is shown in the tsp example. It consists in 4 integers:
- full_pop: size of the population considered in the GA.
- iter_max: the GA stops after this number of iterations.
- iter_out: the GA stops if the same individual is always the same during this number of iterations.
- keep_best: number of best individuals that will be automatically kept from one generation to another.

## Example and required interface 

The tsp project gives an example for the classical Traveling Salesman Problem.
A custom class describes an individual in TSP, that is a particular ordering of the cities. 

New problems can be instanciated as long as the custom class is defined with the following interface, assuming the class is called `Indiv`:
- `Indiv()`: initializes this solution with a default configuration. This function is called only when generating the initial population.
- `double cost`: the cost of this individual (should be public)
- `void computeCost()`: updates the cost of this individual. 
- `void randomize()`: modifies this individual to a random one and computes its cost.
- `void crossAndMutate(Indiv &parent1, Indiv &parent2)`: modifies this individual to a crossing between the two parents, add some mutation and computes its cost.
- `static void waitForCosts()`: static function that is called in the GA after updating the costs (randomizing population or performing several crossings and mutations. If the costs can be computed sequentially this function should just be `{}`. Costs may also be computed from multi-threading, in this case this function should ensure all costs have been computed (`joinAll()` of some sort).

Note that using the multi-threaded solver with a Individual class performing multi-treaded cost computation has not been tested.
