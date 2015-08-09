//
// Created by yaya on 8/9/15.
//

#ifndef AI_TABLE_H
#define AI_TABLE_H

#include "../common.h"
#include "unit.h"

const int MAX_BOARD_SIZE = 300;
const int MARGIN = 100;
class Table {
    std::pair<int, int> table[MAX_BOARD_SIZE+MARGIN][MAX_BOARD_SIZE+MARGIN][6];
    int turn;

public:
    Table() {
        for(int i = 0; i < MAX_BOARD_SIZE; ++i) {
            for(int j = 0; j < MAX_BOARD_SIZE; ++j) {
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


#endif //AI_TABLE_H
