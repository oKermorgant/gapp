#include <travel.h>
#include <algorithm>
#include <iostream>
#include <string>

using namespace std;

inline unsigned int rand_int(const unsigned int &_start, const unsigned int &_length)
{
    return rand()%_length+_start;
}

// static variables
std::vector<std::vector<double> > Travel::nodes_ = std::vector<std::vector<double> >();
unsigned int Travel::n_ = 0;
bool Travel::closed_ = false;


// constructor with static init
Travel::Travel(const std::vector<std::vector<double> > &_nodes, bool _closed)
{
    nodes_ = _nodes;
    n_ = nodes_.size();
    ordering_.resize(n_);
    for(unsigned int i=0;i<n_;++i)
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
    cost = 0;
    for(unsigned int i=1;i<n_;++i)
        cost += nodes_[ordering_[i-1]][ordering_[i]];
    if(closed_)
        cost += nodes_[ordering_[n_-1]][ordering_[0]];
}

void Travel::crossAndMutate(const Travel &_father, const Travel &_mother)
{
    // crossing
    unsigned int n = rand_int(2, n_-3);
    unsigned int i;
    for(i=0;i<n;++i)
        ordering_[i] = _father.ordering_[i];

    // index of last element in mother
    int idx;
    for(idx = 0; idx < n_; idx++)
    {
        if(_mother.ordering_[idx] == ordering_[n-1])
            break;
    }

    // add other elements
    const int cut = n;
    int city;
    for(int i = idx + 1; i < idx + n_; ++i)
    {
        city = _mother.ordering_[i % n_];
        if(std::find(ordering_.begin(), ordering_.begin() + cut, city)
                == ordering_.begin() + cut)
            ordering_[n++] = city;

    }

    // mutation: switch 2 elements
    n = rand() % n_;
    unsigned int n2 = rand() % n_;
    while(n2 == n)
        n2 = rand() % n_;
    std::swap(ordering_[n], ordering_[n2]);

    computeCost();
}

void Travel::print(const YAML::Node &cities)
{
    cout << "cost: " << cost << endl;
    for(unsigned int i=0;i<ordering_.size()-1;++i)
        cout << cities[ordering_[i]] << " -> ";
    cout << cities[ordering_.back()];
    if(closed_)
        cout <<  " -> " << cities[ordering_.front()];
    cout << endl;
}
