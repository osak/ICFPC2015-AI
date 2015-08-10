#pragma once

#include "../common.h"
#include "board_small.h"
#include "unit.h"

using namespace std;

class Game {
public:
	int H, W;
    double time;
	vector <vector <unsigned> > boardHash;
	Board board;
	// ユニット（流れてくる順）
	vector<Unit> units;
	vector<string> spells;
	vector<int> spellLens;
};

