﻿#include "lightningAI_small.h"
#include "../evaluation/lightningeval_small.h"

using namespace std;

int getTurnNum(Unit &unit) {
    set <vector <Point> > visited;
    
    for (int i = 0; i < 6; i++) {
        vector <Point> points;
        
        for (int j = 0; j < unit.member.size(); j++) points.push_back(Util::get(unit.pivot, i, unit.member[j]));
        
        sort(points.begin(), points.end());
        
        points.push_back(unit.pivot);
        
        if (visited.count(points)) return i;
        
        visited.insert(points);
    }
    
    return 6;
}

inline unsigned long long computeData(int value, State &state, char command) {
    unsigned long long data = 0;
    
    data |= (unsigned long long)value << 39;
    data |= (unsigned long long)command << 31;
    data |= (unsigned long long)(state.pivot.x + 10) << 23;
    data |= (unsigned long long)(state.pivot.y + 10) << 15;
    data |= (unsigned long long)state.theta << 12;
    data |= (unsigned long long)(state.bannedPivot + 10) << 4;
    data |= (unsigned long long)state.bannedTheta << 1;
    
    return data;
}

inline char addCommand(unsigned long long data, State &state) {
    data >>= 1;
    state.bannedTheta = data & 7;
    data >>= 3;
    state.bannedPivot = (int)(data & 255) - 10;
    data >>= 8;
    state.theta = data & 7;
    data >>= 3;
    state.pivot.y = (int)(data & 255) - 10;
    data >>= 8;
    state.pivot.x = (int)(data & 255) - 10;
    data >>= 8;
    
    return data & 255;
}

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

string LightningAI::getCommand(Table &table, State state, const char last) {
    string commands(1, last);
    
    while (1) {
        if ((table[state] & 15) == 15) break;
        
        commands += addCommand(table[state], state);
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

/*
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
*/

typedef int(LightningEval::*pEvaluator)(Board &board, Board &nextBoard, int num);
vector<pEvaluator> evaluators;
vector<int> slotSizes;

Result LightningAI::run(){
    int i, j, k;
    int maxScore = -1;
    int beamWidth = 0;
    string ans = "";
    priority_queue <State> queSearch;
    unordered_set <unsigned> states;
    static ValidTable valid;
    static Table table;
    
    LightningEval evaluator(game.H, game.W, game.units);

	auto addEvaluator = [&](int(LightningEval::*pEvaluator)(Board &board, Board &nextBoard, int num), int slot) {
		evaluators.push_back(pEvaluator);
		beamWidth += slot;
		slotSizes.push_back(slot);
	};
	addEvaluator(&LightningEval::calcMaster, 7);
	addEvaluator(&LightningEval::calcBuddha, 6);
	addEvaluator(&LightningEval::calcGod, 4);
	addEvaluator(&LightningEval::calcHole, 2);
	addEvaluator(&LightningEval::calcRand, 1);

	int evNum = evaluators.size();
	// int eachWidth = beamWidth + (evNum - 1) / evNum;
	vector <priority_queue <Board, vector<Board>, greater<Board> > > ques(evNum);
	priority_queue <Board, vector<Board>, greater<Board> > que, queNext;

	auto calcAndPick = [&](Board &board, Board &nextBoard, int num) {
		for (int i = 0; i < evNum; ++i) {
			auto &calc = evaluators[i];
			nextBoard.expectedScore = (evaluator.*calc)(board, nextBoard, num);
			// 同じ盤面を色んなqueueに突っ込むクソコードを書くやつがいるらしい
			ques[i].push(nextBoard);
			if (ques[i].size() > slotSizes[i]) ques[i].pop();
		}
	};

	game.board.expectedScore = 0;
    que.push(game.board);

    for (i = 0; i < game.units.size(); i++) {
        int turnNum = getTurnNum(game.units[i]);
        
        states.clear();
        
		for (j = 0; j < beamWidth && !que.empty(); j++) {
            Board board = que.top();
            
            que.pop();
            
            debug(board);

            valid.clear();
            table.clear();
            
            if (board.currentScore + board.powerScore > maxScore) {
                maxScore = board.currentScore + board.powerScore;
                ans = board.commands;
            }
            
            auto isValid = [&](Point &pivot, int theta) {
                if (!valid.count(make_pair(pivot, theta))) valid[make_pair(pivot, theta)] = Util::check(game.H, game.W, board.field, pivot, theta, game.units[i]);
                return valid[make_pair(pivot, theta)];
            };
            
            if (!isValid(game.units[i].pivot, 0)) continue;

				for (int spellIx = 0; spellIx < game.spells.size(); spellIx++) {

					if (board.spellMask >> spellIx & 1) {
						continue;
					}
					string spell = game.spells[spellIx];
					Point pivot;
					int theta;
					if (Util::checkSpell(game.H, game.W, turnNum, board, game.units[i], spell, pivot, theta)) {
						int score = 300 + 2 * game.spellLens[spellIx];
						table[State(score, pivot, theta, 0, 6)] = ((unsigned long long)score << 39) | 30 | (spellIx<<5);
						queSearch.push(State(score, pivot, theta, 0, 6));
					}
				}
            
            table[State(0, game.units[i].pivot, 0, 0, 6)] = 14;
            queSearch.push(State(0, game.units[i].pivot, 0, 0, 6));
            
				auto setCommand = [&](Board &nextBoard, State state, char cmd) {
					string commands(1, cmd);

					while (1) {
						if ((table[state] & 15) == 15) break;

						commands += addCommand(table[state], state);
					}

					reverse(commands.begin(), commands.end());

					if (table[state]&16) {
						int spellIx = ((table[state]>>5)&31);
						string &spell = game.spells[spellIx];
						for (int ci = 0; ci < spell.size(); ci++) {
							if (spell[ci] < '4') {
								nextBoard.commands += Util::commandMove[spell[ci] - '0'];
							} else {
								nextBoard.commands += Util::commandRotate[spell[ci] - '4'];
							}
						}
						nextBoard.spellMask |= 1<<spellIx;
					}
					nextBoard.commands += commands;
				};

            while (!queSearch.empty()) {
                bool insert = false;
                int flag = 0;
                State state = queSearch.top();
                
                queSearch.pop();
                
                if (table.visited(state)) continue;
                table[state] |= 1;
                
                
                // 移動
                for (k = 0; k < 4; k++) {
                    Point pivot = state.pivot;
                    
                    pivot.x += Util::dx[k];
                    pivot.y += Util::dy[state.pivot.x % 2][k];
                    
                    if (!isValid(pivot, state.theta)) {
                        if (insert) continue;
                        
                        insert = true;
                        Board nextBoard = board;
                        
                        update(nextBoard, state.pivot, state.theta, game.units[i]);
                        
                        if (states.count(nextBoard.hash)) continue;
                        states.insert(nextBoard.hash);
                        
                        nextBoard.powerScore += state.power;
                        setCommand(nextBoard, state, Util::commandMove[k]);
								calcAndPick(board, nextBoard, i + 1);
                    } else {
                        if (k <= 1 && state.bannedPivot == pivot.y && state.bannedTheta == state.theta) continue;
                        
                        State nextState(state.power, pivot, state.theta, state.bannedPivot, state.bannedTheta);
                        if (k >= 2) {
                            nextState.bannedPivot = 0;
                            nextState.bannedTheta = 6;
                        }
                        
                        if (table.visited(nextState)) continue;
                        
                        flag |= (1 << k);
                        
                        if (table.getValue(nextState) >= state.power) continue;
                        
                        table[nextState] = computeData(nextState.power, state, Util::commandMove[k]);
                        queSearch.push(nextState);
                    }
                }
                
                // フレーズ (3方向に移動可能なときにei!を唱えられる)
                if ((flag & 13) == 13) {
                    State nextState1 = state;
                    
                    nextState1.pivot.x += Util::dx[0];
                    nextState1.pivot.y += Util::dy[state.pivot.x % 2][0];
                    nextState1.bannedPivot = 1;
                    nextState1.bannedTheta = 6;
                    
                    State nextState2 = nextState1;
                    
                    nextState2.pivot.x += Util::dx[3];
                    nextState2.pivot.y += Util::dy[state.pivot.x % 2][3];
                    nextState2.bannedPivot = nextState2.pivot.y;
                    nextState2.bannedTheta = nextState2.theta;
                    
                    State nextState3 = nextState2;
                    
                    nextState3.pivot.x += Util::dx[1];
                    nextState3.pivot.y += Util::dy[(state.pivot.x + 1) % 2][1];
                    
							nextState3.power += 6;
                    
                    if (table.getValue(nextState3) < nextState3.power) {
                        table[nextState1] = computeData(nextState1.power, state, Util::commandMove[0]);
                        table[nextState2] = computeData(nextState2.power, nextState1, Util::commandMove[3]);
                        table[nextState3] = computeData(nextState3.power, nextState2, Util::commandMove[1]);
                        queSearch.push(nextState3);
                    }
                }
                
                // 回転
                for (k = -1; k <= 1; k++) {
                    if (k == 0) continue;
                    
                    int theta = (state.theta + k + turnNum) % turnNum;
                    
                    if (!isValid(state.pivot, theta)) {
                        if (insert) continue;
                        
                        insert = true;
                        Board nextBoard = board;
                        
                        update(nextBoard, state.pivot, state.theta, game.units[i]);
                        
                        if (states.count(nextBoard.hash)) continue;
                        states.insert(nextBoard.hash);
                        
                        nextBoard.powerScore += state.power;
                        setCommand(nextBoard, state, Util::commandRotate[k + 1]);
								calcAndPick(board, nextBoard, i + 1);
                    } else {
                        if (state.bannedPivot == state.pivot.y && state.bannedTheta == theta) continue;
                        
                        State nextState(state.power, state.pivot, theta, state.bannedPivot, state.bannedTheta);
                        
                        if (table.visited(nextState)) continue;
                        
                        if (table.getValue(nextState) >= state.power) continue;
                        
                        table[nextState] = computeData(nextState.power, state, Util::commandRotate[k + 1]);
                        queSearch.push(nextState);
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
       
		// merge2
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