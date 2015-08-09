#include "lightningeval.h"
using namespace std;

const BitRow MASK = BitRow(0xffffffffffffffffULL);

// width must be smaller than 64
int LightningEval::kawateaScore(const vector <BitRow> &f) {
#ifdef _MSC_VER
	auto __builtin_popcountll = [](unsigned long long b) {
		int cnt = 0;
		while (b) ++cnt, b &= b - 1;
		return cnt;
	};
#endif
	unsigned long long a = (f[0] & MASK).to_ullong();
    int sum = 0;
	for (int x = 0; x < H - 1; ++x) {
		auto b = (f[x + 1] & MASK).to_ullong();
		if(x%2 == 0) sum += __builtin_popcountll(a & (a >> 1) & (~b));
		else sum += __builtin_popcountll(a & (a << 1) & (~b));
		a = b;
	}
	return sum * -5;
}


int LightningEval::heightScore(const vector <BitRow> &f) {
	int sum = 0;
	for (int x = 0; x < H; ++x) {
		sum += (H - x) * f[x].count();
	}
	return -sum;
}

int LightningEval::dangerScore(const vector <BitRow> &f) {
	int limit = H / 3;
	int sum = 0;
	for (int x = 0; x < limit; ++x) {
		sum += f[x].count() * (H - x);
	}
	return sum * -200;
}

int LightningEval::oneUnitScore(const vector <BitRow> &f) {
	int sum = 0;
	for (int x = 1; x < H; ++x) {
		int cnt = f[x].count();
		if (cnt) sum += W - cnt;
	}
	return -sum*sum * 10;

}

int LightningEval::chanceScore(const vector <BitRow> &f, int leftTurn) {
	int sum = 0;
	for (int x = 0; x < H; ++x) {
		if (f[x].count() == W - 1) sum += 150;
	}
	return sum * (leftTurn > 10 ? 1 : 0);
}

int LightningEval::buddhaScore(const vector <BitRow> &f) {
	int val = 0;
	for (int y = 0; y < H; y++) {
		int n = f[y].popcount();
		val += n ? n * n - W * W : 0;
		for (int x = 0; x < W; x++) {
			if (!f[y].get(x)) {
				int s = f[y].get(x) +
					(y==0 || x==0 || f[y-1].get(y&1?x-1:x)) +
					(y==0 || x==W-1 || f[y-1].get(y&1?x:x+1)) +
					(x==0 || f[y].get(x-1)) +
					(x==W-1 || f[y].get(x+1)) +
					(y==H-1 || x==0 || f[y+1].get(y&1?x-1:x)) +
					(y==H-1 || x==W-1 || f[y+1].get(y&1?x:x+1));
				val += x == 7 ? -x * x : 0;
			}
		}
	}
	return val;
}

int LightningEval::calc(vector <BitRow> &field, int num){
	if (num == units.size()) return 0;
	Unit &next = units[num];
	if (!Util::check(H, W, field, next.pivot, 0, next)) return -1e9;
	if (maxUnitSize == 1) return oneUnitScore(field);
	return kawateaScore(field) + heightScore(field) + chanceScore(field, units.size() - num) + dangerScore(field) + buddhaScore(field);
}

