#ifndef GALG_H
#define GALG_H

#include <vector>
#include <thread>
#include <functional>
#include <chrono>
#include <algorithm>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <random>
#include <concepts>
#include <execution>

template <typename T>
concept Individual = requires(T candidate)
{
  {candidate.randomize()};
  {candidate.crossAndMutate(std::declval<const T&>(), std::declval<const T&>())};
  {candidate.cost()} -> std::convertible_to<double>;
};

template<Individual T>
bool operator<(const T &i1, const T& i2)
{
  return i1.cost() < i2.cost();
}

namespace gapp
{

std::pair<uint, uint> different_randoms(uint n)
{
  static std::random_device rd;
  static std::default_random_engine engine(rd());

  const auto n1{std::uniform_int_distribution<uint>(0,n-1)(engine)};
  const auto n2{std::uniform_int_distribution<uint>(0,n-2)(engine)};

  if(n1 == n2)
    return {n1, n2+1};
  return {n1, n2};
}

uint readFrom(const YAML::Node &config, std::string key, uint default_val)
{
  if(!config.IsNull() && !config[key].IsNull())
    return  config[key].as<uint>();
  return default_val;
}

template <class Candidate>
inline Candidate bestTopN(std::vector<Candidate> &population,
                uint keep_best,
                uint pop_size)
{
  std::nth_element(std::execution::par,
                   population.begin(), population.begin()+keep_best,
                   population.begin() + pop_size);
  return *std::min_element(population.begin(),
                           population.begin()+keep_best);
}

// perform a single run with a random population
template <Individual Candidate>
void solveSingleRun(Candidate &best, const YAML::Node &config = YAML::Node())
{    
  const uint keep_best(readFrom(config, "keep_best", 5));
  const uint max_iter(readFrom(config, "iter_max", 100));
  const uint max_same(readFrom(config, "iter_out", 30));
  const uint full_population(readFrom(config, "full_pop", 500));
  const auto half_population(full_population/2);

  // init first population from random individuals
  std::vector<Candidate> population(full_population + half_population-keep_best);

  std::for_each(std::execution::par,
                population.begin(), population.begin()+full_population,
                std::mem_fn(&Candidate::randomize));

  best = bestTopN(population, keep_best, full_population);
  auto best_cost = best.cost();

  // loop until exit conditions
  uint iter=0,iter_same=0;

  while(iter++ < max_iter && iter_same< max_same)   // max iteration and max iteration where the best is always the same
  {
    // selection, 1 vs 1 tournament to fill half of the population
    std::for_each(std::execution::par, population.begin()+full_population, population.end(),
                  [&full_population, &population](Candidate &c)
                  {const auto [n1,n2] = different_randoms(full_population);
                   c = std::min(population[n1], population[n2]);});


    // put new elements after elites
    std::copy(population.begin()+full_population, population.end(),
              population.begin()+keep_best);

    // crossing and mutation to fill other half of the new pop
    std::for_each(std::execution::par,
                  population.begin()+half_population, population.begin()+full_population,
                  [&half_population, &population](Candidate &c)
                  {
                    const auto [n1,n2] = different_randoms(half_population); {}
                    // cross between parents + compute cost
                    c.crossAndMutate(population[n1],population[n2]);
                  });

    // re-sort from new costs
    best = bestTopN(population, keep_best, full_population);

    if(best.cost() == best_cost)
      iter_same++;
    else
    {
      iter_same = 0;
      best_cost = best.cost();
    }
  }
}

// perform a given number of runs and returns the best one in _best
// not to be used directly
template<Individual Candidate>
void solveMultiRun(Candidate &best, int _runs = 0, const YAML::Node &config = YAML::Node(), bool display = false, int _thread_n = 0)
{
  Candidate indiv;
  std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(100*_thread_n));

  int base_run = (_thread_n-1) * _runs+1;
  for(int run=0;run<_runs;++run)
  {
    if(display)
    {
      if(_thread_n)
        std::cout << "Run #" << base_run + run << " in thread #" << _thread_n << std::endl;
      else
        std::cout << "Run #" << run+1 << std::endl;
    }

    solveSingleRun(indiv, config);

    if(run == 0 || (indiv.cost() < best.cost()))
      best = indiv;
  }
}



// performs a given number of runs across a given number of threads, returns the overall best result
template<Individual Candidate>
void solveMultiThread(Candidate &best, int _runs = 10, int _n_threads = 1, const YAML::Node &config = YAML::Node(), bool display = false)
{
  if(_n_threads > _runs)
    _n_threads = _runs;
  std::vector<std::thread> threads;
  std::vector<Candidate> bests(_n_threads);

  int div = _runs / _n_threads;
  std::cout << "runs per thread: " << div << std::endl;

  for(int i=0;i<_n_threads;++i)
    threads.push_back(std::thread(solveMultiRun<Candidate>, std::ref(bests[i]), div, config, display, i+1));

  for(auto &t: threads)
    t.join();

  // compare results and return best individual
  best = *std::min_element(bests.begin(), bests.end());
}


}

#endif // GALG_H
