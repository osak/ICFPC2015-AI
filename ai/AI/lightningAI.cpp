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

string LightningAI::getCommand(Table &table, State state, const char last) {
    string commands(1, last);
    
    while (1) {
        if (!table.count(state)) break;
        
        commands += table[state].command;
        state = table[state].parent;
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
            fprintf(stderr, "%d ", board.field[i].get(j));
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
    fflush(stderr);
}

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

unsigned getHash(State &state) {
    unsigned hash = 0;
    
    hash |= (unsigned)state.pivot.x << 22;
    hash |= (unsigned)state.pivot.y << 14;
    hash |= (unsigned)state.theta << 11;
    hash |= (unsigned)state.bannedPivot << 3;
    hash |= (unsigned)state.bannedTheta;
    
    return hash;
}

Result LightningAI::run(){
    int i, j, k;
    int maxScore = -1;
    int beamWidth = 10;
    string ans = "";
    priority_queue <Board, vector<Board>, greater<Board> > que, queNext;
    priority_queue <pair <unsigned, Board> > variety;
    priority_queue <State> queSearch;
    unordered_set <unsigned long long> states;
    unordered_set <unsigned> visited;
    ValidTable valid;
    Table table;
    
    LightningEval evaluator(game.H, game.W, game.units);
    game.board.expectedScore = evaluator.calc(game.board.field, 0);
    
    que.push(game.board);

	int maxVarietySize = beamWidth / 5;
	auto varietyUpdate = [&](const Board &nextBoard){
		queNext.push(nextBoard);
		if (queNext.size() > beamWidth - maxVarietySize) {
			variety.push(make_pair(Util::GetRandom(), queNext.top()));
			if (variety.size() >= maxVarietySize) variety.pop();
			queNext.pop();
		}
	};
    
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
            
            visited.clear();
            
            queSearch.push(State(0, game.units[i].pivot, 0, 0, 6));
            
            while (!queSearch.empty()) {
                bool insert = false;
                int flag = 0;
                unsigned hash;
                State state = queSearch.top();
                
                queSearch.pop();
                
                hash = getHash(state);
                if (visited.count(hash)) continue;
                visited.insert(hash);
                
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
                        nextBoard.commands += getCommand(table, state, Util::commandMove[k]);
                        nextBoard.expectedScore = evaluator.calc(nextBoard.field, i + 1);
						varietyUpdate(nextBoard);
                    } else {
                        if (k <= 1 && state.bannedPivot == pivot.y && state.bannedTheta == state.theta) continue;
                        
                        State nextState(state.power, pivot, state.theta, state.bannedPivot, state.bannedTheta);
                        if (k >= 2) {
                            nextState.bannedPivot = 0;
                            nextState.bannedTheta = 6;
                        }
                        
                        if (visited.count(getHash(nextState))) continue;
                        
                        flag |= (1 << k);
                        
                        if (table.count(nextState) && table[nextState].value >= state.power) continue;
                        
                        table[nextState] = Data(nextState.power, state, Util::commandMove[k]);
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
                    
                    if (board.powerScore == 0 && state.power == 0) {
                        nextState3.power += 306;
                    } else {
                        nextState3.power += 6;
                    }
                    
                    if (!table.count(nextState3) || table[nextState3].value < nextState3.power) {
                        table[nextState1] = Data(nextState1.power, state, Util::commandMove[0]);
                        table[nextState2] = Data(nextState2.power, nextState1, Util::commandMove[3]);
                        table[nextState3] = Data(nextState3.power, nextState2, Util::commandMove[1]);
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
                        nextBoard.commands += getCommand(table, state, Util::commandRotate[k + 1]);
                        nextBoard.expectedScore = evaluator.calc(nextBoard.field, i + 1);
						varietyUpdate(nextBoard);
                    } else {
                        if (state.bannedPivot == state.pivot.y && state.bannedTheta == theta) continue;
                        
                        State nextState(state.power, state.pivot, theta, state.bannedPivot, state.bannedTheta);
                        
                        if (visited.count(getHash(nextState))) continue;
                        
                        if (table.count(nextState) && table[nextState].value >= state.power) continue;
                        
                        table[nextState] = Data(nextState.power, state, Util::commandRotate[k + 1]);
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
        
        while (!variety.empty()) {
            queNext.push(variety.top().second);
            variety.pop();
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
