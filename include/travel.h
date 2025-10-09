#ifndef Travel_H
#define Travel_H

#include <vector>
#include <yaml-cpp/yaml.h>

class Travel
{
public:
    std::vector<unsigned int> ordering_;

    Travel() : ordering_{std::vector<uint>(n_)} {}

    void randomize();

    Travel(const std::vector<std::vector<double> > &_nodes);

    void computeCost();
    void crossAndMutate(const Travel &_t1, const Travel &_t2);
    void print(const std::string &src, const YAML::Node& cities) const;

    inline double cost() const {return cost_;}

protected:

    inline static std::vector<std::vector<double> > nodes_{};
  inline static unsigned int n_{};
    double cost_ = 0;
};



#endif // Travel_H
