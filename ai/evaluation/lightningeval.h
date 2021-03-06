#pragma once

#include "../util.h"
#include "../common.h"
#include "../model/board.h"
#include "../model/unit.h"

using namespace std;

class LightningEval{
	int H, W, maxUnitSize;
	vector<Unit> units;

	int kawateaScore(const vector <BitRow> &f);
	int heightScore(const vector <BitRow> &f);
	int dangerScore(const vector <BitRow> &f);
	int oneUnitScore(const vector <BitRow> &f);
	int chanceScore(const vector <BitRow> &f, int leftTurn);
	int kadoScore(const vector <BitRow> &f);
public:

	LightningEval(int H, int W, vector<Unit> &units) : H(H), W(W), units(units){
		maxUnitSize = 0;
		for (auto &u : units) {
			maxUnitSize = max(maxUnitSize, (int)u.member.size());
		}
	}
	int calc(vector <BitRow> &field, int num);
};


