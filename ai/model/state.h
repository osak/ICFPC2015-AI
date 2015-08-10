#pragma once

#include "../common.h"

using namespace std;

class State {
    public:
    
    int power;
    Point pivot;
    int theta;
    int bannedPivot;
    int bannedTheta;
    
    State() {}
    
    State(int power, Point &pivot, int theta, int bannedPivot, int bannedTheta) : power(power), pivot(pivot), theta(theta), bannedPivot(bannedPivot), bannedTheta(bannedTheta) {}
    
    bool operator<(const State &s) const {
        if (pivot.x != s.pivot.x) return pivot.x > s.pivot.x;
        if (power != s.power) return power < s.power;
        if (pivot.y != s.pivot.y) return pivot.y > s.pivot.y;
        if (theta != s.theta) return theta > s.theta;
        if (bannedPivot != s.bannedPivot) return bannedPivot > s.bannedPivot;
        return bannedTheta > s.bannedTheta;
    }
};