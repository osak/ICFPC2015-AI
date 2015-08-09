#pragma once

#include "common.h"
#include "util.h"
#include "model/game.h"
#include "model/result.h"

using namespace std;

class Calc {
    public:
    
    static const int phraseNum;
    static const string phrase[2];
    
    static vector <Point> getPoints(Point &pivot, int theta, Unit &unit) {
        vector <Point> points;
        
        for (int i = 0; i < unit.member.size(); i++) {
            points.push_back(Util::get(pivot, theta, unit.member[i]));
        }
        
        sort(points.begin(), points.end());
        
        points.push_back(pivot);
        
        return points;
    }
    
    static int getScore(Game &game, Point &pivot, int theta, Unit &unit) {
        int count = 0, point;
        
        for (int i = 0; i < unit.member.size(); i++) {
            Point p = Util::get(pivot, theta, unit.member[i]);
            game.board.field[p.x].set(p.y);
        }
        
        for (int i = game.H - 1; i >= 0; i--) {
            if (game.board.field[i].count() == game.W) {
                count++;
            } else {
                game.board.field[i + count] = game.board.field[i];
            }
        }
        
        for (int i = 0; i < count; i++) game.board.field[i].reset();
        
        point = unit.member.size() + 100 * (1 + count) * count / 2;
        if (game.board.previousLine > 1) point += (game.board.previousLine - 1) * point / 10;
        game.board.previousLine = count;
        
        return point;
    }
    
    static void move(Point &pivot, int &theta, char command) {
        int num = -1;
        
        switch (command) {
            case 'i' :
            num++;
            case 'l' :
            num++;
            case '!' :
            num++;
            case 'e' :
            num++;
            pivot.y += Util::dy[pivot.x % 2][num];
            pivot.x += Util::dx[num];
            break;
            case 'd' :
            num += 2;
            case 'k' :
            theta = (theta + num + 6) % 6;
            break;
        }
        
        return;
    }
    
    static Result calcScore(Game &game, const string &commands) {
        int c = 0;
        Result r("", 0);
        
        for (int i = 0; i < game.units.size() && c < commands.size(); i++) {
            Point pivot = game.units[i].pivot;
            int theta = 0;
            string command = "";
            set <vector <Point> > visited;
            
            if (!Util::check(game.H, game.W, game.board.field, pivot, theta, game.units[i])) {
                if (c != commands.size()) {
                    r.command = "TOO LONG";
                    r.score = 0;
                    return r;
                }
            }
            
            while (c < commands.size()) {
                Point nowPivot = pivot;
                int nowTheta = theta;
                vector <Point> points;
                
                move(pivot, theta, commands[c]);
                
                points = getPoints(pivot, theta, game.units[i]);
                if (visited.count(points)) {
                    r.command = "VISITED";
                    r.score = 0;
                    return r;
                }
                visited.insert(points);
                command += commands[c++];
                if (!Util::check(game.H, game.W, game.board.field, pivot, theta, game.units[i])) {
                    r.score += getScore(game, nowPivot, nowTheta, game.units[i]);
                    break;
                }
            }
            
            for (int j = 0; j < command.size(); j++) {
                if (command[j] == 'i') {
                    Point nowPivot = pivot;
                    int nowTheta = theta;
                    vector <Point> points[2];
                    
                    move(nowPivot, nowTheta, 'e');
                    
                    if (!Util::check(game.H, game.W, game.board.field, nowPivot, nowTheta, game.units[i])) {
                        r.command += command[j];
                        continue;
                    }
                    
                    points[0] = getPoints(nowPivot, nowTheta, game.units[i]);
                    
                    if (visited.count(points[0])) {
                        r.command += command[j];
                        continue;
                    }
                    
                    move(nowPivot, nowTheta, 'i');
                    
                    if (!Util::check(game.H, game.W, game.board.field, nowPivot, nowTheta, game.units[i])) {
                        r.command += command[j];
                        continue;
                    }
                    
                    points[1] = getPoints(nowPivot, nowTheta, game.units[i]);
                    
                    if (visited.count(points[1])) {
                        r.command += command[j];
                        continue;
                    }
                    
                    visited.insert(points[0]);
                    visited.insert(points[1]);
                    r.command += "ei!";
                } else {
                    r.command += command[j];
                }
            }
        }
        
        if (c != commands.size()) {
            r.command = "TOO LONG";
            r.score = 0;
            return r;
        }
        
        vector <int> used(phraseNum, 0);
        
        for (int i = 0; i < r.command.size(); i++) {
            for (int j = 0; j < phraseNum; j++) {
                bool flag = true;
                
                if (i + phrase[j].size() > r.command.size()) continue;
                
                for (int k = 0; k < phrase[j].size(); k++) {
                    if (r.command[i + k] != phrase[j][k]) {
                        flag = false;
                        break;
                    }
                }
                
                if (flag) {
                    if (used[j] == 0) r.score += 300;
                    used[j]++;
                    r.score += phrase[j].size() * 2;
                }
            }
        }
        
        return r;
    }
};

const int Calc::phraseNum = 2;
const string Calc::phrase[2] = {"ei!", "ia! ia!"};
