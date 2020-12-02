#pragma once

#include "SetCovering.h"

struct VWTS {
	static VWTSSolution VWTSSolver(const vector<vector<int>> &graph, int pNum, unsigned randomSeed, int bestResult);
	static int calculationResult(const vector<vector<int>> &graph, const VWTSSolution &sol);
};

