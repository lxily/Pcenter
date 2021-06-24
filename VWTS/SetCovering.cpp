#include "SetCovering.h"

SetCovering::SetCovering(const vector<vector<int>> &graph, int slect, int radiu){
	setCnt = (int)graph.size();
	slectCnt = slect;
	UX = myVector();
	centers.Minimum = 0;
	centers.UXSize = setCnt;
	historyOptimalCenters.Minimum = inf;
	historyOptimalCenters.UXSize = setCnt;
	WValue = vector<int>(setCnt, 1);
	QValue = vector<int>(setCnt, 0);
	VSet = vector<vector<int>>(setCnt);
	CSet = vector<vector<int>>(setCnt);
	VSetCrossUX = vector<myVector>(setCnt);
	CSetCrossX = vector<myVector>(setCnt);
	tabuTable = TabuTable(10, setCnt, slectCnt);
	for (int i = 0; i < setCnt; ++i) {
		UX.insert(i);
		for (int j = 0; j < setCnt; ++j) {
			if (graph[i][j] <= radiu) {
				VSet[i].push_back(j);
				CSet[j].push_back(i);
				VSetCrossUX[i].insert(j);	//最初全部未被覆盖
			}
		}
	}
}

void SetCovering::init(){
	for (int i = 0; i < slectCnt; ++i) {
		vector<int>cIndexes;		//可选择的集合
		int maxUnCovertCnt = 0;
		for (int j = 0; j < setCnt; ++j) {
			if (centers.exist(j))continue;
			if (maxUnCovertCnt < VSetCrossUX[j].size()) {
				maxUnCovertCnt = VSetCrossUX[j].size();
				cIndexes.clear();
				cIndexes.push_back(j);
			}
			else if (maxUnCovertCnt == VSetCrossUX[j].size()) {
				cIndexes.push_back(j);
			}
		}
		int choosen = cIndexes[rander.getRandNumber(0, (int)cIndexes.size() - 1)];
		centers.insert(choosen);	//选择其作为服务节点
		for (auto p : VSet[choosen]) {	//choosen可覆盖的所有点p
			if (UX.exist(p)) {			//p原来未被覆盖
				UX.remove(p); 
				for (auto q : CSet[p]) {	//更新所有集合与未覆盖点集的交集
					VSetCrossUX[q].remove(p);
				}
			}
		}
		for (auto p : CSet[choosen]) {
			CSetCrossX[p].insert(choosen);
		}
	}
	centers.Minimum = 0;
	for (auto p : UX.values) {	//当前解下所有的未覆盖点计算最优值
		centers.Minimum += WValue[p];
	}
	centers.UXSize = UX.size();
	initQValue();		//初始化辅助值
	updataOptimal();	//更新历史最优
}

void SetCovering::updataOptimal() {
	if (historyOptimalCenters.UXSize > centers.UXSize) {
		historyOptimalCenters = centers;
	}
}

void SetCovering::initQValue(){
	QValue = vector<int>(setCnt, 0);
	for (int i = 0; i < setCnt; ++i) {
		if (centers.exist(i)) {	//i是中心节点，对应值为->所有被覆盖的点中，仅被i覆盖的点的权值和
			for (auto p : VSet[i]) {	//对于所有i可服务的节点p，若其只被覆盖一次，则说明p只被i覆盖
				if (CSetCrossX[p].size() == 1) {
					QValue[i] += WValue[p];
				}
			}
		}
		else {	//非中心节点i对应的值即为->i能覆盖但是仍未被覆盖的点的权值和
			for (auto p : VSetCrossUX[i].values) {	//即i的服务集合与未覆盖点集合的交集
				QValue[i] += WValue[p];
			}
		}
	}
}

piii SetCovering::findPair(int step){
	vector<piii>bestTabuSwap;
	vector<piii>bestNotTabuSwap;
	int bestTabuObj = inf;
	int bestNotTabuObj = inf;
	if (UX.size() == 0) {
		printf("No vertex uncorvered.\n"); return { { -1, -1 }, -1 };
	}
	int slt = UX.values[rander.getRandNumber(0, UX.size()-1)];
	vector<int>tmpQValue = QValue;
	for (auto i : CSet[slt]) {
		tryToOpenCenter(i);
		for (auto j : centers.values) {
			//tryToOpenCenter(i)不会改变QValue[i]的值，在这里一起计算即可
			int result = centers.Minimum - QValue[i] + QValue[j];
			if (tabuTable.isTabu(i, j, step)) {
				if (bestTabuObj > result) {
					bestTabuObj = result;
					bestTabuSwap.clear();
					bestTabuSwap.push_back({ { i, j }, result });
				}
				else if (bestTabuObj == result) {
					bestTabuSwap.push_back({ { i, j }, result });
				}
			}
			else {
				if (bestNotTabuObj > result) {
					bestNotTabuObj = result;
					bestNotTabuSwap.clear();
					bestNotTabuSwap.push_back({ { i, j }, result });
				}
				else if (bestNotTabuObj == result) {
					bestNotTabuSwap.push_back({ { i, j }, result });
				}
			}
			
		}
		QValue = tmpQValue;
	}

	piii ret = { { -1, -1 }, -inf };
	int tabuSz = (int)bestTabuSwap.size();
	int nTabuSz = (int)bestNotTabuSwap.size();

	/*db2(tabuSz, nTabuSz);
	db3(bestTabuObj, bestNotTabuObj, historyOptimalCenters.UXSize);*/

	if (!nTabuSz) {	//特殊解禁
		ret = bestTabuSwap[rander.getRandNumber(0, tabuSz - 1)];
	}
	else if (nTabuSz) {	//存在非禁忌最优解
		ret = bestNotTabuSwap[rander.getRandNumber(0, nTabuSz - 1)];
	}
	if (ret.second == -inf) {
		printf("No valid neighbor.\n"); return { { -1, -1 }, -1 };
	}
	return ret;
}

void SetCovering::tryToOpenCenter(int tryAdd){
	for (auto p : VSet[tryAdd]) {
		if (CSetCrossX[p].size() == 1) {
			int q = CSetCrossX[p].values[0];
			QValue[q] -= WValue[p];
		}
	}
}

void SetCovering::makeASwapMove(int addPoint, int removePoint){
	centers.Minimum -= QValue[addPoint];
	for (auto p : VSet[addPoint]) {
		if (CSetCrossX[p].size() == 1) {
			int q = CSetCrossX[p].values[0];
			QValue[q] -= WValue[p];
		}
		else if (CSetCrossX[p].size() == 0) {
			for (auto q : CSet[p]) {
				if (q != addPoint) QValue[q] -= WValue[p];
			}
		}
	}
	centers.Minimum += QValue[removePoint];

	centers.insert(addPoint);
	centers.remove(removePoint);
	//加入一个点对数据结果的影响
	for (auto p : CSet[addPoint]) {
		CSetCrossX[p].insert(addPoint);
	}
	for (auto p : VSet[addPoint]) {
		if (UX.exist(p)) {	//p原来未被覆盖
			UX.remove(p);
			for (auto q : CSet[p]) {	//更新所有集合与未覆盖点集的交集
				VSetCrossUX[q].remove(p);
			}
		}
	}
	//删除一个点对数据结果的影响
	for (auto p : CSet[removePoint]) {
		CSetCrossX[p].remove(removePoint);
	}
	for (auto p : VSet[removePoint]) {
		if (CSetCrossX[p].size() == 0) {	//删除v后，点p不能被覆盖
			UX.insert(p);
			for (auto q : CSet[p]) {	//更新所有集合与未覆盖点集的交集
				VSetCrossUX[q].insert(p);
			}
		}
	}

	for (auto p : VSet[removePoint]) {
		if (CSetCrossX[p].size() == 1) {
			int q = CSetCrossX[p].values[0];
			QValue[q] += WValue[p];
		}
		else if (CSetCrossX[p].size() == 0) {
			for (auto q : CSet[p]) {
				if (q != removePoint)QValue[q] += WValue[p];
			}
		}
	}

	centers.UXSize = UX.size();
}

void SetCovering::addTheUXNodeWeight(int point, int addValue){
	centers.Minimum += addValue;
	WValue[point] += addValue;
	for (auto p : CSet[point]) {
		QValue[p] += addValue;
	}
}

VWTSSolution SetCovering::solve(int maxTabuSearchSteps, unsigned randomSeed){
	init(); 
	if (historyOptimalCenters.UXSize == 0)
		return historyOptimalCenters;
	for (int step = 0; step < maxTabuSearchSteps; ++step) {
		int preUnCovered = UX.size();
		piii bestSwap = findPair(step);
		int addPoint = bestSwap.first.first;
		int removePoint = bestSwap.first.second;
		if (addPoint < 0) continue;
		makeASwapMove(addPoint, removePoint);
		//db2(bestSwap.first.first, bestSwap.first.second);
		//db2(bestSwap.second, centers.Minimum);
		if (historyOptimalCenters.UXSize > UX.size()) {
			historyOptimalCenters = centers;
		}
		if (UX.size() >= preUnCovered) {
			for (auto p : UX.values) {
				addTheUXNodeWeight(p, 10);
			}
		}
		tabuTable.updateTabuTable(addPoint,removePoint,step,rander);
		if (historyOptimalCenters.UXSize == 0) break;
	}
	return historyOptimalCenters;
}

