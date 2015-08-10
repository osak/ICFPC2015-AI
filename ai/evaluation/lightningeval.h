#pragma once

#include "../util.h"
#include "../common.h"
#include "../model/board.h"
#include "../model/unit.h"

using namespace std;

class LightningEval{
	int H, W, maxUnitSize, maxUnitVolume, sticklen;
	bool bigHeight, bigWidth;
	bool safeUnits, stickUnits;
	vector<Unit> units;

	int kawateaScore(const vector <BitRow> &f);
	int heightScore(const vector <BitRow> &f);
	int dangerScore(const vector <BitRow> &f);
	int oneUnitScore(const vector <BitRow> &f);
	int chanceScore(const vector <BitRow> &f, int leftTurn);
	int cornerScore(const vector <BitRow> &f);
	int offensiveCalc(const vector <BitRow> &f);
	int kadoScore(const vector <BitRow> &f);
	int buddhaScore(const vector <BitRow> &f);
	int messiahScore(const vector <BitRow> &f);
public:

	// àÍî‘ïùÇ™ã∑Ç≠Ç»ÇÈÇÊÇ§Ç…(ïùÅAçÇÇ≥)Çï‘Ç∑
	Point getUnitSize(const Unit &unit){
		Point p;
		p.x = 1e9, p.y = 1e9;
		for (int i = 0; i < 6; ++i) {
			int minx = 1e9, maxx = -1e9;
			int miny = 1e9, maxy = -1e9;
			for (auto &p : unit.member) {
				minx = min(minx, p.x);
				maxx = max(maxx, p.x);
				miny = min(miny, p.y);
				maxy = max(maxy, p.y);
			}
			int a = maxx - minx, b = maxy - miny;
			Point pp;
			pp.x = min(a, b) + 1, pp.y = max(a, b) + 1;
			p = min(p, pp);
		}
		return p;
	}


	void checkUnits(){
		maxUnitSize = 0, maxUnitVolume = 0, sticklen = 0;
		for (auto &u : units) {
			auto p = getUnitSize(u);
			maxUnitSize = max(maxUnitSize, (int)u.member.size());
			maxUnitVolume = max(maxUnitVolume, p.x*p.y);
			if (p.x == 1 && p.y >= 3) stickUnits = true, sticklen = max(sticklen, p.y);
		}
		safeUnits = maxUnitSize <= 4 && maxUnitVolume <= 8;
	}
	int calcMaster(Board &board, Board &nextBoard, int num);
	int calcGod(Board &board, Board &nextBoard, int num);
	int calcBuddha(Board &board, Board &nextBoard, int num);
	int calcRand(Board &f, Board &nextBoard, int num);
	int calcKawatea(Board &f, Board &nextBoard, int num);
	int calcDangerChance(Board &f, Board &nextBoard, int num);
	int calcHole(Board &f, Board &nextBoard, int num);

public:

	LightningEval(int H, int W, vector<Unit> &units) : H(H), W(W), units(units){
		bigHeight = H >= 15, bigWidth = W >= 15;
		checkUnits();
	}
};


