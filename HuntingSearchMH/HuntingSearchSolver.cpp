#include "HuntingSearchSolver.h"

#include <ctime>
#include <limits>
#include <algorithm>
#include <cmath>

HuntingSearchSolver::HuntingSearchSolver(const HSParams& Params)
	: m_SolverParams(Params) {  }

HSResult HuntingSearchSolver::Solve(const HuntingSearchSpace& SearchSpace)
{
	// HG init
	std::vector<DecisionVars> HG = InitializeHG(SearchSpace);

	int EpochesBefore = 0;
	int Iter = 10000;
	int IterPerEpoch = 0;
	int LeaderIndex;
	while (Iter-- > 0) // TODO: add Epsilon termination criterion
	{
		// Move hunters
		LeaderIndex = GetLeadersIndex(HG, SearchSpace);
		for (int i = 0; i < HG.size(); ++i) 
		{
			if (i == LeaderIndex) 
			{
				continue;
			}

			DecisionVars NewVector(SearchSpace.m_DecisionVarsCount);
			for (int j = 0; j < NewVector.size(); ++j) 
			{
				NewVector[j] = MoveDeciesionVar(HG[i][j], HG[LeaderIndex][j], j, SearchSpace);
			}
			if (SearchSpace.Calculate_ObjectiveFuncConstrained(NewVector) <
				SearchSpace.Calculate_ObjectiveFuncConstrained(HG[i]))
			{
				HG[i] = NewVector;
			}
		}
		LeaderIndex = GetLeadersIndex(HG, SearchSpace);

		// Position correction
		for (int i = 0; i < HG.size(); ++i) 
		{
			if (i == LeaderIndex)
				continue;
			DecisionVars NewVector(SearchSpace.m_DecisionVarsCount);
			for (int j = 0; j < NewVector.size(); ++j) 
			{
				srand(time(0) * (i + 1) * (j + 1));
				float RandValue = RandomFloat();
				if (RandValue <= m_SolverParams.m_HGCR)
				{
					srand(time(0) * (i + 1) * (j + 1) + 1);
					NewVector[j] = HG[rand() % HG.size()][j];
				}
				else
				{
					srand(time(0) * (i + 1) * (j + 1) + 1);
					// TODO: add RA value reduction
					NewVector[j] = HG[i][j] + ((rand() % 2) * 2 - 1) * m_SolverParams.m_RaMax;
				}
				NewVector[j] = SearchSpace.ClampByDesignSpace(NewVector[j], j);
			}
			if (SearchSpace.Calculate_ObjectiveFuncConstrained(NewVector) <
				SearchSpace.Calculate_ObjectiveFuncConstrained(HG[i]))
			{
				HG[i] = NewVector;
			}
		}
		LeaderIndex = GetLeadersIndex(HG, SearchSpace);

		if (SearchSpace.AreValidArguments(HG[LeaderIndex]) &&
			(Iter == 0 || EpochesBefore >= m_SolverParams.m_NumberOfEpochs))
			break;

		// Reorganize HG
		int Worst;
		GetBestAndWorstHunters(HG, SearchSpace, LeaderIndex, Worst);
		const double MaxDiff = std::abs(
			SearchSpace.Calculate_ObjectiveFuncConstrained(HG[LeaderIndex]) - 
			SearchSpace.Calculate_ObjectiveFuncConstrained(HG[Worst]));
		if (MaxDiff < Epsilon || ++IterPerEpoch >= m_SolverParams.m_IterationPerEpoch) 
		{
			for (int i = 0; i < HG.size(); ++i) 
			{
				// TODO: make design space constraints vars
				if (i == LeaderIndex)
					continue;
				DecisionVars NewVector(SearchSpace.m_DecisionVarsCount);
				for (int j = 0; j < NewVector.size(); ++j) 
				{
					srand(time(0) * (i + 1) * (j + 1));
					const double temp = ((rand() % 2) * 2 - 1) * RandomFloat() *
						(SearchSpace.m_DesignSpace[j].second - SearchSpace.m_DesignSpace[j].first) * 
						m_SolverParams.m_ReorgAlpha * std::exp(-m_SolverParams.m_ReorgBeta * EpochesBefore);
					NewVector[j] = HG[LeaderIndex][j] +
						temp;
					NewVector[j] = SearchSpace.ClampByDesignSpace(NewVector[j], j);
				}
				HG[i] = NewVector;
			}
			EpochesBefore++;
			SearchSpace.Calculate_ObjectiveFunc(HG[LeaderIndex]); // Temp
			IterPerEpoch = 0;
			GetBestAndWorstHunters(HG, SearchSpace, LeaderIndex, Worst);
			const double MaxDiff2 = std::abs(
				SearchSpace.Calculate_ObjectiveFuncConstrained(HG[LeaderIndex]) -
				SearchSpace.Calculate_ObjectiveFuncConstrained(HG[Worst]));
			if (MaxDiff2 < Epsilon) 
			{
				break;
			}
		}
	}

	HSResult Result;
	Result.m_FuncMin = 
		SearchSpace.f_ObjectiveFunc(HG[LeaderIndex]);
	Result.Solution = HG[LeaderIndex];

	return Result;
}

std::vector<DecisionVars> HuntingSearchSolver::InitializeHG(
	const HuntingSearchSpace& SearchSpace)
{
	std::vector<DecisionVars> HG(m_SolverParams.m_HGSize);
	for (int i = 0; i < HG.size(); ++i) 
	{
		HG[i] = DecisionVars(SearchSpace.m_DecisionVarsCount);
		for (int j = 0; j < HG[i].size(); ++j) 
		{
			srand(time(0) * (i + 1) * (j + 1));
			HG[i][j] = RandomFloat() * std::abs(
				SearchSpace.m_DesignSpace[j].second - SearchSpace.m_DesignSpace[j].first);
		}
	}

	return HG;
}

int HuntingSearchSolver::GetLeadersIndex(
	const std::vector<DecisionVars>& HG, const HuntingSearchSpace& SearchSpace) const
{
	int LeaderIndex = 0;
	double MinFuncValue = std::numeric_limits<double>::max();
	for (int i = 0; i < HG.size(); ++i) 
	{
		const double FuncValue = SearchSpace.Calculate_ObjectiveFuncConstrained(HG[i]);
		if (FuncValue < MinFuncValue)
		{
			MinFuncValue = FuncValue;
			LeaderIndex = i;
		}
	}

	return LeaderIndex;
}

void HuntingSearchSolver::GetBestAndWorstHunters(
	const std::vector<DecisionVars>& HG, const HuntingSearchSpace& SearchSpace, int& Best, int& Worst) const
{
	Best = 0;
	Worst = 0;
	double MinFuncValue = std::numeric_limits<double>::max();
	double MaxFuncValue = std::numeric_limits<double>::min();
	for (int i = 0; i < HG.size(); ++i)
	{
		const double FuncValue = SearchSpace.Calculate_ObjectiveFuncConstrained(HG[i]);
		if (FuncValue < MinFuncValue)
		{
			MinFuncValue = FuncValue;
			Best = i;
		}
		if (FuncValue > MaxFuncValue) 
		{
			MaxFuncValue = FuncValue;
			Worst = i;
		}
	}
}

inline double HuntingSearchSolver::MoveDeciesionVar(
	const double VarValue, const double LeadersVarValue, int VarIndex, const HuntingSearchSpace& SearchSpace) const
{
	const double Result = VarValue + RandomFloat() * m_SolverParams.m_MML * (LeadersVarValue - VarValue);
	if (Result < SearchSpace.m_DesignSpace[VarIndex].first)
		return SearchSpace.m_DesignSpace[VarIndex].first;
	if (Result > SearchSpace.m_DesignSpace[VarIndex].second)
		return SearchSpace.m_DesignSpace[VarIndex].second;

	return Result;
}

inline double HuntingSearchSolver::RandomFloat()
{
	return (double)rand() / RAND_MAX;
}
