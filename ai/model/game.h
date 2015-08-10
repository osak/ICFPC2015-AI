#pragma once

#include "../common.h"
#include "board.h"
#include "unit.h"

using namespace std;

class Game {
public:
	int H, W;
	vector <vector <unsigned> > boardHash;
	Board board;
	// ユニット（流れてくる順）
	vector<Unit> units;
	vector<string> spells;
	vector<int> spellLens;
};

