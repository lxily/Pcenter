// main.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "GRASP.h"

vector<vector<string>>instPmed = {
{
	"pmed1.n100e198p5","pmed2.n100e193p10","pmed3.n100e198p10","pmed4.n100e196p20",
	"pmed5.n100e196p33","pmed6.n200e786p5","pmed7.n200e779p10","pmed8.n200e792p20",
	"pmed9.n200e785p40","pmed10.n200e786p67","pmed11.n300e1772p5","pmed12.n300e1758p10",
	"pmed13.n300e1760p30","pmed14.n300e1771p60","pmed15.n300e1754p100","pmed16.n400e3153p5",
	"pmed17.n400e3142p10","pmed18.n400e3134p40","pmed19.n400e3134p80","pmed20.n400e3144p133",
	"pmed21.n500e4909p5","pmed22.n500e4896p10","pmed23.n500e4903p50","pmed24.n500e4914p100",
	"pmed25.n500e4894p167","pmed26.n600e7069p5","pmed27.n600e7072p10","pmed28.n600e7054p60",
	"pmed29.n600e7042p120","pmed30.n600e7042p200","pmed31.n700e9601p5","pmed32.n700e9584p10",
	"pmed33.n700e9616p70","pmed34.n700e9585p140","pmed35.n800e12548p5","pmed36.n800e12560p10",
	"pmed37.n800e12564p80","pmed38.n900e15898p5","pmed39.n900e15896p10","pmed40.n900e15879p90",
},
{
	"tsp.u1060.p10","tsp.u1060.p20","tsp.u1060.p30","tsp.u1060.p40",
	"tsp.u1060.p50","tsp.u1060.p60","tsp.u1060.p70","tsp.u1060.p80",
	"tsp.u1060.p90","tsp.u1060.p100","tsp.u1060.p110","tsp.u1060.p120",
	"tsp.u1060.p130","tsp.u1060.p140","tsp.u1060.p150",
},
{
	"tsp.rl1323.p10","tsp.rl1323.p20","tsp.rl1323.p30",
	"tsp.rl1323.p40","tsp.rl1323.p50","tsp.rl1323.p60",
	"tsp.rl1323.p70","tsp.rl1323.p80","tsp.rl1323.p90","tsp.rl1323.p100",
},
{
	"tsp.u1817.p10","tsp.u1817.p20","tsp.u1817.p30","tsp.u1817.p40",
	"tsp.u1817.p50","tsp.u1817.p60","tsp.u1817.p70","tsp.u1817.p80",
	"tsp.u1817.p90","tsp.u1817.p100","tsp.u1817.p110","tsp.u1817.p120",
	"tsp.u1817.p130","tsp.u1817.p140","tsp.u1817.p150",
},
{
	"tsp.pcb3038.p10","tsp.pcb3038.p20","tsp.pcb3038.p30","tsp.pcb3038.p40",
	"tsp.pcb3038.p50","tsp.pcb3038.p100","tsp.pcb3038.p150","tsp.pcb3038.p200","tsp.pcb3038.p250",
	"tsp.pcb3038.p300","tsp.pcb3038.p350","tsp.pcb3038.p400","tsp.pcb3038.p450","tsp.pcb3038.p500",
}
};
vector<vector<int>>graph;

int main(){
	int nodeNum, centerNum, bestResult;
	char instRoad[128]; int test_data = 1;
	for (int tp = 0; tp < min((int)instPmed.size(),test_data); ++tp) {
		for (int i = 0; i < instPmed[tp].size(); ++i) {
			string inst = "../Instance/" + instPmed[tp][i] + ".txt";
			int randomSeed = unsigned(rand()*rand()*rand());
			#pragma warning(disable:4996)
			strcpy(instRoad, inst.c_str());
			freopen(instRoad, "r", stdin);
			scanf("%d %d %d", &nodeNum, &centerNum, &bestResult);
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
			PathRellinking::GRASP(graph, centerNum, randomSeed, bestResult);
			printf("=======================================================\n\n");
		}
	}
	return 0;
}
