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
	return sum * -10;
}


int LightningEval::heightScore(const vector <BitRow> &f) {
	int sum = 0;
	for (int x = 0; x < H; ++x) {
		sum += (H - x) * (H - x) * f[x].popcount();
	}
	return -sum;
}

int LightningEval::dangerScore(const vector <BitRow> &f) {
	int limit = safeUnits ? H / 3 : H / 2;
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

int LightningEval::cornerScore(const vector <BitRow> &f) {
	if (W <= 3) return 0;
	int b = H - 1, r = W - 1;
	int cnt = 0;
	if (f[b].get(0) + f[b].get(1) + f[b - 1].get(0)) ++cnt;
	if (f[b].get(r) + f[b].get(r-1) + f[b - 1].get(r)) ++cnt;

	return cnt * 80;
}

int LightningEval::chanceScore(const vector <BitRow> &f, int leftTurn) {
	if (leftTurn <= 10 || bigWidth) return 0;

	int sum = 0;
	for (int x = H / 3; x < H; ++x) {
		if (f[x].popcount() == W - 1 && f[x].get(0) && f[x].get(W - 1)) sum += 150;
	}
	return sum * (safeUnits ? 1.2 : 0);
}

int LightningEval::calc(vector <BitRow> &field, int num){
	if (num == units.size()) return 0;
	Unit &next = units[num];
	if (!Util::check(H, W, field, next.pivot, 0, next)) return -1e9;
	if (maxUnitSize == 1) return oneUnitScore(field);
	return kawateaScore(field) + heightScore(field) + chanceScore(field, units.size() - num) + dangerScore(field);
}

