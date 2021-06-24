#pragma once

#include <iostream>
#include <vector>
#include <ctime>
#include <map>
#include <cassert>
#include <algorithm>
#include <unordered_map>

#define inf 0x3f3f3f3f
#define pii pair<int,int>
#define piii pair<pii, int>
#define mkp(a, b) make_pair(a, b)

#define db1(a) cerr << #a << "=" << (a) << endl;
#define db2(a,b) cerr << #a << "=" << (a) << " " << #b << "=" << (b) << endl;
#define db3(a,b,c) cerr << #a << "=" << (a) <<" "<< #b << "=" << (b) << " " << #c << "=" << (c) << endl;

using namespace std;

enum {
	/*局部搜索参数*/
	TabuSearchSteps = 1000,
	NoDecreaseLimit = 1000,

	/*禁忌表*/
	DoubleTabu = true,	//双禁忌效果更好
};

struct Rander {
	Rander(unsigned _seed = 0) {
		srand(_seed ? _seed : (unsigned)time(NULL));
	}
	int getRandNumber(int _l = -1, int _r = -1) {
		if (_l < 0 || _r < 0) {
			return rand();
		}
		if (_l < 0 || _r < _l) {
			cout << "RandomNumber Error!\n";
			exit(-1);
		}
		return rand() % (_r - _l + 1) + _l;
	}
};

struct Edge {
	int dist, from, to;
	Edge() {}
	Edge(int d, int f, int t) {
		dist = d; from = f; to = t;
	}
	void output() {
		cout << "(" << from << " -> " << to << ", dist=" << dist << ") ";
	}
	bool operator < (const Edge& E) const {
		return dist < E.dist || (dist == E.dist && to < E.to);
	}
};

struct TabuTable {
	int R, tableLen;
	bool doubleTabu;
	int vNum, pNum;		//For double Tabu
	vector<int>serviceTabu;
	vector<int>costomerTabu;
	vector<vector<int>>tabuTable;

	TabuTable(int _R = 10, int _vNum = 1, int _pNum = 1) {
		R = _R; tableLen = _vNum;
		doubleTabu = DoubleTabu;
		vNum = _vNum; pNum = _pNum;
		serviceTabu = vector<int>(tableLen, 0);		//成为服务节点的禁忌步长
		costomerTabu = vector<int>(tableLen, 0);		//...
		tabuTable = vector<vector<int>>(tableLen, vector<int>(tableLen, 0));
	}

	void clear() {
		serviceTabu = vector<int>(tableLen, 0);
		costomerTabu = vector<int>(tableLen, 0);
		tabuTable = vector<vector<int>>(tableLen, vector<int>(tableLen, 0));
	}

	bool isTabu(int u, int v, int nowStep) {	//u想加入成为服务节点
		if (doubleTabu) {
			return nowStep < serviceTabu[v] || nowStep < costomerTabu[u];
		}
		else {
			return nowStep < tabuTable[u][v];	//应该是不能变回<v, u>?
		}

	}

	void updateTabuTable(int u, int v, int nowStep, Rander rander) {	//u加入成为服务节点
		if (doubleTabu) {
			int alpha = nowStep + pNum / 10 + rander.getRandNumber() % 10;				//u alpha步之内不能再变回为普通节点
			int beta = nowStep + (vNum - pNum) / 10 + rander.getRandNumber() % 100;		//反之v Bate步之内不能成为服务节点
			costomerTabu[u] = alpha;
			serviceTabu[v] = beta;
		}
		else {
			//tabuTable[u][v] = nowStep + R + rander.getRandNumber(1, R+1);		//<u, v>被禁忌
			//tabuTable[u][v] = nowStep + pNum * (vNum - pNum) / 100 + rander.getRandNumber(0, 10 * pNum);
			tabuTable[u][v] = nowStep + 1;
		}
	}
};

struct myVector {
	vector<int>values;
	unordered_map<int, int>indexes;

	void insert(int value) {
		if (indexes.count(value)) {
			printf("Value %d already exists.\n", value);
			return;
		}
		values.push_back(value);
		indexes[value] = (int)values.size() - 1;
	}

	void remove(int value) {
		/*
		A B (C-idx) D E F G (H-last)
		*/
		if (!indexes.count(value)) {
			printf("Value %d not exists.\n", value);
			return;
		}
		int idx = indexes[value];
		if (idx == values.size() - 1) {	//最后一个
			values.pop_back();
			indexes.erase(value);
		}
		else {
			values[idx] = values[values.size() - 1];
			indexes.erase(value);
			indexes[values[idx]] = idx;
			values.pop_back();
		}
	}

	bool exist(int value) {
		return indexes.count(value);
	}

	int size() {
		return (int)values.size();
	}
};

struct VWTSSolution : public myVector {int Minimum, radiu, UXSize;};

extern Rander rander;
extern map<string, int>baselines;
extern vector<vector<string>>instPmed;