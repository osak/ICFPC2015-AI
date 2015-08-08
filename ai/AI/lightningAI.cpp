#include "lightningAI.h"
#include "../evaluation/lightningeval.h"

using namespace std;

void LightningAI::update(Board &board, Point &pivot, int theta, Unit &unit) {
	int count = 0, point, i, j;

	for (i = 0; i < unit.member.size(); i++) {
		Point p = Util::get(pivot, theta, unit.member[i]);

		board.field[p.x].set(p.y);
		board.hash ^= game.boardHash[p.x][p.y];
	}

	for (i = game.H - 1; i >= 0; i--) {
		if (board.field[i].check(game.W)) {
			count++;
		}
		else {
			board.field[i + count] = board.field[i];
		}
	}

	for (i = 0; i < count; i++) board.field[i].clear();

	if (count > 0) {
		board.hash = 0;

		for (i = count; i < game.H; i++) {
			for (j = 0; j < game.W; j++) {
				if (board.field[i].get(j)) board.hash ^= game.boardHash[i][j];
			}
		}
	}

	point = unit.member.size() + 100 * (1 + count) * count / 2;
	board.currentScore += point;
	if (board.previousLine > 1) board.currentScore += (board.previousLine - 1) * point / 10;
	board.previousLine = count;
}

void LightningAI::debug(Board &board) {
	return;
    fprintf(stderr, "%d %d\n", board.currentScore, board.expectedScore);
    fprintf(stderr, "%s\n", board.commands.c_str());
    for (int i = 0; i < game.H; i++) {
        for (int j = 0; j < game.W; j++) {
            fprintf(stderr, "%d", board.field[i].get(j));
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
}

Result LightningAI::run(){
	int i, j, k;
	int maxScore = -1;
	int beamWidth = 1;
	string ans = "";
    priority_queue <Board, vector<Board>, greater<Board> > que, queNext;
  
	LightningEval evaluator(game.H, game.W, game.units);
	game.board.expectedScore = evaluator.calc(game.board.field, 0);
    
	que.push(game.board);
    
	priority_queue <pair <unsigned, Board> > variety;
	int maxVarietySize = beamWidth / 20;
    for (i = 0; i < game.units.size(); i++) {
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
            
			if (!Util::check(game.H, game.W, board.field, game.units[i].pivot, 0, game.units[i])) continue;
            
            parent[make_pair(game.units[i].pivot, 0)] = -1;
            queBFS.push(make_pair(game.units[i].pivot, 0));
            
            while (!queBFS.empty()) {
                bool insert = false;
                Point point = queBFS.front().first;
                int theta = queBFS.front().second;
                
                queBFS.pop();
                
                // 移動
                for (k = 0; k < 4; k++) {
                    Point nextPoint = point;
                    
                    nextPoint.x += Util::dx[k];
					nextPoint.y += Util::dy[point.x % 2][k];
                    
					if (Util::check(game.H, game.W, board.field, nextPoint, theta, game.units[i])) {
                        if (!parent.count(make_pair(nextPoint, theta))) {
                            parent[make_pair(nextPoint, theta)] = k;
                            queBFS.push(make_pair(nextPoint, theta));
                        }
                    } else if (!insert) {
                        insert = true;
                        Board nextBoard = board;
                        
						update(nextBoard, point, theta, game.units[i]);
                        
                        if (states.count(nextBoard.hash)) continue;
                        states.insert(nextBoard.hash);
                        
                        Point nowPoint = point;
                        int nowTheta = theta;
						string commands = Util::commandMove[k];
                        while (1) {
                            int commandNum = parent[make_pair(nowPoint, nowTheta)];
                            
                            if (commandNum == -1) break;
                            
                            if (commandNum <= 3) {
								commands += Util::commandMove[commandNum];
                                Point newPoint = nowPoint;
								newPoint.x += Util::rdx[commandNum];
								newPoint.y += Util::rdy[nowPoint.x % 2][commandNum];
                                nowPoint = newPoint;
                            } else {
                                commandNum -= 5;
								commands += Util::commandRotate[commandNum + 1];
                                nowTheta = (nowTheta - commandNum + 6) % 6;
                            }
                        }
                        reverse(commands.begin(), commands.end());
                        nextBoard.commands += commands;
						nextBoard.expectedScore = evaluator.calc(nextBoard.field, i + 1);
                        queNext.push(nextBoard);
						if (queNext.size() > beamWidth - maxVarietySize) {
							variety.push(make_pair(Util::GetRandom(), queNext.top()));
							if (variety.size() >= maxVarietySize) variety.pop();
							queNext.pop();
						}
                    }
                }
                
                // 回転
                for (k = -1; k <= 1; k++) {
                    if (k == 0) continue;
                    
                    int nextTheta = (theta + k + 6) % 6;
                    
					if (Util::check(game.H, game.W, board.field, point, nextTheta, game.units[i])) {
                        if (!parent.count(make_pair(point, nextTheta))) {
                            parent[make_pair(point, nextTheta)] = k + 5;
                            queBFS.push(make_pair(point, nextTheta));
                        }
                    } else if (!insert) {
                        insert = true;
                        Board nextBoard = board;
                        
                        update(nextBoard, point, theta, game.units[i]);
                        
                        if (states.count(nextBoard.hash)) continue;
                        states.insert(nextBoard.hash);
                        
                        Point nowPoint = point;
                        int nowTheta = theta;
						string commands = Util::commandRotate[k + 1];
                        while (1) {
                            int commandNum = parent[make_pair(nowPoint, nowTheta)];
                            
                            if (commandNum == -1) break;
                            
                            if (commandNum <= 3) {
								commands += Util::commandMove[commandNum];
                                Point newPoint = nowPoint;
								newPoint.x += Util::rdx[commandNum];
								newPoint.y += Util::rdy[nowPoint.x % 2][commandNum];
                                nowPoint = newPoint;
                            } else {
                                commandNum -= 5;
								commands += Util::commandRotate[commandNum + 1];
                                nowTheta = (nowTheta - commandNum + 6) % 6;
                            }
                        }
                        reverse(commands.begin(), commands.end());
                        nextBoard.commands += commands;
						nextBoard.expectedScore = evaluator.calc(nextBoard.field, i + 1);
						queNext.push(nextBoard);
						if (queNext.size() > beamWidth - maxVarietySize) {
							variety.push(make_pair(Util::GetRandom(), queNext.top()));
							if (variety.size() >= maxVarietySize) variety.pop();
							queNext.pop();
						}
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
        
		while (!variety.empty()) {
			queNext.push(variety.top().second);
			variety.pop();
		}
		swap(que, queNext);
    }
    
    while (!que.empty()) {
        if (que.top().currentScore > maxScore) {
            maxScore = que.top().currentScore;
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
				res.push_back('p');
            break;
            case 'E':
				res.push_back('b');
			break;
            case 'D':
				res.push_back('l');
            break;
            case 'S':
				res.push_back('a');
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
