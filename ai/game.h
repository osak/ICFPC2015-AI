#pragma once
#include "common.h"
#include "board.h"
#include "unit.h"

using namespace std;

class Game {
public:
	int H, W;
	vector <vector <unsigned long long> > boardHash;
	Board board;
	vector<Unit> units;
};

