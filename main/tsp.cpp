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
                  const std::string &legend,
                  const YAML::Node & cities)
{
  std::chrono::duration<double> elapsed_seconds =
      std::chrono::system_clock::now()-start;
  cout << '\n' << legend << " in " << 1000*elapsed_seconds.count() << " ms" << std::endl; 
  t.print(cities);
  cout << endl;
}

int main(int argc, char ** argv)
{
  // load configuration for genetic algorithm
  const std::string tsp_dir{TSP_DIR};
  YAML::Node config = YAML::LoadFile(tsp_dir + "config.yaml");

  // load travel cost from YAML
  const auto path = tsp_dir + (argc > 1 ? argv[1] : "tsp.yaml");
  YAML::Node data = YAML::LoadFile(path);
  YAML::Node cities = data["cities"];
  const auto N = cities.size();
  std::vector<std::vector<double> > nodes(N);
  for(uint i=0;i<N;++i)
  {
    nodes[i].resize(N);
    for(uint j=0;j<N;++j)
      nodes[i][j] = data[i][j].as<double>();
  }

  Travel t(nodes, true);

  std::chrono::time_point<std::chrono::system_clock> start;

  t.randomize();
  cout << "Random solution" << endl;
  t.print(cities);

  // single-run solver
  start = std::chrono::system_clock::now();
  gapp::solveSingleRun(t, config);
  printSummary(start, t, "Single run solution",cities );

  // 100 runs without threading
  start = std::chrono::system_clock::now();
  gapp::solveMultiRun(t, 100, config, false);
  printSummary(start, t, "Multi run solution", cities);

  // 100 runs in 4 threads
  start = std::chrono::system_clock::now();
  gapp::solveMultiThread(t, 100, 4, config, false);
  printSummary(start, t, "Multi run x multi thread solution", cities);

  // display map just for fun
  std::stringstream ss;
  ss << "python ../data/show_travel.py " << path;
  for(auto i: t.ordering_)
    ss << " " << i;
  auto ignored(system(ss.str().c_str()));
}
