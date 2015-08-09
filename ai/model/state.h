#pragma once

#include "../common.h"

using namespace std;

class State {
    public:
    
    int power;
    Point pivot;
    int theta;
    
    State(int power, Point &pivot, int theta) : power(power), pivot(pivot), theta(theta) {}
    
    bool operator<(const State &s) const {
        if (power != s.power) return power < s.power;
        if (pivot != s.pivot) return pivot < s.pivot;
        return theta < s.theta;
    }
};