#include <iostream>

#include "HuntingSearchSolver.h"

int main() 
{
	const HSParams Params = HSParams
	{ 
		2, // Number of Epochs (NE)
		30, // Iteration per Epoch (IE)
		10, // Hunting Group Size (HGS)
		0.3f, // Maximum movement towards leader (MML)
		0.3f, // Hunting group consideration rate (HGCR)
		1e-2f, // Ra max
		1e-7f, // Ra min
		0.1f, // Reorganization parameter Alpha
		-1.0f // Reorganization parameter Beta
	};
	HuntingSearchSolver Solver(Params);
	const HSResult Result = Solver.Solve();

	return 0;
}