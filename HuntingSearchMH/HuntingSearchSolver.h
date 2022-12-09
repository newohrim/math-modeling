#pragma once

#include "HuntingSearchSpace.h"
#include <vector>
#include <fstream>

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

class Hunter 
{
public:
	Hunter() = default;

	Hunter(const DecisionVars& Pos) : m_Pos(Pos) {  }

	const DecisionVars& GetHunterPosition() const { return m_Pos; }

	double GetValueAtIndex(int Index) const { return m_Pos[Index]; }

	void SetValueAtIndex(int Index, double Value) { m_Pos[Index] = Value; }

private:
	DecisionVars m_Pos;
};

class HuntingSearchSolver
{
public:
	HuntingSearchSolver(const HSParams& Params);

	HSResult Solve(
		const HuntingSearchSpace& SearchSpace, const char* OutputFile);

private:
	const HSParams m_SolverParams;

	static constexpr float Epsilon = 0.01f;

	std::vector<Hunter> InitializeHG(const HuntingSearchSpace& SearchSpace);

	void MoveHuntersToLeader(
		std::vector<Hunter>& HG, const HuntingSearchSpace& SearchSpace);

	void PositionCorrection(
		std::vector<Hunter>& HG, const HuntingSearchSpace& SearchSpace);

	void ReorganizeHG(
		std::vector<Hunter>& HG, const HuntingSearchSpace& SearchSpace, int LeaderIndex, int EpochesBefore);

	bool CheckTerminationCriterion(
		const std::vector<Hunter>& HG, const HuntingSearchSpace& SearchSpace) const;

	int GetLeadersIndex(
		const std::vector<Hunter>& HG, const HuntingSearchSpace& SearchSpace) const;

	void GetBestAndWorstHunters(
		const std::vector<Hunter>& HG, const HuntingSearchSpace& SearchSpace, int& Best, int& Worst) const;

	double MoveDeciesionVar(
		const double VarValue, const double LeadersVarValue, int VarIndex, const HuntingSearchSpace& SearchSpace) const;

	void LogHuntersPositions(
		const std::vector<Hunter>& HG, const HuntingSearchSpace& SearchSpace, std::ofstream& Output) const;

	void LogPosition(const DecisionVars& Pos, std::ofstream& Output) const;

	static double RandomFloat();
};

