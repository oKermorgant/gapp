#include <iostream>
#include <gapp/galg.h>
#include <travel.h>
#include <yaml-cpp/yaml.h>
#include <map>
#include <chrono>

using namespace std;
using namespace gapp;

void printSummary(std::chrono::time_point<std::chrono::system_clock> start,
                  Travel t,
                  std::string legend,
                  const YAML::Node & cities)
{
    std::chrono::duration<double> elapsed_seconds =
            std::chrono::system_clock::now()-start;
    cout << legend << " in " << 1000*elapsed_seconds.count() << " ms" << std::endl;
    t.print(cities);
    cout << endl;
}

int main(int argc, char ** argv)
{
    std::srand(std::time(0));


    // load configuration for genetic algorithm
    YAML::Node config = YAML::LoadFile("../data/config.yaml");

    // load travel cost from YAML

    std::string path = "../data/tsp.yaml";
    if(argc > 1)
        path = std::string(argv[1]);
    YAML::Node data = YAML::LoadFile(path);
    YAML::Node cities = data["cities"];
    const unsigned int N = cities.size();
    std::vector<std::vector<double> > nodes(N);
    for(unsigned int i=0;i<N;++i)
    {
        nodes[i].resize(N);
        for(unsigned int j=0;j<N;++j)
            nodes[i][j] = data[i][j].as<double>();
    }

    Travel t(nodes, true);

    std::chrono::time_point<std::chrono::system_clock> start;
    start = std::chrono::system_clock::now();

    // single-run solver
    gapp::solveSingleRun(t, config);
    printSummary(start, t, "Single run solution",cities );


    // 50 runs without threading
    start = std::chrono::system_clock::now();
    gapp::solveMultiRun(t, 100, config, false);
    printSummary(start, t, "Multi run solution", cities);

    // 200 runs in 4 threads
    start = std::chrono::system_clock::now();
    gapp::solveMultiThread(t, 100, 4, config, false);
    printSummary(start, t, "Multi run x multi thread solution", cities);

    t.randomize();
    cout << "Random solution" << endl;
    t.print(cities);


    YAML::Node solution = data["solution"];
    if(!solution.IsNull() && argc > 1)
        cout << "Best known solution: " << solution << endl;
}