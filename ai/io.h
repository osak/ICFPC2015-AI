#pragma once

#include "common.h"
#include "model/board.h"
#include "model/unit.h"
#include "model/game.h"
#include "util.h"

using namespace std;

/*
	createGameFromInput::入力からGameを生成
*/

// 各 unit を pivot からの相対座標に直す
void init(Unit &unit, int H, int W) {
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
        Point point = Util::get(unit.pivot, unit.member[i].x, unit.member[i].y);
        
        lefty = min(lefty, point.y);
        righty = min(righty, W - point.y - 1);
    }
    
    if (lefty >= righty) {
        unit.pivot.y -= (lefty - righty + 1) / 2;
    } else {
        unit.pivot.y += (righty - lefty) / 2;
    }
}

//void debug(Board &board) {
//	return;
//    fprintf(stderr, "%d %d\n", board.currentScore, board.expectedScore);
//    fprintf(stderr, "%s\n", board.commands.c_str());
//    for (int i = 0; i < H; i++) {
//        for (int j = 0; j < W; j++) {
//            fprintf(stderr, "%d", board.field[i].get(j));
//        }
//        fprintf(stderr, "\n");
//    }
//    fprintf(stderr, "\n");
//}

Game createGameFromInput() {
	Game game;
	int &H = game.H, &W = game.W;
	auto &boardHash = game.boardHash;
	auto &units = game.units;

	vector<Unit> unitTypes;
    int unitCount, fieldCount, sourceLength, i, j, k;
    Board &initBoard = game.board;
    
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
        
		init(unit, H, W);
        
		unitTypes.push_back(unit);
    }
    
    vector <BitRow> field(H, BitRow(W));
    
    boardHash.resize(H);
    for (i = 0; i < H; i++) {
        boardHash[i].resize(W);
		for (j = 0; j < W; j++) boardHash[i][j] = ((unsigned long long)Util::GetRandom() << 32) | Util::GetRandom();
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
        
		units.push_back(unitTypes[id]);
    }
    
	initBoard.currentScore = 0;
	initBoard.previousLine = 0;
    initBoard.expectedScore = 0;
    initBoard.commands = "";
    initBoard.field = field;
    
    return game;
}
