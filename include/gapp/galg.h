#ifndef GALG_H
#define GALG_H

#include <vector>
#include <thread>
#include <functional>
#include <chrono>
#include <algorithm>
#include <yaml-cpp/yaml.h>
#include <iostream>

namespace gapp
{

template<class T> bool compareCosts(const T &i1, const T& i2)
{return i1.cost < i2.cost;}

// perform a single run with a random population
template<class T> void solveSingleRun(T &best, const YAML::Node &config = YAML::Node())
{
    // read config parameters
    int keep_best = 5;
    int iter_max = 100;
    int iter_out = 30;
    int full_population = 500;

    if(!config.IsNull())
    {
        if(!config["keep_best"].IsNull())
            keep_best = config["keep_best"].as<double>();

        if(!config["iter_max"].IsNull())
            iter_max = config["iter_max"].as<double>();

        if(!config["iter_out"].IsNull())
            iter_out = config["iter_out"].as<double>();

        if(!config["full_pop"].IsNull())
            full_population = 2*((config["full_pop"].as<int>()+1)/2);;
    }
    const int half_population = full_population/2;

    // init first population from random individuals
    std::vector<T> population(full_population);
    for(auto &indiv: population)
        indiv.randomize();
    T::waitForCosts();

    population.resize(full_population + half_population);

    std::nth_element(population.begin(), population.begin()+keep_best,
                     population.begin()+full_population,
                     compareCosts<T>);
    best = *(std::min_element(population.begin(),
                              population.begin()+keep_best,
                              compareCosts<T>));

    // loop until exit conditions
    unsigned int i, iter=0,iter_follow=0;
    unsigned int n1,n2;
    while(iter++ < iter_max && iter_follow< iter_out)   // max iteration and max iteration where the best is always the same
    {
        // we keep the best individuals anyway
        for(i=0;i<keep_best;++i)
            population[full_population+i] = population[i];

        // selection, 1 vs 1 tournament to fill half of the population
        for(i=keep_best;i<half_population;++i)
        {
            n1 = rand() % full_population;
            n2 = rand() % full_population;
            while(n1 == n2)
                n2 = rand() % full_population;
            if(compareCosts(population[n1], population[n2]))
                population[full_population+i] = population[n1];
            else
                population[full_population+i] = population[n2];
        }

        // put new elements at front of new population
        std::swap_ranges(population.begin(),
                         population.begin() + half_population,
                         population.begin() + full_population);

        // crossing and mutation to fill other half of the new pop
        for(i=half_population;i<full_population;++i)
        {
            n1 = rand() % half_population;
            n2 = rand() % half_population;
            while(n1 == n2)
                n2 = rand() % half_population;
            //   std::cout << "  new " << i << " is crossing between " << n1 << " and " << n2 << std::endl;
            // cross between parents + compute cost
            population[i].crossAndMutate(population[n1],population[n2]);
        }
        T::waitForCosts();

        // re-sort from new costs
        std::nth_element(population.begin(), population.begin()+keep_best,
                         population.begin()+full_population,
                         compareCosts<T>);
        auto new_best = std::min_element(population.begin(),
                                        population.begin()+keep_best,
                                         compareCosts<T>);

        // check for best individual
        if(compareCosts(*new_best, best))
        {
            // found new best individual
            // reset counter
            iter_follow = 0;
            // update best
            best = *new_best;
        }
        else
            iter_follow += 1;   // always the same winner
    }
}




// perform a given number of runs and returns the best one in _best
// not to be used directly
template<class T> void solveMultiRun(T &best, int _runs = 0, const YAML::Node &config = YAML::Node(), bool display = false, int _thread_n = 0)
{
    T indiv;
    bool first = true;
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
        if(first || indiv.cost < best.cost)
        {
            best = indiv;
            first = false;
        }
    }
}



// performs a given number of runs across a given number fo threads, returns the overall best result
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
    best = *(std::min_element(bests.begin(), bests.end(), compareCosts<T>));
}


}

#endif // GALG_H