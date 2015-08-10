#pragma once
#include "../common.h"
#include <cstring>

using namespace std;

typedef std::bitset<200> BitRow;

class Board {
public:
	int spellMask;
	int currentScore;
	int previousLine;
    int powerScore;
	int expectedScore;
	unsigned hash;
	string commands;
	vector<BitRow> field;

	bool operator<(const Board &b) const {
		if (currentScore + powerScore + expectedScore != b.currentScore + b.powerScore + b.expectedScore) return currentScore + powerScore + expectedScore < b.currentScore + b.powerScore + b.expectedScore;
		if (currentScore + powerScore != b.currentScore + b.powerScore) return currentScore + powerScore < b.currentScore + b.powerScore;
        if (currentScore != b.currentScore) return currentScore < b.currentScore;
		if (previousLine != b.previousLine) return previousLine < b.previousLine;
		return hash < b.hash;
	}
	bool operator>(const Board &b) const {
		return b < *this;
	}
};

