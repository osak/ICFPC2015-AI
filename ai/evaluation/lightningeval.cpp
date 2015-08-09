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
	return sum * -10;
}


int LightningEval::heightScore(const vector <BitRow> &f) {
	int sum = 0;
	for (int x = 0; x < H; ++x) {
		sum += (H - x) * (H - x) * f[x].count();
	}
	return -sum;
}

int LightningEval::dangerScore(const vector <BitRow> &f) {
	int limit = safeUnits ? H / 3 : H / 2;
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

int LightningEval::cornerScore(const vector <BitRow> &f) {
	if (W <= 3) return 0;
	int b = H - 1, r = W - 1;
	int cnt = 0;
	if (f[b][0] + f[b][1] + f[b - 1][0]) ++cnt;
	if (f[b][r] + f[b][r-1] + f[b - 1][r]) ++cnt;

	return cnt * 80;
}

int LightningEval::chanceScore(const vector <BitRow> &f, int leftTurn) {
	if (leftTurn <= 10 || bigWidth) return 0;

	int sum = 0;
	for (int x = H / 3; x < H; ++x) {
		if (f[x].count() == W - 1 && f[x][0] && f[x][W - 1]) sum += 150;
	}
	return sum * (safeUnits ? 1.2 : 0);
}

int LightningEval::buddhaScore(const vector <BitRow> &f){
	int val = 0;
	for (int y = 0; y < H; y++) {
		int n = f[y].count();
		val += n ? n * n - W * W : 0;
		for (int x = 0; x < W; x++) {
			if (!f[y][x]) {
				int s =
					(y==0 || x==0 || f[y-1][y&1?x-1:x]) * 2 +
					(y==0 || x==W-1 || f[y-1][y&1?x:x+1]) * 2 +
					(x==0 || f[y][x-1]) +
					(x==W-1 || f[y][x+1]) +
					(y==H-1 || x==0 || f[y+1][y&1?x-1:x]) +
					(y==H-1 || x==W-1 || f[y+1][y&1?x:x+1]);
				val += -s * s;
			}
		}
	}
	return val/10;
}

int LightningEval::calcRand(vector <BitRow> &f, int num) {
	return Util::GetRandom();
}

/*
int LightningEval::calcGod(vector <BitRow> &field, int num){
	if (num == units.size()) return 0;
	Unit &next = units[num];
	if (!Util::check(H, W, field, next.pivot, 0, next)) return -1e9;
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
			sum += (!field[y][x]) * min(x, W - x - 1);
		}
		//ret += (field[y].popcount() + (field[y].bits[0] ^ (field[y].bits[0]>>1))) * (y-H-sum) * (W + __builtin_popcountll(field[y].bits[0] ^ field[y+1].bits[0]));
		int power = (!field[y][0] + !field[y][W - 1] + __builtin_popcountll(field[y].bits[0] ^ (field[y].bits[0] >> 1)));
		int loc = (H - y)*(H - y) + sum;
		int special = W + __builtin_popcountll(field[y].bits[0] ^ (y == 0 ? 0 : field[y - 1].bits[0]));
		+__builtin_popcountll(field[y].bits[0] ^ (y == 0 ? 0 : (y & 1 ? field[y - 1].bits[0] >> 1 : field[y - 1].bits[0] << 1)));
		ret -= power * loc * special;
	}
	return ret / W;
}
*/

int LightningEval::calcMaster(vector <BitRow> &field, int num){
	if (num == units.size()) return 0;
	Unit &next = units[num];
	if (!Util::check(H, W, field, next.pivot, 0, next)) return -1e9;
	if (maxUnitSize == 1) return oneUnitScore(field);
	return kawateaScore(field) + heightScore(field) + chanceScore(field, units.size() - num) + dangerScore(field);
}

int LightningEval::calcBuddha(vector <BitRow> &field, int num){
	return heightScore(field) + buddhaScore(field);
}

