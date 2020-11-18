#pragma once

#include <iostream>
#include <vector>
#include <ctime>
#include <map>
#include <cassert>
#include <algorithm>

#define inf 0x3f3f3f3f
#define pii pair<int,int>
#define piii pair<pii, int>
#define mkp(a, b) make_pair(a, b)

#define db1(a) cerr << #a << "=" << (a) << endl;
#define db2(a,b) cerr << #a << "=" << (a) << " " << #b << "=" << (b) << endl;
#define db3(a,b,c) cerr << #a << "=" << (a) <<" "<< #b << "=" << (b) << " " << #c << "=" << (c) << endl;

using namespace std;

map<string, int>baselines = {
	{"pmed1.n100e198p5",127},{"pmed2.n100e193p10",98},
	{"pmed3.n100e198p10",93},{"pmed4.n100e196p20",74},
	{"pmed5.n100e196p33",48},{"pmed6.n200e786p5",84},
	{"pmed7.n200e779p10",64},{"pmed8.n200e792p20",55},
	{"pmed9.n200e785p40",37},{"pmed10.n200e786p67",20},
	{"pmed11.n300e1772p5",59},{"pmed12.n300e1758p10",51},
	{"pmed13.n300e1760p30",36},{"pmed14.n300e1771p60",26},
	{"pmed15.n300e1754p100",18},{"pmed16.n400e3153p5",47},
	{"pmed17.n400e3142p10",39},{"pmed18.n400e3134p40",28},
	{"pmed19.n400e3134p80",18},{"pmed20.n400e3144p133",13},
	{"pmed21.n500e4909p5",40},{"pmed22.n500e4896p10",38},
	{"pmed23.n500e4903p50",22},{"pmed24.n500e4914p100",15},
	{"pmed25.n500e4894p167",11},{"pmed26.n600e7069p5",38},
	{"pmed27.n600e7072p10",32},{"pmed28.n600e7054p60",18},
	{"pmed29.n600e7042p120",13},{"pmed30.n600e7042p200",9},
	{"pmed31.n700e9601p5",30},{"pmed32.n700e9584p10",29},
	{"pmed33.n700e9616p70",15},{"pmed34.n700e9585p140",11},
	{"pmed35.n800e12548p5",30},{"pmed36.n800e12560p10",27},
	{"pmed37.n800e12564p80",15},{"pmed38.n900e15898p5",29},
	{"pmed39.n900e15896p10",23},{"pmed40.n900e15879p90",13},
	{"tsp.u1060.p10",227308},{"tsp.u1060.p20",158080},
	{"tsp.u1060.p30",120777},{"tsp.u1060.p40",102056},
	{"tsp.u1060.p50",90492},{"tsp.u1060.p60",78117},
	{"tsp.u1060.p70",71075},{"tsp.u1060.p80",65216},
	{"tsp.u1060.p90",60787},{"tsp.u1060.p100",57001},
	{"tsp.u1060.p110",53884},{"tsp.u1060.p120",51027},
	{"tsp.u1060.p130",49965},{"tsp.u1060.p140",45246},
	{"tsp.u1060.p150",44701},{"tsp.rl1323.p10",307730},
	{"tsp.rl1323.p20",201640},{"tsp.rl1323.p30",163150},
	{"tsp.rl1323.p40",135236},{"tsp.rl1323.p50",118727},
	{"tsp.rl1323.p60",106301},{"tsp.rl1323.p70",97193},
	{"tsp.rl1323.p80",89506},{"tsp.rl1323.p90",83200},
	{"tsp.rl1323.p100",78710},{"tsp.u1817.p10",45791},
	{"tsp.u1817.p20",30901},{"tsp.u1817.p30",24099},
	{"tsp.u1817.p40",20945},{"tsp.u1817.p50",18491},
	{"tsp.u1817.p60",16264},{"tsp.u1817.p70",14811},
	{"tsp.u1817.p80",13677},{"tsp.u1817.p90",12951},
	{"tsp.u1817.p100",12699},{"tsp.u1817.p110",10925},
	{"tsp.u1817.p120",10776},{"tsp.u1817.p130",10473},
	{"tsp.u1817.p140",10160},{"tsp.u1817.p150",9160},
	{"tsp.pcb3038.p10",72854},{"tsp.pcb3038.p20",49304},
	{"tsp.pcb3038.p30",39350},{"tsp.pcb3038.p40",33642},
	{"tsp.pcb3038.p50",29820},{"tsp.pcb3038.p100",20663},
	{"tsp.pcb3038.p150",16477},{"tsp.pcb3038.p200",14090},
	{"tsp.pcb3038.p250",12271},{"tsp.pcb3038.p300",11525},
	{"tsp.pcb3038.p350",10481},{"tsp.pcb3038.p400",9749},
	{"tsp.pcb3038.p450",8896},{"tsp.pcb3038.p500",8500},
};

using Solution = vector<int>;

enum {
	Greedy = false,
	Random = true,

	/*局部搜索参数*/
	TabuSearchSteps = 1000,
	NoDecreaseLimit = 1000,
	RandomAlphaInInit = 70,	// /100
	RandomAlphaInSearch = 85,	// /100

	/*PathRellinking参数*/
	DistBeta = 50,		// /100
	PopulationSize = 15,
	MaxIterations = 1000,

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
		assert(_l >= 0 && _r >= l);
		return rand() % (_r - _l + 1) + _l;
	}
}rander;

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

	void updateTabuTable(int u, int v, int nowStep) {	//u加入成为服务节点
		if (doubleTabu) {
			int alpha = nowStep + pNum / 10 + rander.getRandNumber() % 10;				//u alpha步之内不能再变回为普通节点
			int beta = nowStep + (vNum - pNum) / 10 + rander.getRandNumber() % 100;		//反之v Bate步之内不能成为服务节点
			costomerTabu[u] = alpha;
			serviceTabu[v] = beta;
		}
		else {
			//tabuTable[u][v] = nowStep + R + rander.getRandNumber(1, R+1);		//<u, v>被禁忌
			tabuTable[u][v] = nowStep + pNum * (vNum - pNum) / 100 + rander.getRandNumber(0, 10 * pNum);
		}
	}
};

struct PCenterSolver {

	///原始输入数据
	int vNum, pNum;	//vNum: 总节点数, pNum: 总中心数
	vector<vector<int>> graph;	//邻接矩阵表示的图

	///预处理数据
	vector<vector<Edge>> minDistEdge;	//minDistAndIndex[i][j] 表示距离点i出发第j长的边
	vector<vector<int>> kValue;			//kValue[i][j]=k 表示点j是距离点i第k近的点,距离相等取较小的小标

	///辅助数据结构
	vector<pii> FTable;
	vector<pii> DTable;
	TabuTable tabuTable;

	///计算结果
	int minMaximum;
	Solution centers;
	vector<bool>isCenter;

	int historyOptimalValue;
	Solution historyOptimalCenters;

	PCenterSolver() {};
	~PCenterSolver() {};
	PCenterSolver(vector<vector<int>> g, int p);	//初始化输入
	bool operator == (const PCenterSolver & sol)const;	//判断两个解是否相同(可优化)
	///算法过程

	void init(bool randomSolution = false);	//预处理
	Solution getInitSolution();	//得到一个初始解 O(p^2 * n)
	//返回距离当前解sol最远的点集合（所有的最长服务边）; 格式(u, v)->点u服务点v; 仅在得到初始解使用; 复杂度O(n*p)
	vector<pii> getMaxDistPointsFromSolution(const Solution &sol);
	vector<pii> getMaxDistPointsFromSolution();						//同上,区别在于通过查表得到，O(n)
	vector<pii> getNeighbourhoodsBySolution(const Solution &sol);	//返回当前解的交换邻域, O(N_k * p)	--已经舍弃
	vector<int> neighbourhoodsEvaluation(const vector<pii> &Neighbourhoods);	//邻域评估 O(N_k * p * n)	--已经舍弃
	vector<piii> getNeighbourhoodsAndEvaluation(const Solution &sol);	//邻域评估 O(N_k * n)
	int initFDTable(const Solution &sol);	//初始化FD表 O(n^2)
	int addPointToSolution(int addPoint);	//从当前解sol中增加一个点u，修改F表，返回Sc值 O(n)
	int removePointFromSolution(int removePoint); //从当前解sol中删除一个解u, 查询并修改D表，返回Mf值 O(n * p)
	int resultOfRemovePoint(int removePoint); //从当前解sol中删除一个解u, 查询D表，返回Mf值 O(n)
	void swapPairs(int addPoint, int removePoint);	//对当前解进行交换操作 O(n)
	void upDateHistoryOptimal();		//由自身解更新
	void upDateHistoryOptimal(const PCenterSolver &sol); //由指定解得到
	int searchOneStep(int nowStep);	//进行一步禁忌搜索 O(N_k * p * n + n^2) -> 邻域生成 + 评估
	void solveWithNSearch(int steps);	//进行N步禁忌搜索
	void changeToOptimalSolution();		//将历史最优解更新到当前解
	int calculateResultByForce(const Solution &sol);	//暴力计算结果，仅用于验算FDTable

	/// 输出 FOR DEBUG
	void printMinDistEdge();
	void printKValue();
	void printSolution(const Solution &sol);
	void printFTable();
	void printDTable();
};

struct PathRellinking {
	int a;
	struct QRecoder {
		int result, vc, vci, ve, vei;
	};
	PathRellinking() {}
	~PathRellinking() {}
	static vector<bool> pointsAppear(const vector<int>& points, int vNum); //O(n)
	static vector<int> intersectionOfSolution(const PCenterSolver& Sc, const PCenterSolver& Se); //O(n)
	static vector<int> removePointsFromSolution(const PCenterSolver& sol, const vector<int> &removePoints); //O(n)
	static vector<int> addPointsToSolution(const PCenterSolver& sol, const vector<int> &addPoints);  //O(n)
	static int distOfTwoSolution(const PCenterSolver& Sc, const PCenterSolver& Se);	//O(n)
	static bool solverInPopulation(const PCenterSolver& sol, const vector<PCenterSolver>& P);
	static PCenterSolver pathRellinkingCrossover(const PCenterSolver &Sc, const PCenterSolver &Se);
	static Solution GRASP(vector<vector<int>>_graph, int pNum, unsigned _randSeed = 0, int opt = 0);
};

//Solution run(vector<vector<int>>_graph, int pNum, unsigned _randSeed = 0);