#include <travel.h>
#include <algorithm>
#include <iostream>
#include <random>

using namespace std;



inline uint rand_int( uint start, uint end)
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return std::uniform_int_distribution<uint>(start, end)(gen);
}

// constructor with static init
Travel::Travel(const std::vector<std::vector<double> > &_nodes)
{
    nodes_ = _nodes;
    n_ = nodes_.size();
    ordering_.resize(n_);
    std::iota(ordering_.begin(), ordering_.end(), 0);
}

// randomize
void Travel::randomize()
{  
    std::iota(ordering_.begin(), ordering_.end(), 0);
    std::shuffle(ordering_.begin()+1,ordering_.end(), std::default_random_engine());
    computeCost();
}

void Travel::computeCost()
{
    cost_ = nodes_[ordering_[n_-1]][ordering_[0]];
    for(uint i=1;i<n_;++i)
        cost_ += nodes_[ordering_[i-1]][ordering_[i]];
}

void Travel::crossAndMutate(const Travel &_t1, const Travel &_t2)
{
    // crossing
    uint n = rand_int(3, n_-2);
    for(uint i=0;i<n;++i)
        ordering_[i] = _t1.ordering_[i];

    // index of last element in mother
    uint idx;
    for(idx = 0; idx < n_; idx++)
    {
        if(_t2.ordering_[idx] == ordering_[n-1])
            break;
    }

    // add other elements
    const uint cut = n;
    for(uint i = idx + 1; i < idx + n_; ++i)
    {
        const auto city = _t2.ordering_[i % n_];
        if(std::find(ordering_.begin(), ordering_.begin() + cut, city)
                == ordering_.begin() + cut)
            ordering_[n++] = city;
    }

    // mutation: switch 2 elements
    n = rand_int(1, n_-1);
    uint n2 = rand_int(1, n_-2);
    if(n2 == n)
      n2++;
    std::swap(ordering_[n], ordering_[n2]);

    computeCost();
}

void Travel::print(const string& src, const YAML::Node &cities) const
{
  cout << "cost: " << cost_ << endl;
  for(auto city: ordering_)
    cout << cities[city] << " -> ";
        cout << cities[ordering_.front()];
    cout << endl;

    std::stringstream ss;
    ss << "python3 ../data/show_travel.py " << src;
    for(auto i: ordering_)
      ss << " " << i;
    ss << " &";
    [[maybe_unused]] auto ignored(system(ss.str().c_str()));
}
