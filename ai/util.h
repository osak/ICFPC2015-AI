#pragma once

#include "common.h"
#include "model/board.h"
#include "model/unit.h"

using namespace std;

class Util{
	static unsigned x;
	static unsigned y;
	static unsigned z;
	static unsigned w;
public:

	static const int pxx[6];
	static const int pxy[6];
	static const int pyx[6];
	static const int pyy[6];
	static const int dx[4];
	static const int dy[2][4];
	static const int rdx[4];
	static const int rdy[2][4];
	static const char commandMove[4];
	static const char commandRotate[3];

	static unsigned GetRandom(void) {
		unsigned t;
		t = x ^ (x << 11);
		x = y;
		y = z;
		z = w;
		w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));

		return w;
	}

	// pivot からの相対座標に対する絶対座標を求める
	static Point get(const Point &pivot, int x, int y) {
		Point point;

		point.x = pivot.x + x;
		if (pivot.x % 2 == 0) {
			if (x > 0) {
				point.y = pivot.y + y + x / 2;
			}
			else {
				point.y = pivot.y + y + (x - 1) / 2;
			}
		}
		else {
			if (x > 0) {
				point.y = pivot.y + y + (x + 1) / 2;
			}
			else {
				point.y = pivot.y + y + x / 2;
			}
		}

		return point;
	}

	// pivot と回転角から point の絶対座標を求める
	static Point get(const Point &pivot, int theta, const Point &point) {
		int px, py;

		px = pxx[theta] * point.x + pxy[theta] * point.y;
		py = pyx[theta] * point.x + pyy[theta] * point.y;

		return get(pivot, px, py);
	}

	// validな配置かチェック
	static bool check(int H, int W, const vector <BitRow> &field, const Point &pivot, int theta, const Unit &unit) {
		int i;

		for (i = 0; i < unit.member.size(); i++) {
			Point p = get(pivot, theta, unit.member[i]);
			if (p.x < 0 || p.x >= H || p.y < 0 || p.y >= W || field[p.x].test(p.y)) return false;
		}

		return true;
	}

	static bool checkSpell(int H, int W, int turnNum, Board &board, Unit &unit, string spell, Point &pivot, int &theta) {
		int n = spell.size();
		pivot = unit.pivot;
		theta = 0;
		set<pair<Point, int> > visited;
		visited.insert(make_pair(pivot, theta));
		for (int i = 0; i < n; i++) {
			int spellIx = spell[i] - '0';
			if (spellIx < 4) {
				pivot.y += dy[pivot.x%2][spellIx];
				pivot.x += dx[spellIx];
			} else {
				theta = (theta + spellIx - 5 + turnNum) % turnNum;
			}
			if (!check(H, W, board.field, pivot, theta, unit)) {
				return false;
			}
			auto p = make_pair(pivot, theta);
			if (visited.count(p)) {
				return false;
			}
			visited.insert(p);
		}
		return true;
	}

};
