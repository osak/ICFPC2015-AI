#include "lightningeval.h"
using namespace std;

int LightningEval::holeScore(const vector <BitRow> &ff) {
	auto f = ff;
	Point st;
	st.x = 0, st.y = W / 2; // ŽG

	int distSum = 0;
	if (!f[st.x].get(st.y)) {
		queue<Point> q;
		f[st.x].set(st.y);
		q.push(st);
		while (!q.empty()){
			auto &p = q.front();
			q.pop();
			for (int k = 0; k < 4; ++k) {
				auto nextPoint = p;
				nextPoint.x += Util::dx[k];
				nextPoint.y += Util::dy[p.x % 2][k];
				if (nextPoint.x < 0 || nextPoint.x >= H || nextPoint.y < 0 || nextPoint.y >= W || f[nextPoint.x].get(nextPoint.y)) continue;
				f[nextPoint.x].set(nextPoint.y);
				q.push(nextPoint);
			}
		}
	}
	int holeCnt = 0;
	for (auto &v : f) holeCnt += W - v.popcount();
	return holeCnt * -5;
}

int LightningEval::heightScore(const vector <BitRow> &f) {
	int sum = 0;
	for (int x = 0; x < H; ++x) {
		for (int y = 0; y < W; ++y) {
			if (f[x].get(y)) sum += H - x;
		}
	}
	return -sum;
}

int LightningEval::dangerScore(const vector <BitRow> &f) {
	int limit = H / 3;
	int sum = 0;
	for (int x = 0; x < limit; ++x) {
		for (int y = 0; y < W; ++y) {
			if (f[x].get(y)) sum += H - x;
		}
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
		int cnt = 0;
		for (int y = 0; y < W; ++y) {
			if (f[x].get(y)) ++cnt;
		}
		if (cnt == W - 1) sum += 150;
	}
	return sum * (leftTurn > 10 ? 1 : 0);
}

int LightningEval::calc(vector <BitRow> &field, int num){
	int ret = 0;
	for (int y = 0; y < H; y++) {
		int sum = 0;
		for (int x = 0; x < W; x++) {
			sum += (!field[y].get(x)) * min(x, W-x-1);
		}
		//ret += (field[y].popcount() + (field[y].bits[0] ^ (field[y].bits[0]>>1))) * (y-H-sum) * (W + __builtin_popcountll(field[y].bits[0] ^ field[y+1].bits[0]));
		int power = (!field[y].get(0) + !field[y].get(W-1) + __builtin_popcountll(field[y].bits[0] ^ (field[y].bits[0]>>1)));
		int loc = (H-y)*(H-y)+sum;
		int special = W + __builtin_popcountll(field[y].bits[0] ^ (y == 0 ? 0 : field[y-1].bits[0]));
			+ __builtin_popcountll(field[y].bits[0] ^ (y == 0 ? 0 : (y&1 ? field[y-1].bits[0] >> 1 : field[y-1].bits[0] << 1)));
		ret -= power * loc * special;
	}
	return ret/W;
	/*
	if (num == units.size()) return 0;
	Unit &next = units[num];
	if (!Util::check(H, W, field, next.pivot, 0, next)) return -1e9;
	if (maxUnitSize == 1) return oneUnitScore(field);
	return holeScore(field) + heightScore(field) + chanceScore(field, units.size() - num) + dangerScore(field);
	*/
}

