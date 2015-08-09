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

int LightningEval::kawateaScore(const vector <BitRow> &f) {
	int sum = 0;
	for (int x = 0; x < H - 1; ++x) {
		if (x % 2 == 0) {
			auto a = f[x].bits[0];
			auto b = f[x + 1].bits[0];
			sum += __builtin_popcountll(a & (a >> 1) & (~b));
		} else {
			auto a = f[x].bits[0];
			auto b = f[x + 1].bits[0];
			sum += __builtin_popcountll(a & (a << 1) & (~b));
		}
	}
	return sum * -5;
}

int LightningEval::calc(vector <BitRow> &field, int num){
	if (num == units.size()) return 0;
	Unit &next = units[num];
	if (!Util::check(H, W, field, next.pivot, 0, next)) return -1e9;
	if (maxUnitSize == 1) return oneUnitScore(field);
	return kawateaScore(field) + heightScore(field) + dangerScore(field) + chanceScore(field, units.size() - num);
}
