#pragma once
#include "../common.h"
#include <cstring>

using namespace std;

class BitRow {
public:

    unsigned long long bits;

	BitRow() {
        bits = 0;
    }

	inline int test(int index) const {
        return (bits >> index) & 1;
	}

    inline void set(int index) {
        bits |= (1ULL << index);
	}

	inline bool check(int width) const {
        return count() == width;
	}

	inline void reset(void) {
        bits = 0;
	}

	inline int count() const {
		int cnt = 0;
        auto b = bits;
#ifdef _MSC_VER
		while (b) ++cnt, b &= b - 1;
#else
		cnt += __builtin_popcountll(b);
#endif
        
		return cnt;
	}
};

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

