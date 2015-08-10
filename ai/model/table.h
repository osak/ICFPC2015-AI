#ifndef AI_TABLE_H
#define AI_TABLE_H

#include "../common.h"
#include "unit.h"
#include "state.h"

const int MAX_BOARD_SIZE = 50;
const int MARGIN = 10;
class ValidTable {
    pair<int, int> table[MAX_BOARD_SIZE+MARGIN*2][MAX_BOARD_SIZE+MARGIN*2][6];
    int turn;

public:
    ValidTable() {
        for(int i = 0; i < MAX_BOARD_SIZE + MARGIN * 2; ++i) {
            for(int j = 0; j < MAX_BOARD_SIZE + MARGIN * 2; ++j) {
                for (int k = 0; k < 6; ++k) {
                    table[i][j][k] = make_pair(-1, -1);
                }
            }
        }
        turn = 0;
    }

    int operator[](const std::pair<Point, int> &key) const {
        // Here assumes given key is **valid** one; must be already inserted.
        return table[key.first.x+MARGIN][key.first.y+MARGIN][key.second].second;
    }

    int& operator[](const std::pair<Point, int> &key) {
        // Here assumes value always to be set when caller requires lvalue.
        table[key.first.x+MARGIN][key.first.y+MARGIN][key.second].first = turn;
        return table[key.first.x+MARGIN][key.first.y+MARGIN][key.second].second;
    }

    bool count(const std::pair<Point, int> &key) const {
        return table[key.first.x+MARGIN][key.first.y+MARGIN][key.second].first == turn;
    }

    void clear() {
        ++turn;
    }
};

class Data {
    public:
    
    int value;
    State parent;
    char command;
    
    Data() {}
    
    Data(int value, State &parent, char command) : value(value), parent(parent), command(command) {}
};

class Table {
    int last[MAX_BOARD_SIZE+MARGIN*2][MAX_BOARD_SIZE+MARGIN*2][6][MAX_BOARD_SIZE+MARGIN*2][7];
    Data table[MAX_BOARD_SIZE+MARGIN*2][MAX_BOARD_SIZE+MARGIN*2][6][MAX_BOARD_SIZE+MARGIN*2][7];
    int turn;

public:
    Table() {
        for(int i = 0; i < MAX_BOARD_SIZE + MARGIN * 2; ++i) {
            for(int j = 0; j < MAX_BOARD_SIZE + MARGIN * 2; ++j) {
                for (int k = 0; k < 6; ++k) {
                    for (int l = 0; l < MAX_BOARD_SIZE + MARGIN * 2; ++l) {
                        for (int r = 0; r < 7; r++) {
                            last[i][j][k][l][r] = -1;
                        }
                    }
                }
            }
        }
        turn = 0;
    }

    Data operator[](const State &key) const {
        // Here assumes given key is **valid** one; must be already inserted.
        return table[key.pivot.x+MARGIN][key.pivot.y+MARGIN][key.theta][key.bannedPivot][key.bannedTheta];
    }

    Data& operator[](const State &key) {
        // Here assumes value always to be set when caller requires lvalue.
        last[key.pivot.x+MARGIN][key.pivot.y+MARGIN][key.theta][key.bannedPivot][key.bannedTheta] = turn;
        return table[key.pivot.x+MARGIN][key.pivot.y+MARGIN][key.theta][key.bannedPivot][key.bannedTheta];
    }

    bool count(const State &key) const {
        return last[key.pivot.x+MARGIN][key.pivot.y+MARGIN][key.theta][key.bannedPivot][key.bannedTheta] == turn;
    }

    void clear() {
        ++turn;
    }
};

#endif //AI_TABLE_H
