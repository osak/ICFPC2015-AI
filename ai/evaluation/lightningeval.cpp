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

int LightningEval::calcGod(vector <BitRow> &field, int num){
#ifdef _MSC_VER
	auto __builtin_popcountll = [](unsigned long long b) {
		int cnt = 0;
		while (b) ++cnt, b &= b - 1;
		return cnt;
	};
#endif
	int ret = 0;
	for (int y = 0; y < H; y++) {
		int sum = 0;
		for (int x = 0; x < W; x++) {
			sum += (!field[y].get(x)) * min(x, W - x - 1);
		}
		//ret += (field[y].popcount() + (field[y].bits[0] ^ (field[y].bits[0]>>1))) * (y-H-sum) * (W + __builtin_popcountll(field[y].bits[0] ^ field[y+1].bits[0]));
		int power = (!field[y].get(0) + !field[y].get(W - 1) + __builtin_popcountll(field[y].bits[0] ^ (field[y].bits[0] >> 1)));
		int loc = (H - y)*(H - y) + sum;
		int special = W + __builtin_popcountll(field[y].bits[0] ^ (y == 0 ? 0 : field[y - 1].bits[0]));
		+__builtin_popcountll(field[y].bits[0] ^ (y == 0 ? 0 : (y & 1 ? field[y - 1].bits[0] >> 1 : field[y - 1].bits[0] << 1)));
		ret -= power * loc * special;
	}
	return ret / W;
}

int LightningEval::calcMaster(vector <BitRow> &field, int num){
	if (num == units.size()) return 0;
	Unit &next = units[num];
	if (!Util::check(H, W, field, next.pivot, 0, next)) return -1e9;
	if (maxUnitSize == 1) return oneUnitScore(field);
	return kawateaScore(field) + heightScore(field) + chanceScore(field, units.size() - num) + dangerScore(field);
}



