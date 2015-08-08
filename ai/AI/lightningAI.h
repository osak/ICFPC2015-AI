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
	void update(Board &board, Point &pivot, int theta, Unit &unit);
	void debug(Board &board);
	int calc(Board &board, Unit &unit);
public:
	LightningAI(Game game){
		this->game = game;
	}
	Result run();
};
