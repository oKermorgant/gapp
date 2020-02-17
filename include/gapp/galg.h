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


template<class T>
bool operator<(const T &i1, const T& i2)
{
  return i1.cost() < i2.cost();
}
namespace gapp
{

std::pair<uint, uint> different_randoms(std::uniform_int_distribution<> & dis, std::mt19937 &gen)
{
  const int n1 = dis(gen);
  int n2 = dis(gen);
  while(n1 == n2)
    n2 = dis(gen);
  return {n1,n2};
}

template <class T>
bool updateBest(T& old_best, const T&new_best)
{
  if(new_best < old_best)
  {
    old_best = new_best;
    return true;
  }
  return false;
}

uint readFrom(const YAML::Node &config, std::string key, uint default_val)
{
  if(!config.IsNull() && !config[key].IsNull())
    return  config[key].as<uint>();
  return default_val;

}

// perform a single run with a random population
template<class T> void solveSingleRun(T &best, const YAML::Node &config = YAML::Node())
{    
  const uint keep_best(readFrom(config, "keep_best", 5));
  const uint iter_max(readFrom(config, "iter_max", 100));
  const uint iter_out(readFrom(config, "iter_out", 30));
  const uint full_population(readFrom(config, "full_pop", 500));
  const auto half_population(full_population/2);

  // random number generators
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> full_rand(0, static_cast<int>(full_population)-1);
  static std::uniform_int_distribution<> half_rand(0, static_cast<int>(half_population)-1);

  // init first population from random individuals
  std::vector<T> population(full_population);
  for(auto &indiv: population)
    indiv.randomize();
  T::waitForCosts();

  std::nth_element(population.begin(), population.begin()+keep_best,
                   population.end());
  best = *std::min_element(population.begin(),
                           population.begin()+keep_best);

  // loop until exit conditions
  uint iter=0,iter_follow=0;
  std::vector<T> selected(full_population/2-keep_best);

  while(iter++ < iter_max && iter_follow< iter_out)   // max iteration and max iteration where the best is always the same
  {
    // selection, 1 vs 1 tournament to fill half of the population
    for(auto & indiv: selected)
    {
      const auto idx = different_randoms(full_rand, gen);
      indiv = std::min(population[idx.first], population[idx.second]);
    }

    // put new elements after elites
    std::copy(selected.begin(), selected.end(),
              population.begin()+keep_best);

    // crossing and mutation to fill other half of the new pop
    for(uint i=half_population;i<full_population;++i)
    {
      const auto idx = different_randoms(half_rand, gen);
      // cross between parents + compute cost
      population[i].crossAndMutate(population[idx.first],population[idx.second]);
    }
    T::waitForCosts();

    // re-sort from new costs
    std::nth_element(population.begin(), population.begin()+keep_best,
                     population.end());

    if(updateBest(best, *std::min_element(population.begin(),
                                          population.begin()+keep_best))
       )
      iter_follow = 0;
    else
      iter_follow++;
  }
}

// perform a given number of runs and returns the best one in _best
// not to be used directly
template<class T> void solveMultiRun(T &best, int _runs = 0, const YAML::Node &config = YAML::Node(), bool display = false, int _thread_n = 0)
{
  T indiv;
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

    if(run == 0)
      best = indiv;
    else
      updateBest(best, indiv);
  }
}



// performs a given number of runs across a given number of threads, returns the overall best result
template<class T> void solveMultiThread(T &best, int _runs = 10, int _n_threads = 1, const YAML::Node &config = YAML::Node(), bool display = false)
{
  if(_n_threads > _runs)
    _n_threads = _runs;
  std::vector<std::thread> threads;
  std::vector<T> bests(_n_threads);

  int div = _runs / _n_threads;
  std::cout << "runs per thread: " << div << std::endl;

  for(int i=0;i<_n_threads;++i)
    threads.push_back(std::thread(solveMultiRun<T>, std::ref(bests[i]), div, config, display, i+1));

  for(auto &t: threads)
    t.join();

  // compare results and return best individual
  best = *std::min_element(bests.begin(), bests.end());
}


}

#endif // GALG_H
