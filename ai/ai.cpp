#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <algorithm>

using namespace std;

const int beamWidth = 10;

class BitRow {
    public:
    
    vector <unsigned long long> bits;
    
    BitRow(int size) : bits((size + 63) / 64) {
    }
    
    inline int get(int index) const {
        return (bits[index / 64] >> (index % 64)) & 1;
    }
    
    inline void set(int index) {
        bits[index / 64] |= (1ULL << (index % 64));
    }
    
    inline bool check(int width) const {
        int i;
        
        for (i = 0; i < width / 64; i++) {
            if (bits[i] != ~0ULL) return false;
        }
        
        if (i < bits.size()) {
            if (bits[i] != (1ULL << (width % 64)) - 1) return false;
        }
        
        return true;
    }
    
    inline void clear(void) {
        for (int i = 0; i < bits.size(); i++) bits[i] = 0;
    }
    
    inline int popcount() {
        int cnt = 0;
        for (auto &b : bits) {
            #ifdef _MSC_VER
            while (b) ++cnt, b &= b - 1;
            #else
            cnt += __builtin_popcountll(b);
            #endif
        }
        return cnt;
    }
};

class Board {
    public:
    
    int currentScore;
    int previousLine;
    int expectedScore;
    unsigned long long hash;
    string commands;
    vector <BitRow> field;
    
    bool operator<(const Board &b) const {
        if (currentScore + expectedScore != b.currentScore + b.expectedScore) return currentScore + expectedScore < b.currentScore + b.expectedScore;
        if (currentScore != b.currentScore) return currentScore < b.currentScore;
        if (previousLine != b.previousLine) return previousLine < b.previousLine;
        return hash < b.hash;
    }
};

class Point {
    public:
    
    int x;
    int y;
    
    bool operator<(const Point &p) const {
        if (x != p.x) return x < p.x;
        return y < p.y;
    }
};

class Unit {
    public:
    
    Point pivot;
    vector <Point> member;
};

int H, W;
int pxx[6] = {1, 1, 0, -1, -1, 0};
int pxy[6] = {0, 1, 1, 0, -1, -1};
int pyx[6] = {0, -1, -1, 0, 1, 1};
int pyy[6] = {1, 0, -1, -1, 0, 1};
int dx[4] = {0, 0, 1, 1};
int dy[2][4] = {1, -1, 0, -1, 1, -1, 1, 0};
int rdx[4] = {0, 0, -1, -1};
int rdy[2][4] = {-1, 1, -1, 0, -1, 1, 0, 1};
string commandMove[4] = {"E", "W", "D", "S"};
string commandRotate[3] = {"X", "", "C"};
vector <Unit> units;
vector <int> source;
vector <vector <unsigned long long> > boardHash;

unsigned GetRandom(void) {
    static unsigned x = 123456789;
    static unsigned y = 362436039;
    static unsigned z = 521288629;
    static unsigned w = 88675123;
    unsigned t;
    
    t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
    
    return w;
}

// pivot からの相対座標に対する絶対座標を求める
Point get(Point &pivot, int x, int y) {
    Point point;
    
    point.x = pivot.x + x;
    if (pivot.x % 2 == 0) {
        if (x > 0) {
            point.y = pivot.y + y + x / 2;
        } else {
            point.y = pivot.y + y + (x - 1) / 2;
        }
    } else {
        if (x > 0) {
            point.y = pivot.y + y + (x + 1) / 2;
        } else {
            point.y = pivot.y + y + x / 2;
        }
    }
    
    return point;
}

// 各 unit を pivot からの相対座標に直す
void init(Unit &unit) {
    int topx = 1e9, lefty = 1e9, righty = 1e9, i;
    
    for (i = 0; i < unit.member.size(); i++) {
        int x = unit.member[i].x - unit.pivot.x;
        
        if (unit.pivot.x % 2 == 0) {
            if (x > 0) {
                unit.member[i].y = unit.member[i].y - unit.pivot.y - x / 2;
            } else {
                unit.member[i].y = unit.member[i].y - unit.pivot.y - (x - 1) / 2;
            }
        } else {
            if (x > 0) {
                unit.member[i].y = unit.member[i].y - unit.pivot.y - (x + 1) / 2;
            } else {
                unit.member[i].y = unit.member[i].y - unit.pivot.y - x / 2;
            }
        }
        
        topx = min(topx, unit.member[i].x);
        unit.member[i].x -= unit.pivot.x;
    }
    
    unit.pivot.x -= topx;
    
    for (i = 0; i < unit.member.size(); i++) {
        Point point = get(unit.pivot, unit.member[i].x, unit.member[i].y);
        
        lefty = min(lefty, point.y);
        righty = min(righty, W - point.y - 1);
    }
    
    if (lefty >= righty) {
        unit.pivot.y -= (lefty - righty + 1) / 2;
    } else {
        unit.pivot.y += (righty - lefty) / 2;
    }
}

// pivot と回転角から point の絶対座標を求める
Point get(Point &pivot, int theta, Point &point) {
    int px, py;
    
    px = pxx[theta] * point.x + pxy[theta] * point.y;
    py = pyx[theta] * point.x + pyy[theta] * point.y;
    
    return get(pivot, px, py);
}

bool check(vector <BitRow> &field, Point &pivot, int theta, int num) {
    int i;
    
    for (i = 0; i < units[source[num]].member.size(); i++) {
        Point p = get(pivot, theta, units[source[num]].member[i]);
        if (p.x < 0 || p.x >= H || p.y < 0 || p.y >= W || field[p.x].get(p.y) == 1) return false;
    }
    
    return true;
}

struct TestEval{
    int holeScore(const vector <BitRow> &ff, const int num) {
        auto f = ff;
        Point st;
        st.x = 0, st.y = W / 2; // 雑
        
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
		int sum = 0;
		for (int x = 0; x < H / 3; ++x) {
			for (int y = 0; y < W; ++y) {
				if (f[x].get(y)) sum += H - x;
			}
		}
		return sum * -200;
	}

	int chanceScore(const vector <BitRow> &f) {
		int sum = 0;
		for (int x = 0; x < H; ++x) {
			int cnt = 0;
			for (int y = 0; y < W; ++y) {
				if (f[x].get(y)) ++cnt;
			}
			if (cnt == W - 1) sum += 150;
		}
		return sum;
	}
    
    int chainScore(const vector <BitRow> &f) {
		return 0;
        vector<int> vy(H, -1);
        for (int x = 0; x < H; ++x) {
            int cnt = 0;
            for (int y = 0; y < W; ++y) {
                if (f[x].get(y)) ++cnt;
                else if (vy[x] == -1) vy[x] = y;
                else vy[x] = -2;
            }
        }
        int sum = 0;
        for (int x = 0; x < H - 1; ++x) {
            if (vy[x] >= 0 && (vy[x] == vy[x + 1] + (x % 2 ? 1 : -1) || vy[x] == vy[x + 1])) sum += 50;
        }
        return sum;
    }
    
    int calc(vector <BitRow> &field, int num, vector<Unit> &units, vector<int> &source){
        if (num == source.size()) return 0;
        
        if (!check(field, units[source[num]].pivot, 0, num)) return -1e9;
		return holeScore(field, num) + heightScore(field) + chanceScore(field) + dangerScore(field);
    }
};

int calc(vector <BitRow> &field, int num) {
    TestEval e;
    return e.calc(field, num, units, source);
}

void update(Board &board, Point &pivot, int theta, int num) {
    int count = 0, point, i, j;
    
    for (i = 0; i < units[source[num]].member.size(); i++) {
        Point p = get(pivot, theta, units[source[num]].member[i]);
        
        board.field[p.x].set(p.y);
        board.hash ^= boardHash[p.x][p.y];
    }
    
    for (i = H - 1; i >= 0; i--) {
        if (board.field[i].check(W)) {
            count++;
        } else {
            board.field[i + count] = board.field[i];
        }
    }
    
    for (i = 0; i < count; i++) board.field[i].clear();
    
    if (count > 0) {
        board.hash = 0;
        
        for (i = count; i < H; i++) {
            for (j = 0; j < W; j++) {
                if (board.field[i].get(j)) board.hash ^= boardHash[i][j];
            }
        }
    }
    
    point = units[source[num]].member.size() + 100 * (1 + count) * count / 2;
    board.currentScore += point;
    if (board.previousLine > 1) board.currentScore += (board.previousLine - 1) * point / 10;
    board.previousLine = count;
}

void debug(Board &board) {
    return;
    fprintf(stderr, "%d %d\n", board.currentScore, board.expectedScore);
    fprintf(stderr, "%s\n", board.commands.c_str());
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            fprintf(stderr, "%d", board.field[i].get(j));
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
}

int main()
{
#ifdef _MSC_VER
	freopen("../../ICFPC2015/cpp_input/problem_23_0.txt", "r", stdin);
#endif

    int unitCount, fieldCount, sourceLength, maxScore = -1, i, j, k;
    string ans = "";
    Board initBoard;
    priority_queue <Board> que, queNext;
    
    scanf("%d %d", &H, &W);
    scanf("%d", &unitCount);
    
    for (i = 0; i < unitCount; i++) {
        int count;
        Unit unit;
        
        scanf("%d %d %d", &unit.pivot.y, &unit.pivot.x, &count);
        
        for (j = 0; j < count; j++) {
            Point point;
            
            scanf("%d %d", &point.y, &point.x);
            
            unit.member.push_back(point);
        }
        
        init(unit);
        
        units.push_back(unit);
    }
    
    vector <BitRow> field(H, BitRow(W));
    
    boardHash.resize(H);
    for (i = 0; i < H; i++) {
        boardHash[i].resize(W);
        for (j = 0; j < W; j++) boardHash[i][j] = ((unsigned long long)GetRandom() << 32) | GetRandom();
    }
    
    initBoard.hash = 0;
    
    scanf("%d", &fieldCount);
    
    for (i = 0; i < fieldCount; i++) {
        int x, y;
        
        scanf("%d %d", &y, &x);
        
        field[x].set(y);
        initBoard.hash ^= boardHash[x][y];
    }
    
    scanf("%d", &sourceLength);
    
    for (i = 0; i < sourceLength; i++) {
        int id;
        
        scanf("%d", &id);
        
        source.push_back(id);
    }
    
    initBoard.currentScore = initBoard.previousLine = 0;
    initBoard.expectedScore = calc(field, 0);
    initBoard.commands = "";
    initBoard.field = field;
    
    que.push(initBoard);
    
    for (i = 0; i < source.size(); i++) {
        set <unsigned long long> states;
        
        for (j = 0; j < beamWidth && !que.empty(); j++) {
            Board board = que.top();
            queue <pair<Point, int> > queBFS;
            map <pair<Point, int>, int> parent;
            
            que.pop();
            
            debug(board);
            
            if (board.currentScore > maxScore) {
                maxScore = board.currentScore;
                ans = board.commands;
            }
            
            if (!check(board.field, units[source[i]].pivot, 0, i)) continue;
            
            parent[make_pair(units[source[i]].pivot, 0)] = -1;
            queBFS.push(make_pair(units[source[i]].pivot, 0));
            
            while (!queBFS.empty()) {
                bool insert = false;
                Point point = queBFS.front().first;
                int theta = queBFS.front().second;
                
                queBFS.pop();
                
                // 移動
                for (k = 0; k < 4; k++) {
                    Point nextPoint = point;
                    
                    nextPoint.x += dx[k];
                    nextPoint.y += dy[point.x % 2][k];
                    
                    if (check(board.field, nextPoint, theta, i)) {
                        if (!parent.count(make_pair(nextPoint, theta))) {
                            parent[make_pair(nextPoint, theta)] = k;
                            queBFS.push(make_pair(nextPoint, theta));
                        }
                    } else if (!insert) {
                        insert = true;
                        Board nextBoard = board;
                        
                        update(nextBoard, point, theta, i);
                        
                        if (states.count(nextBoard.hash)) continue;
                        states.insert(nextBoard.hash);
                        
                        Point nowPoint = point;
                        int nowTheta = theta;
                        string commands = commandMove[k];
                        while (1) {
                            int commandNum = parent[make_pair(nowPoint, nowTheta)];
                            
                            if (commandNum == -1) break;
                            
                            if (commandNum <= 3) {
                                commands += commandMove[commandNum];
                                Point newPoint = nowPoint;
                                newPoint.x += rdx[commandNum];
                                newPoint.y += rdy[nowPoint.x % 2][commandNum];
                                nowPoint = newPoint;
                            } else {
                                commandNum -= 5;
                                commands += commandRotate[commandNum + 1];
                                nowTheta = (nowTheta - commandNum + 6) % 6;
                            }
                        }
                        reverse(commands.begin(), commands.end());
                        nextBoard.commands += commands;
                        nextBoard.expectedScore = calc(nextBoard.field, i + 1);
                        queNext.push(nextBoard);
                    }
                }
                
                // 回転
                for (k = -1; k <= 1; k++) {
                    if (k == 0) continue;
                    
                    int nextTheta = (theta + k + 6) % 6;
                    
                    if (check(board.field, point, nextTheta, i)) {
                        if (!parent.count(make_pair(point, nextTheta))) {
                            parent[make_pair(point, nextTheta)] = k + 5;
                            queBFS.push(make_pair(point, nextTheta));
                        }
                    } else if (!insert) {
                        insert = true;
                        Board nextBoard = board;
                        
                        update(nextBoard, point, theta, i);
                        
                        if (states.count(nextBoard.hash)) continue;
                        states.insert(nextBoard.hash);
                        
                        Point nowPoint = point;
                        int nowTheta = theta;
                        string commands = commandRotate[k + 1];
                        while (1) {
                            int commandNum = parent[make_pair(nowPoint, nowTheta)];
                            
                            if (commandNum == -1) break;
                            
                            if (commandNum <= 3) {
                                commands += commandMove[commandNum];
                                Point newPoint = nowPoint;
                                newPoint.x += rdx[commandNum];
                                newPoint.y += rdy[nowPoint.x % 2][commandNum];
                                nowPoint = newPoint;
                            } else {
                                commandNum -= 5;
                                commands += commandRotate[commandNum + 1];
                                nowTheta = (nowTheta - commandNum + 6) % 6;
                            }
                        }
                        reverse(commands.begin(), commands.end());
                        nextBoard.commands += commands;
                        nextBoard.expectedScore = calc(nextBoard.field, i + 1);
                        queNext.push(nextBoard);
                    }
                }
            }
        }
        
        while (!que.empty()) {
            if (que.top().currentScore > maxScore) {
                maxScore = que.top().currentScore;
                ans = que.top().commands;
            }
            
            que.pop();
        }
        
        swap(que, queNext);
    }
    
    while (!que.empty()) {
        if (que.top().currentScore > maxScore) ans = que.top().commands;
        
        que.pop();
    }
    
    fprintf(stderr, "%d\n", maxScore);
    fflush(stderr);
    
    for (i = 0; i < ans.size(); i++) {
        switch (ans[i]) {
            case 'W':
            putchar('p');
            break;
            case 'E':
            putchar('b');
            break;
            case 'D':
            putchar('l');
            break;
            case 'S':
            putchar('a');
            break;
            case 'X':
            putchar('k');
            break;
            case 'C':
            putchar('d');
            break;
        }
    }
    
    return 0;
}
