#include "GRASP.h"

Rander rander;

PCenterSolver::PCenterSolver(vector<vector<int>> g, int p) {
	vNum = (int)g.size();
	pNum = p;
	graph = g;
	isCenter = vector<bool>(vNum, false);
	tabuTable = TabuTable(15, vNum, pNum);
	historyOptimalValue = inf;
}

void PCenterSolver::init(bool randomSolution) {
	minDistEdge = vector<vector<Edge>>(vNum);
	kValue = vector<vector<int>>(vNum, vector<int>(vNum, 0));
	for (int i = 0; i < vNum; ++i) {
		for (int j = 0; j < vNum; ++j) {
			minDistEdge[i].push_back(Edge(graph[i][j], i, j));
		}
		sort(minDistEdge[i].begin(), minDistEdge[i].end());
		for (int preV = -1, j = 0; j < vNum; ++j) {
			if (minDistEdge[i][j].dist == preV) {
				kValue[i][minDistEdge[i][j].to] = kValue[i][minDistEdge[i][j - 1].to];
			}
			else {
				kValue[i][minDistEdge[i][j].to] = j;
			}
			preV = minDistEdge[i][j].dist;
		}
	}

	if (randomSolution) {
		vector<bool>used(vNum, false);
		centers = vector<int>();
		while (centers.size() < pNum) {
			int point = rander.getRandNumber(0, vNum - 1);
			if (used[point]) continue;
			used[point] = true;
			isCenter[point] = true;
			centers.push_back(point);
		}
	}
	else {
		centers = getInitSolution();
	}
	minMaximum = initFDTable(centers);
	upDateHistoryOptimal();
}

Solution PCenterSolver::getInitSolution() {
	//第一个点随机
	int firstNode = rander.getRandNumber(0, vNum - 1);
	Solution ret(1, firstNode);
	isCenter[firstNode] = true;
	//其余点贪心构造
	for (int t = 1; t < pNum; ++t) {
		vector<pii>maxDistPoints = getMaxDistPointsFromSolution(ret);
		int chosenIndex = rander.getRandNumber(0, (int)maxDistPoints.size() - 1);
		int serverPoint = maxDistPoints[chosenIndex].first;	//<server, chosen>
		int chosenPoint = maxDistPoints[chosenIndex].second;

		int nextPoint = -1;
		if (rander.getRandNumber(0, 100) <= RandomAlphaInInit) {
			int k = kValue[chosenPoint][serverPoint];
			int nextChosenIndex = rander.getRandNumber(0, k - 1);
			nextPoint = minDistEdge[chosenPoint][nextChosenIndex].to;
		}
		else {
			while (nextPoint == -1 || isCenter[nextPoint]) {
				int nextChosenIndex = rander.getRandNumber(0, vNum - 1);
				nextPoint = minDistEdge[chosenPoint][nextChosenIndex].to;
			}
		}
		ret.push_back(nextPoint);
		isCenter[nextPoint] = true;
	}
	//printSolution(ret);
	return ret;
}

vector<pii> PCenterSolver::getMaxDistPointsFromSolution(const Solution &sol)
{
	vector<pii>ret;
	int maxDist = 0;
	for (int u = 0; u < vNum; ++u) {
		if (isCenter[u])continue;
		int minDist = inf;
		int servePoint = -1;
		for (int i = 0; i < sol.size(); ++i) {
			if (minDist > graph[sol[i]][u]) {
				minDist = graph[sol[i]][u];
				servePoint = sol[i];
			}
		}
		if (maxDist < minDist) {
			ret.clear();
			maxDist = minDist;
		}
		if (maxDist == minDist) {
			ret.push_back(mkp(servePoint, u));
		}
	}
	return ret;
}

vector<pii> PCenterSolver::getMaxDistPointsFromSolution() {
	vector<pii>ret;
	int maxDist = 0;
	for (int u = 0; u < vNum; ++u) {
		if (isCenter[u])continue;
		int minDist = DTable[u].first;
		int servePoint = FTable[u].first;
		if (maxDist < minDist) {
			ret.clear();
			maxDist = minDist;
		}
		if (maxDist == minDist) {
			ret.push_back(mkp(servePoint, u));
		}
	}
	return ret;
}

vector<pii> PCenterSolver::getNeighbourhoodsBySolution(const Solution & sol) {
	vector<pii> ret;
	vector<pii> maxDistPoints = getMaxDistPointsFromSolution();
	int chosenIndex = rander.getRandNumber(0, (int)maxDistPoints.size() - 1);
	int serverPoint = maxDistPoints[chosenIndex].first;	//<server, chosen>
	int chosenPoint = maxDistPoints[chosenIndex].second;
	int k = kValue[chosenPoint][serverPoint];

	for (int i = 0; i < k; ++i) {
		for (int j = 0; j < sol.size(); ++j) {
			ret.push_back(mkp(minDistEdge[chosenPoint][i].to, sol[j]));
		}
	}

	return ret;
}

vector<int> PCenterSolver::neighbourhoodsEvaluation(const vector<pii>& Neighbourhoods) {
	vector<int>ret;
	vector<pii> F = FTable;
	vector<pii> D = DTable;
	for (int i = 0; i < Neighbourhoods.size(); ++i) {
		int addPoint = Neighbourhoods[i].first;
		int removePoint = Neighbourhoods[i].second;
		int Sc = addPointToSolution(addPoint);
		int Mf = resultOfRemovePoint(removePoint);
		ret.push_back(max(Sc, Mf));

		FTable.assign(F.begin(), F.end());
		DTable.assign(D.begin(), D.end());
	}
	return ret;
}

vector<piii> PCenterSolver::getNeighbourhoodsAndEvaluation(const Solution & sol) {
	vector<piii> ret;
	vector<pii> F = FTable;
	vector<pii> D = DTable;
	vector<pii> maxDistPoints = getMaxDistPointsFromSolution();
	int chosenIndex = rander.getRandNumber(0, (int)maxDistPoints.size() - 1);
	int serverPoint = maxDistPoints[chosenIndex].first;	//<server, chosen>
	int chosenPoint = maxDistPoints[chosenIndex].second;
	int k = (rander.getRandNumber(0, 100) <= RandomAlphaInSearch) ?
		kValue[chosenPoint][serverPoint] : vNum;

	int minIncrease = inf;
	for (int i = 0; i < k; ++i) {
		int addPoint = minDistEdge[chosenPoint][i].to;
		if (isCenter[addPoint])continue;
		int Sc = addPointToSolution(addPoint);
		vector<int> Mf(vNum, 0);
		for (int j = 0; j < vNum; ++j) {
			int removePoint = FTable[j].first;		//删除j的老大
			if (removePoint == addPoint) continue;	//不考虑删除刚刚加入的服务点
			int dist = min(graph[addPoint][j], DTable[j].second);	//j的新老大的距离
			if (Mf[removePoint] < dist) {
				Mf[removePoint] = dist;
			}
		}

		for (int j = 0; j < pNum; ++j) {
			int removePoint = sol[j];
			int result = max(Mf[removePoint], Sc);
			if (minIncrease > result) {
				minIncrease = result;
				ret.clear();
			}
			if (minIncrease == result) {
				ret.push_back(mkp(mkp(addPoint, removePoint), result));
			}
		}
		FTable.assign(F.begin(), F.end());
		DTable.assign(D.begin(), D.end());
	}
	return ret;
}

int PCenterSolver::initFDTable(const Solution & sol) {
	FTable.clear();
	DTable.clear();

	int _minMaximum = 0;
	for (int u = 0; u < vNum; ++u) {
		int p1 = -1, p2 = -1;
		int d1 = -1, d2 = -1;
		for (int i = 0; i < vNum; ++i) {
			int v = minDistEdge[u][i].to;
			if (!isCenter[v])continue;		//v不是服务节点
			if (d1 == -1) {					//最近的服务节点
				p1 = minDistEdge[u][i].to;
				d1 = minDistEdge[u][i].dist;
			}
			else if (d2 == -1) {			//次近的服务节点
				p2 = minDistEdge[u][i].to;
				d2 = minDistEdge[u][i].dist;
				break;
			}
		}
		FTable.push_back(mkp(p1, p2));
		DTable.push_back(mkp(d1, d2));

		if (_minMaximum < d1) {
			_minMaximum = d1;
		}
	}
	/*if (calc(centers) != _minMaximum) {
		cout << "初始化FDTable错误!!!\n";
		db2(calc(centers), _minMaximum);
		printSolution(centers);
		cout << "centers: \n";
		for (int i = 0; i < vNum; i++) {
			if (isCenter[i]) {
				cout << i << '\n';
			}
		}cout << endl;
		exit(0);
	}*/
	//cout << "(initFTTable)初始解: " << minMaximum << endl;
	return _minMaximum;
}

int PCenterSolver::addPointToSolution(int addPoint) {
	int Sc = 0;
	for (int i = 0; i < vNum; ++i) {
		const int &d = graph[addPoint][i];
		if (d < DTable[i].first) {
			DTable[i].second = DTable[i].first;
			DTable[i].first = d;
			FTable[i].second = FTable[i].first;
			FTable[i].first = addPoint;
		}
		else if (d < DTable[i].second) {
			DTable[i].second = d;
			FTable[i].second = addPoint;
		}
		if (Sc < DTable[i].first) {
			Sc = DTable[i].first;
		}
	}
	return Sc;
}

int PCenterSolver::removePointFromSolution(int removePoint) {
	/*
	调用前需要注意的是必须已经从解中删掉了removePoint，但是没有修改FDTable
	*/
	int Mf = 0;
	for (int i = 0; i < vNum; ++i) {
		if (FTable[i].first == removePoint ||
			FTable[i].second == removePoint) {
			if (FTable[i].first == removePoint) {	//老大被干掉了
				FTable[i].first = FTable[i].second;
				DTable[i].first = DTable[i].second;
			}
			//找新的老二
			int secondPoint = -1;
			for (int minDist = inf, j = 0; j < pNum; j++) {
				if (centers[j] == FTable[i].first) continue;
				if (minDist > graph[i][centers[j]]) {
					minDist = graph[i][centers[j]];
					secondPoint = centers[j];
				}
			}
			FTable[i].second = secondPoint;
			DTable[i].second = graph[i][secondPoint];
		}
		if (Mf < DTable[i].first) {
			Mf = DTable[i].first;
		}
	}
	return Mf;
}

int PCenterSolver::resultOfRemovePoint(int removePoint) {
	int Mf = 0;
	for (int minDist, i = 0; i < vNum; i++) {
		minDist = FTable[i].first == removePoint ? DTable[i].second : DTable[i].first;
		if (Mf < minDist)Mf = minDist;
	}
	return Mf;
}

void PCenterSolver::swapPairs(int addPoint, int removePoint) {
	isCenter[addPoint] = true;
	isCenter[removePoint] = false;
	for (int i = 0; i < centers.size(); ++i) {
		if (centers[i] == removePoint) {
			centers[i] = addPoint; break;
		}
	}
}

void PCenterSolver::upDateHistoryOptimal() {
	if (historyOptimalValue > minMaximum) {		//记录历史最优
		historyOptimalValue = minMaximum;
		historyOptimalCenters = centers;
		/*if (calc(centers) != historyOptimalValue) {
			cout << "更新历史最优值错误!!!\n";
			exit(0);
		}*/
	}
}

void PCenterSolver::upDateHistoryOptimal(const PCenterSolver & sol) {
	/*if (calculateResultByForce(sol.historyOptimalCenters) != sol.historyOptimalValue) {
		cout << "upDateHistoryOptimal: 传入解非法!!!\n";
		exit(0);
	}*/
	if (historyOptimalValue > sol.historyOptimalValue) {
		historyOptimalValue = sol.historyOptimalValue;
		historyOptimalCenters = sol.historyOptimalCenters;
	}
}

int PCenterSolver::searchOneStep(int nowStep) {
	//vector<pii> neighbourhoods = getNeighbourhoodsBySolution(centers);	//邻域
	//vector<int> evaluation = neighbourhoodsEvaluation(neighbourhoods);	//领域对应的评估
	vector<piii> minNeighbourhoodsAndEvaluation = getNeighbourhoodsAndEvaluation(centers);	//<<u,v>, result>

	if (!minNeighbourhoodsAndEvaluation.size()) {
		cout << "不存在可行邻域!\n";
		return inf;
	}

	int bestResult = minNeighbourhoodsAndEvaluation[0].second;
	vector<int>indexesOfNotTabu;
	vector<int>indexesOfInTabu;

	for (int i = 0; i < minNeighbourhoodsAndEvaluation.size(); ++i) {
		int res = minNeighbourhoodsAndEvaluation[i].second;
		int add = minNeighbourhoodsAndEvaluation[i].first.first;
		int sub = minNeighbourhoodsAndEvaluation[i].first.second;
		if (!tabuTable.isTabu(add, sub, nowStep)) {	//非禁忌的操作
			indexesOfNotTabu.push_back(i);
		}
		else {	//禁忌的操作
			indexesOfInTabu.push_back(i);
		}
	}

	int adoptIndex = -1;

	if (indexesOfNotTabu.size()) {			//存在非禁忌最优解
		adoptIndex = indexesOfNotTabu[rander.getRandNumber(0, (int)indexesOfNotTabu.size() - 1)];
	}
	else if (indexesOfInTabu.size()) {	//特殊解禁
		adoptIndex = indexesOfInTabu[rander.getRandNumber(0, (int)indexesOfInTabu.size() - 1)];
	}
	//else if (bestResult < historyOptimalValue) {	//特殊解禁
	//	adoptIndex = indexesOfInTabu[rander.getRandNumber(0, (int)indexesOfInTabu.size() - 1)];
	//}

	if (adoptIndex == -1) {
		//cout << "Step " << nowStep << ": No more solution!" << endl;
		return inf;
	}

	//cout << "Step " << nowStep << ": (Best solution | " << minMaximum << " -> " << bestResult << ")" << endl;
	int addPoint = minNeighbourhoodsAndEvaluation[adoptIndex].first.first;
	int removePoint = minNeighbourhoodsAndEvaluation[adoptIndex].first.second;

	swapPairs(addPoint, removePoint);
	int sc = addPointToSolution(addPoint);			//仅修改FDTable
	int mf = removePointFromSolution(removePoint);	//仅修改FDTable
	minMaximum = max(sc, mf);
	//minMaximum = initFDTable(centers);			//!!!更新FDTable
	if (minMaximum != bestResult) {
		cout << "Wrong Calculation!!!\n";
		db3(minMaximum, bestResult, max(sc, mf));
		exit(0);
	}

	upDateHistoryOptimal();						//记录历史最优
	tabuTable.updateTabuTable(addPoint, removePoint, nowStep, rander); //更新禁忌表

	return sc;
}

void PCenterSolver::solveWithNSearch(int steps) {
	int minSc = inf, count = 0;
	//int  noDecreaseLimit = vNum / 10;
	for (int i = 0; i < steps; ++i) {
		int newSc = searchOneStep(i);
		if (minSc > newSc) {
			count = 0;
			minSc = newSc;
		}
		else {
			count++;
		}
		if (count > NoDecreaseLimit) {
			break;
		}
	}
}

void PCenterSolver::changeToOptimalSolution() {
	isCenter = vector<bool>(vNum, false);
	centers = historyOptimalCenters;
	for (int i = 0; i < pNum; ++i) {
		isCenter[centers[i]] = true;
	}
	minMaximum = initFDTable(centers);
	tabuTable.clear();
}

int PCenterSolver::calculateResultByForce(const Solution & sol) {
	int ret = 0;
	for (int i = 0; i < vNum; ++i) {
		int _min = inf;
		for (int j = 0; j < sol.size(); ++j) {
			if (_min > graph[i][sol[j]]) {
				_min = graph[i][sol[j]];
			}
		}
		if (ret < _min) {
			ret = _min;
		}
	}
	return ret;
}

bool PCenterSolver::operator==(const PCenterSolver & sol) const {
	for (int i = 0; i < vNum; ++i) {
		if (isCenter[i] != sol.isCenter[i]) {
			return false;
		}
	}
	return true;
}

void PCenterSolver::printMinDistEdge() {
	cout << "printMinDistEdge:\n";
	for (int i = 0; i < vNum; ++i) {
		for (int j = 0; j < vNum; ++j) {
			minDistEdge[i][j].output();
		}
		cout << '\n';
	}
}

void PCenterSolver::printKValue() {
	cout << "printKValue:\n";
	for (int i = 0; i < vNum; ++i) {
		for (int j = 0; j < vNum; ++j) {
			cout << kValue[i][j] << ' ';
		}
		cout << '\n';
	}
}

void PCenterSolver::printSolution(const Solution & sol) {
	cout << "Solution: ";
	for (int i = 0; i < sol.size(); ++i) {
		cout << sol[i] << ' ';
	}
	cout << '\n';
}

void PCenterSolver::printFTable() {
	cout << "printFTable:\n";
	for (int i = 0; i < FTable.size(); ++i) {
		cout << i << " " << FTable[i].first << " " << FTable[i].second << endl;
	}
}

void PCenterSolver::printDTable() {
	cout << "printDTable:\n";
	for (int i = 0; i < DTable.size(); ++i) {
		cout << i << " " << DTable[i].first << " " << DTable[i].second << endl;
	}
}

vector<bool> PathRellinking::pointsAppear(const vector<int>& points, int vNum) {
	vector<bool> ret(vNum, false);
	for (int i = 0; i < points.size(); ++i) {
		ret[points[i]] = true;
	}
	return ret;
}

vector<int> PathRellinking::intersectionOfSolution(const PCenterSolver& Sc, const PCenterSolver& Se) {
	vector<int>ret;
	vector<bool>appear = pointsAppear(Sc.centers, Sc.vNum);
	for (int i = 0; i < Se.centers.size(); ++i) {
		if (appear[Se.centers[i]]) {
			ret.push_back(Se.centers[i]);
		}
	}
	return ret;
}

vector<int> PathRellinking::removePointsFromSolution(const PCenterSolver& sol, const vector<int>& removePoints) {
	vector<bool> appear = pointsAppear(removePoints, sol.vNum);
	vector<int> ret;
	for (int i = 0; i < sol.centers.size(); ++i) {
		if (!appear[sol.centers[i]]) {
			ret.push_back(sol.centers[i]);
		}
	}
	return ret;
}

vector<int> PathRellinking::addPointsToSolution(const PCenterSolver& sol, const vector<int>& addPoints) {
	vector<int>ret = sol.centers;
	for (int i = 0; i < addPoints.size(); ++i) {
		ret.push_back(addPoints[i]);
	}
	return ret;
}

int PathRellinking::distOfTwoSolution(const PCenterSolver & Sc, const PCenterSolver & Se) {
	int samePoint = 0;
	for (int i = 0; i < Sc.vNum; ++i) {
		samePoint += Sc.isCenter[i] && Se.isCenter[i];
	}
	return Sc.pNum - samePoint;
}

bool PathRellinking::solverInPopulation(const PCenterSolver & sol, const vector<PCenterSolver>& P) {
	bool existing = false;
	for (int i = 0; i < P.size(); ++i) {
		existing |= (sol == P[i]);
	}
	return existing;
}

PCenterSolver PathRellinking::pathRellinkingCrossover(const PCenterSolver &Sc, const PCenterSolver &Se) {
	PCenterSolver Sr = Sc;
	vector<QRecoder> QSet;
	//保证交叉得到的解不会变差(记录历史最优)
	Sr.upDateHistoryOptimal(Se);
	Solution intersection = intersectionOfSolution(Sc, Se);
	Solution Sc1 = removePointsFromSolution(Sc, intersection);
	Solution Se1 = removePointsFromSolution(Se, intersection);

	int distScToSe = Sc.pNum - (int)intersection.size();

	int iterTimes = distScToSe * DistBeta / 100;
	//int PRBeta = distScToSe;

	for (int _ = 0; _ < iterTimes; ++_) {
		int minIncrease = inf;
		vector<pii> F = Sr.FTable;
		vector<pii> D = Sr.DTable;
		for (int i = 0; i < Se1.size(); ++i) {
			int addPoint = Se1[i];
			int sc = Sr.addPointToSolution(addPoint);

			vector<int> Mf(Sr.vNum, 0);
			for (int j = 0; j < Sr.vNum; ++j) {
				int removePoint = Sr.FTable[j].first;	//删除j的老大
				if (removePoint == addPoint) continue;	//不考虑删除刚刚加入的服务点
				int dist = min(Sr.graph[addPoint][j], Sr.DTable[j].second);	//j的新老大的距离
				if (Mf[removePoint] < dist) {
					Mf[removePoint] = dist;
				}
			}
			for (int j = 0; j < Sc1.size(); ++j) {
				int removePoint = Sc1[j];
				int result = max(Mf[removePoint], sc);
				if (minIncrease > result) {
					minIncrease = result;
					QSet.clear();
				}
				if (minIncrease == result) {
					QSet.push_back({ result, Sc1[j], j, Se1[i], i });
				}
			}
			Sr.FTable.assign(F.begin(), F.end());
			Sr.DTable.assign(D.begin(), D.end());
		}

		if (!QSet.size()) {
			cout << "No new solution is generated!\n";
			cout << "For times: " << _ << " Dist(Sc, Se):" << distScToSe << endl;
			continue;
		}
		int index = rander.getRandNumber(0, (int)QSet.size() - 1);

		//再用最优值实际更新Sr
		int addPoint = QSet[index].ve;
		int removePoint = QSet[index].vc;

		Sr.swapPairs(addPoint, removePoint);
		int sc = Sr.addPointToSolution(addPoint);
		int mf = Sr.removePointFromSolution(removePoint);
		Sr.minMaximum = max(sc, mf);
		Sr.upDateHistoryOptimal();							//记录历史最优

		Sc1[QSet[index].vci] = Sc1[Sc1.size() - 1];
		Se1[QSet[index].vei] = Se1[Se1.size() - 1];
		Sc1.pop_back();
		Se1.pop_back();

		QSet.clear();
	}

	Sr.tabuTable.clear();	//初始化新解Sr的禁忌表
	Sr.solveWithNSearch(Sc.vNum * 2);
	Sr.changeToOptimalSolution();
	return Sr;
}

PCenterSolver getAGreedySolution(vector<vector<int>>_graph, int pNum) {
	PCenterSolver sol(_graph, pNum);
	sol.init(Greedy);
	return sol;
}
PCenterSolver getARandomSolution(vector<vector<int>>_graph, int pNum) {
	PCenterSolver sol(_graph, pNum);
	sol.init(Random);
	return sol;
}
PCenterSolver getAPCenterSolver(vector<vector<int>>_graph, int pNum, bool random) {
	if (random) return getARandomSolution(_graph, pNum);
	else		return getAGreedySolution(_graph, pNum);
}

Solution PathRellinking::GRASP(vector<vector<int>>_graph, int pNum, unsigned _randSeed, int opt) {
	clock_t start_t = clock();

	rander = Rander(_randSeed);
	//复杂度O(常数巨大 * maxIterations * tabuSearchSteps * ((N_k + p) * n)) ;
	vector<PCenterSolver> P;
	int optFound = inf;
	for (int _ = 0; _ < MaxIterations; ++_) {
		PCenterSolver St = getAPCenterSolver(_graph, pNum, Greedy);
		St.solveWithNSearch(TabuSearchSteps);
		St.changeToOptimalSolution();

		if ((int)P.size() == PopulationSize) {
			PCenterSolver Se = P[rander.getRandNumber(0, (int)P.size() - 1)];
			PCenterSolver Sr = pathRellinkingCrossover(St, Se);

			//cout << "Iter " << _ << ": "; db3(St.historyOptimalValue, Se.historyOptimalValue, Sr.historyOptimalValue);

			vector<int>indexOfGreaterSr;
			for (int i = 0; i < PopulationSize; ++i) {
				if (Sr.minMaximum <= P[i].minMaximum) {
					indexOfGreaterSr.push_back(i);
				}
			}

			if (!solverInPopulation(Sr, P) && indexOfGreaterSr.size()) {
				int minDist = inf;
				vector<int>indexOfMinDist;
				for (int i = 0; i < indexOfGreaterSr.size(); ++i) {
					int idx = indexOfGreaterSr[i];
					int dist = distOfTwoSolution(Sr, P[idx]);
					if (minDist > dist) {
						minDist = dist;
						indexOfMinDist.clear();
					}
					if (minDist == dist) {
						indexOfMinDist.push_back(idx);
					}
				}

				int randomSlectIndex = rander.getRandNumber(0, (int)indexOfMinDist.size() - 1);

				/*Sr.tabuTable.clear()
				Sr.solveWithNSearch(TabuSearchSteps / 10);
				Sr.changeToOptimalSolution();*/

				P[indexOfMinDist[randomSlectIndex]] = Sr;

				if (optFound > Sr.minMaximum) {
					optFound = Sr.minMaximum;
				}
				if (optFound <= opt) {
					cout << "Answers from path-rellinking crossover.\n";
				}

			}
		}
		else if (!solverInPopulation(St, P)) {
			P.push_back(St);
			optFound = min(optFound, St.minMaximum);
			if (optFound <= opt) {
				cout << "Answers from local search.\n";
			}
		}

		if (optFound <= opt) {
			cout << "The optimal solution is obtained after " << _ << " iterations.\n";
			break;
		}
	}

	int minDist = inf, index = -1;
	for (int i = 0; i < P.size(); ++i) {
		if (minDist > P[i].minMaximum) {
			minDist = P[i].minMaximum;
			index = i;
		}
	}

	cout << "存在最优解: " << opt << endl;
	cout << "找到最优解: " << optFound << endl;
	clock_t end_t = clock();
	double endtime = (double)(end_t - start_t) / CLOCKS_PER_SEC;
	cout << "总耗时:" << endtime << "s" << endl;        //s为单位
	//cout << "Total time:" << endtime * 1000 << "ms" << endl;    //ms为单位

	return P[index].centers;
}


