// VWTS.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "VWTS.h"

vector<vector<int>>graph = {
	{0,2,4,3,1},
	{2,0,2,4,6},
	{4,2,0,3,5},
	{3,4,3,0,4},
	{1,6,5,4,0},
};

int main() {
	int nodeNum=5, centerNum=2, bestResult=inf;
	char instRoad[128]; int test_data = 5;
	for (int tp = 4; tp < min((int)instPmed.size(), test_data); ++tp) {
		for (int i = 0; i < instPmed[tp].size(); ++i) {
			string inst = "../Instance/" + instPmed[tp][i] + ".txt";
			int randomSeed = unsigned(rand()*rand()*rand());

			#pragma warning(disable:4996)
			strcpy(instRoad, inst.c_str());
			freopen(instRoad, "r", stdin);
			scanf("%d %d %d", &nodeNum, &centerNum, &bestResult);
			bestResult = baselines[instPmed[tp][i]];
			//db3(nodeNum, centerNum, bestResult);
			graph.resize(nodeNum);
			for (int x = 0; x < nodeNum; ++x) {
				graph[x].resize(nodeNum);
				for (int y = 0; y < nodeNum; ++y) {
					scanf("%d", &graph[x][y]);
				}
			}
			fclose(stdin);

			printf("#######################################################\n");
			printf("%s, Seed: %u\n", instRoad, randomSeed);
			VWTS::VWTSSolver(graph, centerNum, randomSeed, bestResult);
			printf("=======================================================\n\n");
		}
	}
	return 0;
}