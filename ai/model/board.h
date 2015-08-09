#pragma once
#include "../common.h"

using namespace std;

class BitRow {
public:

	vector <unsigned long long> bits;

	BitRow(int size) : bits((size + 63) / 64) {
	}

	inline int get(int index) const {
		return (bits[index / 64] >> (index % 64)) & 1;
	}

	inline void set(int index) {
		bits[index / 64] |= (1ULL << (index % 64));
	}

	inline bool check(int width) const {
		int i;

		for (i = 0; i < width / 64; i++) {
			if (bits[i] != ~0ULL) return false;
		}

		if (i < bits.size()) {
			if (bits[i] != (1ULL << (width % 64)) - 1) return false;
		}

		return true;
	}

	inline void clear(void) {
		for (int i = 0; i < bits.size(); i++) bits[i] = 0;
	}

	inline int popcount() const {
		int cnt = 0;
		for (auto b : bits) {
			cnt += __builtin_popcountll(b);
		}
		return cnt;
	}
};

class Board {
public:

	int currentScore;
	int previousLine;
	int expectedScore;
	unsigned long long hash;
	string commands;
	vector <BitRow> field;

	bool operator<(const Board &b) const {
		if (currentScore + expectedScore != b.currentScore + b.expectedScore) return currentScore + expectedScore < b.currentScore + b.expectedScore;
		if (currentScore != b.currentScore) return currentScore < b.currentScore;
		if (previousLine != b.previousLine) return previousLine < b.previousLine;
		return hash < b.hash;
	}
	bool operator>(const Board &b) const {
		return b < *this;
	}
};

