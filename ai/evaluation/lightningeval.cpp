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
	return val / 10;
}

int LightningEval::messiahScore(const vector <BitRow> &f) {
	int val = 0;
	for (int y = 0; y < H; y++) {
		int n = f[y].count();
		val += n ? (n * n - W * W) * (H - y) * 4 / (y + 1) : 0;
	}
	return val / 10;
}

int LightningEval::calcRand(Board &board, Board &nextBoard, int num) {
	vector <BitRow> &field = nextBoard.field;
	if (num == units.size()) return 0;
	Unit &next = units[num];
	if (!Util::check(H, W, field, next.pivot, 0, next)) return -1e9;
	return Util::GetRandom();
}

int LightningEval::calcGod(Board &board, Board &nextBoard, int num){
	vector <BitRow> &field = nextBoard.field;
	if (num == units.size()) return 0;
	Unit &next = units[num];
	if (!Util::check(H, W, field, next.pivot, 0, next)) return -1e9;
	int ret = 0;
	for (int y = 0; y < H; y++) {
		int sum = 0;
		for (int x = 0; x < W; x++) {
			sum += (!field[y][x]) * min(x, W - x - 1);
		}
		int power = (!field[y][0] + !field[y][W - 1] + (field[y] ^ (field[y] >> 1)).count());
		int loc = (H - y)*(H - y) + sum;
		int special = W + (field[y] ^ (y == 0 ? 0 : field[y - 1])).count()
		+ (field[y] ^ (y == 0 ? 0 : (y & 1 ? field[y - 1] >> 1 : field[y - 1] << 1))).count();
		ret -= power * loc * special;
	}
	return ret;
}

int LightningEval::calcMaster(Board &board, Board &nextBoard, int num){
	vector <BitRow> &field = nextBoard.field;
	if (num == units.size()) return 0;
	Unit &next = units[num];
	if (!Util::check(H, W, field, next.pivot, 0, next)) return -1e9;
	if (maxUnitSize == 1) return oneUnitScore(field);
	return kawateaScore(field) + heightScore(field) + chanceScore(field, units.size() - num) + dangerScore(field);
}

int LightningEval::calcBuddha(Board &board, Board &nextBoard, int num){
	vector <BitRow> &field = nextBoard.field;
	if (num == units.size()) return 0;
	Unit &next = units[num];
	if (!Util::check(H, W, field, next.pivot, 0, next)) return -1e9;
	return heightScore(field) + buddhaScore(field) + messiahScore(field);
}

int LightningEval::calcHole(Board &board, Board &nextBoard, int num){
	int length = min(H, W) / 2 - 1;
	vector <BitRow> &field = nextBoard.field;
	if (nextBoard.currentScore - board.currentScore >= (length + 1) * (length + 2) * 50) {
		return 1<<30;
	}
	if (num == units.size()) return 0;
	Unit &next = units[num];
	if (!Util::check(H, W, field, next.pivot, 0, next)) return -1e9;
	int val = (nextBoard.currentScore - board.currentScore) / 100;
	int holeY = -1, holeX = -1;
	for (int y = H-1; y >= 0; y--) {
		if (field[y].count() == W-1) {
			holeY = y;
			for (int x = 0; x < W; x++) {
				if (!field[y][x]) {
					holeX = x;
					break;
				}
			}
			break;
		}
	}
	if (holeY != -1) {
		val++;
		int left = 0, right = 0, ok = 0;
		for (int yy = holeY-1; yy >= 0; yy--) {
			int i = holeY - yy;
			int xx = holeX-(i+!(holeY&1))/2;
			if (xx < 0) {
				break;
			}
			if (field[yy][xx]) {
				break;
			}
			left++;
		}
		for (int yy = holeY-1; yy >= 0; yy--) {
			int i = holeY - yy;
			int xx = holeX+(i+(holeY&1))/2;
			if (xx >= W) {
				break;
			}
			if (field[yy][xx]) {
				break;
			}
			right++;
		}
		for (int yy = holeY-1; yy >= 0 && ok < max(left, right); yy--) {
			if (field[yy].count() != W-1) {
				break;
			}
			ok++;
		}
		val += ok * max(left, right);
	}
	int cnt = 0;
	if (holeY != -1) {
		for (int y = holeY; y < H; y++) {
			cnt += field[y].count();
		}
	}
	for (int y = 0; y < length + 1; y++) {
		if (field[y].count()) {
			return -(1<<30);
		}
	}
	return (cnt + val) * H * W * H + buddhaScore(field) + heightScore(field);
}
