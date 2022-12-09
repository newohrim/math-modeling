#include <iostream>

#include "HuntingSearchSolver.h"

double ObjectiveFunc(DecisionVars X)
{
	const double a = X[0] * X[0] + X[1] - 11;
	const double b = X[0] + X[1] * X[1] - 7;

	return a * a + b * b;
}

double G1(DecisionVars X)
{
	const double a = X[0] - 0.05f;
	const double b = X[1] - 2.5f;

	// >= 0
	return 4.84f - a * a - b * b;
}

double G2(DecisionVars X)
{
	const double a = X[0];
	const double b = X[1] - 2.5f;

	// >= 0
	return a * a + b * b - 4.84f;
}

int main() 
{
	const HSParams Params = HSParams
	{ 
		2, // Number of Epochs (NE)
		60, // Iteration per Epoch (IE)
		10, // Hunting Group Size (HGS)
		0.3f, // Maximum movement towards leader (MML)
		0.3f, // Hunting group consideration rate (HGCR)
		1e-2f, // Ra max
		1e-7f, // Ra min
		0.1f, // Reorganization parameter Alpha
		-1.0f // Reorganization parameter Beta
	};
	HuntingSearchSolver Solver(Params);
	HuntingSearchSpace SearchSpace;
	SearchSpace.m_DecisionVarsCount = 2;
	SearchSpace.f_ObjectiveFunc = &ObjectiveFunc;
	SearchSpace.f_InequalityConstraints = { &G1, &G2 };
	SearchSpace.m_DesignSpace = 
	{ 
		{0.0f, 6.0f}, // x1 space
		{0.0f, 6.0f}  // x2 space
	};
	const HSResult Result = Solver.Solve(SearchSpace, "output.txt");

	return 0;
}