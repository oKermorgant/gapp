#include <travel.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <random>

using namespace std;

inline uint rand_int( int start, uint end)
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(start, static_cast<int>(end));
  return static_cast<uint>(dis(gen));
}

// static variables
std::vector<std::vector<double> > Travel::nodes_ = std::vector<std::vector<double> >();
uint Travel::n_ = 0;
bool Travel::closed_ = false;


// constructor with static init
Travel::Travel(const std::vector<std::vector<double> > &_nodes, bool _closed)
{
    nodes_ = _nodes;
    n_ = nodes_.size();
    ordering_.resize(n_);
    for(uint i=0;i<n_;++i)
        ordering_[i] = i;
    closed_ = _closed;
}

// randomize
void Travel::randomize()
{
    std::random_shuffle(ordering_.begin(),ordering_.end());
    computeCost();
}

void Travel::computeCost()
{
    cost_ = 0;
    for(uint i=1;i<n_;++i)
        cost_ += nodes_[ordering_[i-1]][ordering_[i]];
    if(closed_)
        cost_ += nodes_[ordering_[n_-1]][ordering_[0]];
}

void Travel::crossAndMutate(const Travel &_father, const Travel &_mother)
{
    // crossing
    uint n = rand_int(2, n_-1);
    for(uint i=0;i<n;++i)
        ordering_[i] = _father.ordering_[i];

    // index of last element in mother
    uint idx;
    for(idx = 0; idx < n_; idx++)
    {
        if(_mother.ordering_[idx] == ordering_[n-1])
            break;
    }

    // add other elements
    const uint cut = n;
    uint city;
    for(uint i = idx + 1; i < idx + n_; ++i)
    {
        city = _mother.ordering_[i % n_];
        if(std::find(ordering_.begin(), ordering_.begin() + cut, city)
                == ordering_.begin() + cut)
            ordering_[n++] = city;
    }

    // mutation: switch 2 elements
    n = rand_int(0, n_-1);
    uint n2 = rand_int(0, n_-1);
    while(n2 == n)
        n2 = rand_int(0, n_-1);
    std::swap(ordering_[n], ordering_[n2]);

    computeCost();
}

void Travel::print(const YAML::Node &cities)
{
    cout << "cost: " << cost_ << endl;
    for(uint i=0;i<ordering_.size()-1;++i)
        cout << cities[ordering_[i]] << " -> ";
    cout << cities[ordering_.back()];
    if(closed_)
        cout <<  " -> " << cities[ordering_.front()];
    cout << endl;
}
