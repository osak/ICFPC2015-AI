#ifndef AI_TABLE_H
#define AI_TABLE_H

#include "../common.h"
#include "unit.h"
#include "state.h"

const int MAX_BOARD_SIZE = 100;
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

class Table {
    pair <int, unsigned long long> table[MAX_BOARD_SIZE+MARGIN*2][MAX_BOARD_SIZE+MARGIN*2][6][MAX_BOARD_SIZE+MARGIN*2][7];
    int turn;

public:
    Table() {
        for(int i = 0; i < MAX_BOARD_SIZE + MARGIN * 2; ++i) {
            for(int j = 0; j < MAX_BOARD_SIZE + MARGIN * 2; ++j) {
                for (int k = 0; k < 6; ++k) {
                    for (int l = 0; l < MAX_BOARD_SIZE + MARGIN * 2; ++l) {
                        for (int r = 0; r < 7; r++) {
                            table[i][j][k][l][r] = make_pair(-1, 0);
                        }
                    }
                }
            }
        }
        turn = 0;
    }

    unsigned long long operator[](const State &key) const {
        // Here assumes given key is **valid** one; must be already inserted.
        return table[key.pivot.x+MARGIN][key.pivot.y+MARGIN][key.theta][key.bannedPivot][key.bannedTheta].second;
    }

    unsigned long long& operator[](const State &key) {
        // Here assumes value always to be set when caller requires lvalue.
        table[key.pivot.x+MARGIN][key.pivot.y+MARGIN][key.theta][key.bannedPivot][key.bannedTheta].first = turn;
        return table[key.pivot.x+MARGIN][key.pivot.y+MARGIN][key.theta][key.bannedPivot][key.bannedTheta].second;
    }

    bool count(const State &key) const {
        return table[key.pivot.x+MARGIN][key.pivot.y+MARGIN][key.theta][key.bannedPivot][key.bannedTheta].first == turn;
    }
    
    bool visited(const State &key) const {
        return table[key.pivot.x+MARGIN][key.pivot.y+MARGIN][key.theta][key.bannedPivot][key.bannedTheta].first == turn && (table[key.pivot.x+MARGIN][key.pivot.y+MARGIN][key.theta][key.bannedPivot][key.bannedTheta].second & 1);
    }
    
    int getValue(const State &key) const {
        if (table[key.pivot.x+MARGIN][key.pivot.y+MARGIN][key.theta][key.bannedPivot][key.bannedTheta].first != turn) return -1;
        return table[key.pivot.x+MARGIN][key.pivot.y+MARGIN][key.theta][key.bannedPivot][key.bannedTheta].second >> 39;
    }

    void clear() {
        ++turn;
    }
};

#endif //AI_TABLE_H
