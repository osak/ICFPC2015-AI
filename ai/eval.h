#include "common.h"
#include "board.h"
#include "unit.h"

using namespace std;

extern int H, W;

struct TestEval{
    int holeScore(const vector <BitRow> &ff) {
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
                    nextPoint.x += dx[k];
                    nextPoint.y += dy[p.x % 2][k];
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
    
    int heightScore(const vector <BitRow> &f) {
        int sum = 0;
        for (int x = 0; x < H; ++x) {
            for (int y = 0; y < W; ++y) {
                if (f[x].get(y)) sum += H - x;
            }
        }
        return -sum;
    }

	int dangerScore(const vector <BitRow> &f) {
		int limit = H / 3;
		int sum = 0;
		for (int x = 0; x < limit; ++x) {
			for (int y = 0; y < W; ++y) {
				if (f[x].get(y)) sum += H - x;
			}
		}
		return sum * -200;
	}

	int oneUnitScore(const vector <BitRow> &f) {
		int sum = 0;
		for (int x = 1; x < H; ++x) {
			int cnt = f[x].popcount();
			if (cnt) sum += W - cnt;
		}
		return -sum*sum*10;

	}

	int chanceScore(const vector <BitRow> &f, int leftTurn) {
		int sum = 0;
		for (int x = 0; x < H; ++x) {
			int cnt = 0;
			for (int y = 0; y < W; ++y) {
				if (f[x].get(y)) ++cnt;
			}
			if (cnt == W - 1) sum += 150;
		}
		return sum * (leftTurn < 10 ? 1 : -1);
	}
    
    int calc(vector <BitRow> &field, int num, vector<Unit> &units, vector<int> &source){
        if (num == source.size()) return 0;
        
        if (!check(field, units[source[num]].pivot, 0, num)) return -1e9;
		if (maxUnitSize == 1) return oneUnitScore(field);
		return holeScore(field) + heightScore(field) + chanceScore(field, units.size() - num) + dangerScore(field);
    }
};

int calc(vector <BitRow> &field, int num) {
    TestEval e;
	int x = e.calc(field, num, units, source);
    return x;
}
