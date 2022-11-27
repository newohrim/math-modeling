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
	double x1 = 0.0f;
	double x2 = 0.0f;
};

class HuntingSearchSolver
{
public:
	HuntingSearchSolver(const HSParams Params);

	HSResult Solve();

private:
	const HSParams m_SolverParams;

	const HuntingSearchSpace m_SearchSpace;

	static constexpr float Epsilon = 0.01f;

	std::vector<std::pair<double, double>> InitializeHG();

	int GetLeadersIndex(const std::vector<std::pair<double, double>>& HG) const;

	void GetBestAndWorstHunters(const std::vector<std::pair<double, double>>& HG, int& Best, int& Worst) const;

	double MoveDeciesionVar(const double VarValue, const double LeadersVarValue) const;

	static double RandomFloat();
};

