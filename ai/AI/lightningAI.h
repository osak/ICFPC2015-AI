#pragma once

#include "../common.h"
#include "../util.h"
#include "../model/board.h"
#include "../model/unit.h"
#include "../model/game.h"
#include "../model/result.h"

using namespace std;

class LightningAI{
	Game game;
    void update(Board &board, const Point &pivot, int theta, const Unit &unit);
	string getCommand(map <pair<Point, int>, int> &parent, Point point, int theta, const char last);
	void debug(const Board &board);
	int calc(const Board &board, const Unit &unit);
public:
	LightningAI(const Game &game):game(game){}
	Result run();
};
