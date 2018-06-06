#ifndef Travel_H
#define Travel_H

#include <vector>
#include <yaml-cpp/yaml.h>

class Travel
{
public:
    std::vector<unsigned int> ordering_;

    Travel()
    {
        ordering_.resize(n_);
        for(unsigned int i=0;i<n_;++i)
            ordering_[i] = i;
    }

    void randomize();

    Travel(const std::vector<std::vector<double> > &_nodes, bool _closed = false);

    void computeCost();
    void crossAndMutate(const Travel &_father, const Travel &_mother);
    void print(const YAML::Node& cities);

    inline static void waitForCosts() {}

    double cost;

protected:

    static std::vector<std::vector<double> > nodes_;    
    static unsigned int n_;
    static bool closed_;
};



#endif // Travel_H
