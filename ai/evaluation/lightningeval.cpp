#include "lightningeval.h"
using namespace std;

// width must be smaller than 64
int LightningEval::kawateaScore(const vector <BitRow> &f) {
#ifdef _MSC_VER
	auto __builtin_popcountll = [](unsigned long long b) {
		int cnt = 0;
		while (b) ++cnt, b &= b - 1;
		return cnt;
	};
#endif
	int sum = 0, a = f[0].bits[0];
	for (int x = 0; x < H - 1; ++x) {
		auto b = f[x + 1].bits[0];
		if(x%2 == 0) sum += __builtin_popcountll(a & (a >> 1) & (~b));
		else sum += __builtin_popcountll(a & (a << 1) & (~b));
		a = b;
	}
	return sum * -5;
}


int LightningEval::heightScore(const vector <BitRow> &f) {
	int sum = 0;
	for (int x = 0; x < H; ++x) {
		sum += (H - x) * f[x].popcount();
	}
	return -sum;
}

int LightningEval::dangerScore(const vector <BitRow> &f) {
	int limit = H / 3;
	int sum = 0;
	for (int x = 0; x < limit; ++x) {
		sum += f[x].popcount() * (H - x);
	}
	return sum * -200;
}

int LightningEval::oneUnitScore(const vector <BitRow> &f) {
	int sum = 0;
	for (int x = 1; x < H; ++x) {
		int cnt = f[x].popcount();
		if (cnt) sum += W - cnt;
	}
	return -sum*sum * 10;

}

int LightningEval::chanceScore(const vector <BitRow> &f, int leftTurn) {
	int sum = 0;
	for (int x = 0; x < H; ++x) {
		if (f[x].popcount() == W - 1) sum += 150;
	}
	return sum * (leftTurn > 10 ? 1 : 0);
}

int LightningEval::calc(vector <BitRow> &field, int num){
	/*if (num == units.size()) return 0;
	Unit &next = units[num];
	if (!Util::check(H, W, field, next.pivot, 0, next)) return -1e9;
	if (maxUnitSize == 1) return oneUnitScore(field);
	return kawateaScore(field) + heightScore(field) + chanceScore(field, units.size() - num) + dangerScore(field);
	*/
	int val = 0;
	int searchSize = 4;
	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W; x++) {
			if (!field[y].get(x)) {
				int s = 0;
				for (int yy = y - 1; y - yy <= searchSize; yy--) {
					if (yy < 0) {
						s += 100 * (y - yy + 1) / (y - yy);
					} else {
						for (int xx = x - yy/2 - ((yy&1) && !(y&1)); xx <= x + yy/2 + ((yy&1) && (y&1)); xx++) {
							if (xx < 0 || xx >= W) {
								s += 100 / (y - yy);
							} else {
								s += 100 * field[yy].get(xx) / (y - yy);
							}
						}
					}
				}
				val -= s * field[y].popcount();
			}
		}
	}
	return val + heightScore(field) * W * H;
}
