#include <iostream>
#include <gapp/galg.h>
#include <travel.h>
#include <yaml-cpp/yaml.h>

using namespace std;
using namespace gapp;


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

  Travel t(nodes);

  // actual GA
  gapp::solveSingleRun(t, config);
  t.print(path, cities);

  // brute force
  config["iter_max"] = 2;
  config["full_pop"] = 500000;
  gapp::solveSingleRun(t, config);
  t.print(path, cities);


}
