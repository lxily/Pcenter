#include "VWTS.h"
#include "commom.h"

VWTSSolution VWTS::VWTSSolver(const vector<vector<int>> &graph, int pNum, unsigned randomSeed, int bestResult){
	db3(pNum, randomSeed, bestResult);
	clock_t start_t = clock();

	vector<int>radius;
	VWTSSolution ret; ret.radiu = inf;
	rander = Rander(randomSeed);
	for (int i = 0; i < graph.size(); ++i) {
		for (int j = 0; j < graph.size(); ++j) {
			radius.push_back(graph[i][j]);
		}
	}
	sort(radius.begin(), radius.end());
	radius.erase(unique(radius.begin(), radius.end()), radius.end());

	int idx = lower_bound(radius.begin(), radius.end(), bestResult) - radius.begin();
	int low = max(0, idx);
	for (int i = low + 50; i >= low; --i) {
		SetCovering SetCoveringSolver = SetCovering(graph, pNum, radius[i]);
		VWTSSolution sol = SetCoveringSolver.solve(200000, randomSeed);
		db3(i, radius[i], sol.UXSize);
		if (sol.UXSize == 0) {
			ret = sol; ret.radiu = calculationResult(graph, ret);
			if (ret.radiu <= bestResult) break;
		}
		/*else if (sol.UXSize == 1) {
			int nc = SetCoveringSolver.UX.values[0];
			printf("No Covered Point: %d\n", nc);
			printf("CSet: "); for (auto p : SetCoveringSolver.CSet[nc])printf("%d ", p); printf("\n");
		}*/
	}
	//ret.radiu = calculationResult(graph, ret);

	//二分半径
	//int l = 0, r = (int)radius.size() - 1;
	//while (l < r) {
	//	int m = (l + r) >> 1;
	//	SetCovering SetCoveringSolver = SetCovering(graph, pNum, radius[m]);
	//	VWTSSolution sol = SetCoveringSolver.solve(5000, randomSeed);
	//	/*
	//	for (int i = 0; i < SetCoveringSolver.setCnt; ++i) {
	//		printf("(%d, %d) ", SetCoveringSolver.WValue[i], SetCoveringSolver.QValue[i]);
	//	}
	//	cout << endl;
	//	*/
	//	if (sol.UXSize == 0) {
	//		r = m;
	//		ret = sol; 
	//		ret.radiu = radius[m];
	//	}
	//	else {
	//		l = m + 1;
	//	}
	//	if (ret.radiu <= bestResult)break;
	//}

	cout << "存在最优解: " << bestResult << endl;
	cout << "找到最优解: " << ret.radiu << endl;
	
	clock_t end_t = clock();
	double endtime = (double)(end_t - start_t) / CLOCKS_PER_SEC;
	cout << "总耗时:" << endtime << "s" << endl;        //s为单位
	return ret;
}

int VWTS::calculationResult(const vector<vector<int>> &graph, const VWTSSolution &sol) {
	//最终在半径r下的解决方案，其最优覆盖结果不一定是r
	int ret = 0;
	for (int i = 0; i < graph.size(); ++i) {
		int minDist = inf;
		for (auto p : sol.values) {
			minDist = min(minDist, graph[i][p]);
		}
		ret = max(ret, minDist);
	}
	return ret;
}
