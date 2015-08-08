#include "common.h"

using namespace std;

struct Result{
	string command;
	int score;

	Result(string command, int score): command(command), score(score) {
	}
};