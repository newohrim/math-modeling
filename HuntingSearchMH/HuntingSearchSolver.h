#pragma once

#include "HuntingSearchSpace.h"
#include <vector>

struct HSParams 
{
	int m_NumberOfEpochs = 1;
	int m_IterationPerEpoch = 10;
	int m_HGSize = 10;
	float m_MML = 0.3f;
	float m_HGCR = 0.3f;
	float m_RaMax = 1e-2f;
	float m_RaMin = 1e-7f;
	float m_ReorgAlpha = 0.1f;
	float m_ReorgBeta = -1.0f;
};

struct HSResult 
{
	double m_FuncMin = 0.0f;
	DecisionVars Solution;
};

class HuntingSearchSolver
{
public:
	HuntingSearchSolver(const HSParams& Params);

	HSResult Solve(const HuntingSearchSpace& SearchSpace);

private:
	const HSParams m_SolverParams;

	static constexpr float Epsilon = 0.01f;

	std::vector<DecisionVars> InitializeHG(const HuntingSearchSpace& SearchSpace);

	int GetLeadersIndex(
		const std::vector<DecisionVars>& HG, const HuntingSearchSpace& SearchSpace) const;

	void GetBestAndWorstHunters(
		const std::vector<DecisionVars>& HG, const HuntingSearchSpace& SearchSpace, int& Best, int& Worst) const;

	double MoveDeciesionVar(
		const double VarValue, const double LeadersVarValue, int VarIndex, const HuntingSearchSpace& SearchSpace) const;

	static double RandomFloat();
};

