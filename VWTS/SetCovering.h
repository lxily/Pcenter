#pragma once

#include "heads.h"

struct SetCovering {
	int setCnt, slectCnt;
	vector<vector<int>>VSet;	//V[j]表示 点i可覆盖点 的集合
	vector<vector<int>>CSet;	//C[i]表示 可覆盖点i的 点的集合
	vector<int>WValue;	//加权值
	vector<int>QValue;	//中间辅助数据 
	vector<pii>CoveredTimes; //被覆盖的次数以及对应点(只记录一个)
	TabuTable tabuTable;	//	禁忌表

	myVector UX;	//未被覆盖的点集合
	vector<myVector>VSetCrossUX;	//维护V[i]与Ux的交集
	vector<myVector>CSetCrossX;	//维护C[i]与X的交集

	VWTSSolution centers;
	VWTSSolution historyOptimalCenters;

	SetCovering() {
		printf("Nothing Init!");
	}
	SetCovering(const vector<vector<int>>&graph, int slect, int radiu);	// 预处理
	void init();	//得到初始解
	void updataOptimal();
	void initQValue();
	piii findPair(int step);
	void tryToOpenCenter(int tryAdd);
	void makeASwapMove(int addPoint, int removePoint);
	void addTheUXNodeWeight(int point, int addValue);
	VWTSSolution solve(int maxTabuSearchSteps = TabuSearchSteps, unsigned randomSeed=0);

	void printResult() {
		printf("\n\n====================================\n");
		printf("Centers: "); for (int i = 0; i < centers.values.size(); ++i)printf("%d ", centers.values[i]); printf("\n");
		printf("VSet: \n");
		for (int i = 0; i < VSet.size(); ++i) {
			printf("VSet[%d]: ", i);
			for (int j = 0; j < VSet[i].size(); ++j) {
				printf("%d ", VSet[i][j]);
			}
			printf("\n");
		}
		printf("\nCSet: \n");
		for (int i = 0; i < CSet.size(); ++i) {
			printf("CSet[%d]: ", i);
			for (int j = 0; j < CSet[i].size(); ++j) {
				printf("%d ", CSet[i][j]);
			}
			printf("\n");
		}
		printf("\nUX: \n");
		for (int i = 0; i < UX.size(); ++i) {
			printf("%d ", UX.values[i]);
		}
		printf("\n");
		printf("\nVSetCrossUX: \n");
		for (int i = 0; i < VSetCrossUX.size(); ++i) {
			printf("VSetCrossUX[%d]: ", i);
			for (int j = 0; j < VSetCrossUX[i].size(); ++j) {
				printf("%d ", VSetCrossUX[i].values[j]);
			}
			printf("\n");
		}
		printf("\nCSetCrossX: \n");
		for (int i = 0; i < CSetCrossX.size(); ++i) {
			printf("CSetCrossX[%d]: ", i);
			for (int j = 0; j < CSetCrossX[i].size(); ++j) {
				printf("%d ", CSetCrossX[i].values[j]);
			}
			printf("\n");
		}
	}
};
