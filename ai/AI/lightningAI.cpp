#include "lightningAI.h"
#include "../evaluation/lightningeval.h"

using namespace std;

void LightningAI::update(Board &board, const Point &pivot, int theta, const Unit &unit) {
    int count = 0, point, i, j;
    
    for (i = 0; i < unit.member.size(); i++) {
        Point p = Util::get(pivot, theta, unit.member[i]);
        
        board.field[p.x].set(p.y);
        board.hash ^= game.boardHash[p.x][p.y];
    }
    
    for (i = game.H - 1; i >= 0; i--) {
        if (board.field[i].count() == game.W) {
            count++;
        }
        else {
            board.field[i + count] = board.field[i];
        }
    }
    
    for (i = 0; i < count; i++) board.field[i].reset();
    
    if (count > 0) {
        board.hash = 0;
        
        for (i = count; i < game.H; i++) {
            for (j = 0; j < game.W; j++) {
                if (board.field[i].test(j)) board.hash ^= game.boardHash[i][j];
            }
        }
    }
    
    point = unit.member.size() + 100 * (1 + count) * count / 2;
    board.currentScore += point;
    if (board.previousLine > 1) board.currentScore += (board.previousLine - 1) * point / 10;
    board.previousLine = count;
}

string LightningAI::getCommand(map <pair<Point, int>, int> &parent, Point point, int theta, const char last) {
    string commands(1, last);
    
    while (1) {
        int commandNum = parent[make_pair(point, theta)];
        
        if (commandNum == -1) break;
        
        if (commandNum <= 3) {
            commands += Util::commandMove[commandNum];
            Point newPoint = point;
            newPoint.x += Util::rdx[commandNum];
            newPoint.y += Util::rdy[point.x % 2][commandNum];
            point = newPoint;
        } else {
            commandNum -= 5;
            commands += Util::commandRotate[commandNum + 1];
            theta = (theta - commandNum + 6) % 6;
        }
    }
    
    reverse(commands.begin(), commands.end());
    
    return commands;
}

void LightningAI::debug(const Board &board) {
    return;
    fprintf(stderr, "%d %d %d\n", board.currentScore, board.powerScore, board.expectedScore);
    fprintf(stderr, "%s\n", board.commands.c_str());
    for (int i = 0; i < game.H; i++) {
		if (i % 2) fprintf(stderr, " ");
        for (int j = 0; j < game.W; j++) {
            fprintf(stderr, "%d ", board.field[i].test(j));
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
    fflush(stderr);
}

unsigned long long getHash(Point &pivot, int theta, Unit &unit) {
    unsigned long long hash = 0;
    vector <Point> points;
    
    for (int i = 0; i < unit.member.size(); i++) points.push_back(Util::get(pivot, theta, unit.member[i]));
    
    sort(points.begin(), points.end());
    
    points.push_back(pivot);
    
    for (int i = 0; i < points.size(); i++) {
        hash = hash * 1009 + points[i].x;
        hash = hash * 1009 + points[i].y;
    }
    
    return hash;
}

typedef int(LightningEval::*pEvaluator)(vector <BitRow> &field, int num);
vector<pEvaluator> evaluators;
vector<int> slotSizes;

Result LightningAI::run(){
    int i, j, k;
    int maxScore = -1;
    int beamWidth = 10;
    string ans = "";
    priority_queue <State> queSearch;
    
    LightningEval evaluator(game.H, game.W, game.units);
	evaluators.push_back(&LightningEval::calcMaster);
	slotSizes.push_back(5);
	//evaluators.push_back(&LightningEval::calcGod);
	evaluators.push_back(&LightningEval::calcBuddha);
	slotSizes.push_back(5);

	int evNum = evaluators.size();
	// int eachWidth = beamWidth + (evNum - 1) / evNum;
	vector <priority_queue <Board, vector<Board>, greater<Board> > > ques(evNum);
	priority_queue <Board, vector<Board>, greater<Board> > que, queNext;

	auto calcAndPick = [&](Board &board, int num) {
		for (int i = 0; i < evNum; ++i) {
			auto &calc = evaluators[i];
			board.expectedScore = (evaluator.*calc)(board.field, num);
			// 同じ盤面を色んなqueueに突っ込むクソコードを書くやつがいるらしい
			ques[i].push(board);
			if (ques[i].size() > slotSizes[i]) ques[i].pop();
		}
	};

	game.board.expectedScore = 0;
    que.push(game.board);

    for (i = 0; i < game.units.size(); i++) {
        set <unsigned long long> states;
        
		for (j = 0; j < beamWidth && !que.empty(); j++) {
            Board board = que.top();
            map <pair<Point, int>, int> parent;
            unordered_set <unsigned long long> hash;
            unordered_set <unsigned long long> visit;
            
            que.pop();
            
            debug(board);
            
            if (board.currentScore + board.powerScore > maxScore) {
                maxScore = board.currentScore + board.powerScore;
                ans = board.commands;
            }
            
            if (!Util::check(game.H, game.W, board.field, game.units[i].pivot, 0, game.units[i])) continue;
            
            hash.insert(getHash(game.units[i].pivot, 0, game.units[i]));
            parent[make_pair(game.units[i].pivot, 0)] = -1;
            queSearch.push(State(0, game.units[i].pivot, 0));
            
            while (!queSearch.empty()) {
                bool insert = false;
                int flag = 0;
                unsigned long long value;
                State state = queSearch.top();
                
                queSearch.pop();
                
                value = getHash(state.pivot, state.theta, game.units[i]);
                if (visit.count(value)) continue;
                visit.insert(value);
                
                // 移動
                for (k = 0; k < 4; k++) {
                    Point pivot = state.pivot;
                    
                    pivot.x += Util::dx[k];
                    pivot.y += Util::dy[state.pivot.x % 2][k];
                    
                    if (Util::check(game.H, game.W, board.field, pivot, state.theta, game.units[i])) {
                        unsigned long long nextHash = getHash(pivot, state.theta, game.units[i]);
                        if (!hash.count(nextHash)) {
                            flag |= (1 << k);
                            hash.insert(nextHash);
                            parent[make_pair(pivot, state.theta)] = k;
                            queSearch.push(State(state.power, pivot, state.theta));
                        }
                    } else if (!insert) {
                        insert = true;
                        Board nextBoard = board;
                        
                        update(nextBoard, state.pivot, state.theta, game.units[i]);
                        
                        if (states.count(nextBoard.hash)) continue;
                        states.insert(nextBoard.hash);
                        
                        nextBoard.powerScore += state.power;
                        nextBoard.commands += getCommand(parent, state.pivot, state.theta, Util::commandMove[k]);
								calcAndPick(nextBoard, i + 1);
                    }
                }
                
                // フレーズ (3方向に移動可能なときにei!を唱えられる)
                if ((flag & 13) == 13) {
                    Point pivot = state.pivot;
                    
                    pivot.x += Util::dx[2];
                    pivot.y += Util::dy[state.pivot.x % 2][2];
                    
                    parent[make_pair(pivot, state.theta)] = 3;
                    
                    pivot = state.pivot;
                    
                    pivot.x += Util::dx[3];
                    pivot.y += Util::dy[state.pivot.x % 2][3];
                    
                    parent[make_pair(pivot, state.theta)] = 1;
                    if (board.powerScore == 0 && state.power == 0) {
                        queSearch.push(State(state.power + 306, pivot, state.theta));
                    } else {
                        queSearch.push(State(state.power + 6, pivot, state.theta));
                    }
                }
                
                // 回転
                for (k = -1; k <= 1; k++) {
                    if (k == 0) continue;
                    
                    int theta = (state.theta + k + 6) % 6;
                    
                    if (Util::check(game.H, game.W, board.field, state.pivot, theta, game.units[i])) {
                        unsigned long long nextHash = getHash(state.pivot, theta, game.units[i]);
                        if (!hash.count(nextHash)) {
                            hash.insert(nextHash);
                            parent[make_pair(state.pivot, theta)] = k + 5;
                            queSearch.push(State(state.power, state.pivot, theta));
                        }
                    } else if (!insert) {
                        insert = true;
                        Board nextBoard = board;
                        
                        update(nextBoard, state.pivot, state.theta, game.units[i]);
                        
                        if (states.count(nextBoard.hash)) continue;
                        states.insert(nextBoard.hash);
                        
                        nextBoard.powerScore += state.power;
                        nextBoard.commands += getCommand(parent, state.pivot, state.theta, Util::commandRotate[k + 1]);
								calcAndPick(nextBoard, i + 1);
						}
               }
            }
        }
        
        while (!que.empty()) {
            if (que.top().currentScore + que.top().powerScore > maxScore) {
                maxScore = que.top().currentScore + que.top().powerScore;
                ans = que.top().commands;
            }
            
            que.pop();
        }
        
		// merge
		for (int i = 0; i < evNum; ++i) {
			while (!ques[i].empty()){
				queNext.push(ques[i].top());
				ques[i].pop();
			}
		}
        swap(que, queNext);
    }
    
    while (!que.empty()) {
        if (que.top().currentScore + que.top().powerScore > maxScore) {
            maxScore = que.top().currentScore + que.top().powerScore;
            ans = que.top().commands;
        }
        
        que.pop();
    }
    
    //fprintf(stderr, "%d\n", maxScore);
    //fflush(stderr);
    
    string res = "";
    for (i = 0; i < ans.size(); i++) {
        switch (ans[i]) {
            case 'W':
            res.push_back('!');
            break;
            case 'E':
            res.push_back('e');
            break;
            case 'D':
            res.push_back('l');
            break;
            case 'S':
            res.push_back('i');
            break;
            case 'X':
            res.push_back('k');
            break;
            case 'C':
            res.push_back('d');
            break;
        }
    }
    
    return Result(res, maxScore);
}
