#include "common.h"
#include "board.h"
#include "unit.h"
#include "util.h"
#include "game.h"
#include "result.h"

using namespace std;

class LightningAI{
	Game game;
	void update(Board &board, Point &pivot, int theta, Unit &unit);
	void debug(Board &board);
public:
	LightningAI(Game game){
		this->game = game;
	}
	Result run();
};
