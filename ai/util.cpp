#include "util.h"

using namespace std;

unsigned Util::x = 123456789;
unsigned Util::y = 362436039;
unsigned Util::z = 521288629;
unsigned Util::w = 88675123;

const int Util::pxx[6] = { 1, 1, 0, -1, -1, 0 };
const int Util::pxy[6] = { 0, 1, 1, 0, -1, -1 };
const int Util::pyx[6] = { 0, -1, -1, 0, 1, 1 };
const int Util::pyy[6] = { 1, 0, -1, -1, 0, 1 };
const int Util::dx[4] = { 0, 0, 1, 1 };
const int Util::dy[2][4] = { 1, -1, 0, -1, 1, -1, 1, 0 };
const int Util::rdx[4] = { 0, 0, -1, -1 };
const int Util::rdy[2][4] = { -1, 1, -1, 0, -1, 1, 0, 1 };
const char Util::commandMove[4] = { 'E', 'W', 'D', 'S' };
const char Util::commandRotate[3] = { 'X', ' ', 'C' };