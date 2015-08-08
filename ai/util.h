#include "common.h"
#include "board.h"
#include "unit.h"

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
	static const string commandMove[4];
	static const string commandRotate[3];

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
	static Point get(Point &pivot, int x, int y) {
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
	static Point get(Point &pivot, int theta, Point &point) {
		int px, py;

		px = pxx[theta] * point.x + pxy[theta] * point.y;
		py = pyx[theta] * point.x + pyy[theta] * point.y;

		return get(pivot, px, py);
	}

	// validな配置かチェック
	static bool check(vector <BitRow> &field, Point &pivot, int theta, Unit &unit) {
		int i;

		for (i = 0; i < unit.member.size(); i++) {
			Point p = get(pivot, theta, unit.member[i]);
			if (p.x < 0 || p.x >= H || p.y < 0 || p.y >= W || field[p.x].get(p.y) == 1) return false;
		}

		return true;
	}

};

unsigned Util::x = 123456789;
unsigned Util::y = 362436039;
unsigned Util::z = 521288629;
unsigned Util::w = 88675123;

const int Util::pxx[6] = { 1, 1, 0, -1, -1, 0 };
const int Util::pxy[6] = { 0, 1, 1, 0, -1, -1 };
const int Util::pyx[6] = { 0, -1, -1, 0, 1, 1 };
const int Util::pyy[6] = { 1, 0, -1, -1, 0, 1 };
const int Util::dx[4] = { 0, 0, 1, 1 };
const int Util::dy[2][4] = { 1, -1, 0, -1, 1, -1, 1, 0 };
const int Util::rdx[4] = { 0, 0, -1, -1 };
const int Util::rdy[2][4] = { -1, 1, -1, 0, -1, 1, 0, 1 };
const string Util::commandMove[4] = { "E", "W", "D", "S" };
const string Util::commandRotate[3] = { "X", "", "C" };