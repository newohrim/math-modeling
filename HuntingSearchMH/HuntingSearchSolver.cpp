#include "HuntingSearchSolver.h"

#include <ctime>
#include <limits>
#include <algorithm>
#include <cmath>

HuntingSearchSolver::HuntingSearchSolver(const HSParams& Params)
	: m_SolverParams(Params) {  }

HSResult HuntingSearchSolver::Solve(
	const HuntingSearchSpace& SearchSpace, const char* OutputFile)
{
	std::ofstream Output(OutputFile);
	// HG init
	std::vector<Hunter> HG = InitializeHG(SearchSpace);

	int EpochesBefore = 0;
	int Iter = 10000;
	int IterPerEpoch = 0;
	int LeaderIndex;
	while (Iter-- > 0)
	{
		LogHuntersPositions(HG, SearchSpace, Output);
		MoveHuntersToLeader(HG, SearchSpace);
		PositionCorrection(HG, SearchSpace);
		
		LeaderIndex = GetLeadersIndex(HG, SearchSpace);
		if (SearchSpace.AreValidArguments(HG[LeaderIndex].GetHunterPosition()) &&
			(Iter == 0 || EpochesBefore >= m_SolverParams.m_NumberOfEpochs))
			break;

		if (CheckTerminationCriterion(HG, SearchSpace) || 
			++IterPerEpoch >= m_SolverParams.m_IterationPerEpoch) 
		{
			ReorganizeHG(HG, SearchSpace, LeaderIndex, EpochesBefore);
			if (CheckTerminationCriterion(HG, SearchSpace)) 
			{
				break;
			}

			// End of epoch
			EpochesBefore++;
			IterPerEpoch = 0;
		}
	}

	HSResult Result;
	Result.m_FuncMin = 
		SearchSpace.f_ObjectiveFunc(HG[LeaderIndex].GetHunterPosition());
	Result.Solution = HG[LeaderIndex].GetHunterPosition();
	LogPosition(Result.Solution, Output);
	Output.flush();
	Output.close();

	return Result;
}

std::vector<Hunter> HuntingSearchSolver::InitializeHG(
	const HuntingSearchSpace& SearchSpace)
{
	std::vector<Hunter> HG(m_SolverParams.m_HGSize);
	for (int i = 0; i < HG.size(); ++i) 
	{
		DecisionVars Pos(SearchSpace.m_DecisionVarsCount);
		for (int j = 0; j < Pos.size(); ++j) 
		{
			srand(time(0) * (i + 1) * (j + 1));
			Pos[j] = RandomFloat() * std::abs(
				SearchSpace.m_DesignSpace[j].second - SearchSpace.m_DesignSpace[j].first);
		}
		HG[i] = Hunter(Pos);
	}

	return HG;
}

void HuntingSearchSolver::MoveHuntersToLeader(
	std::vector<Hunter>& HG, const HuntingSearchSpace& SearchSpace)
{
	const int LeaderIndex = GetLeadersIndex(HG, SearchSpace);
	for (int i = 0; i < HG.size(); ++i)
	{
		if (i == LeaderIndex)
		{
			continue;
		}

		DecisionVars NewVector(SearchSpace.m_DecisionVarsCount);
		for (int j = 0; j < NewVector.size(); ++j)
		{
			NewVector[j] = MoveDeciesionVar(
				HG[i].GetValueAtIndex(j), // Hunter's value at index j
				HG[LeaderIndex].GetValueAtIndex(j), // Leader's value at index j
				j, // Hunter's index (need for design space constrain)
				SearchSpace
			);
		}
		if (SearchSpace.Calculate_ObjectiveFuncConstrained(NewVector) <
			SearchSpace.Calculate_ObjectiveFuncConstrained(HG[i].GetHunterPosition()))
		{
			HG[i] = NewVector;
		}
	}
}

void HuntingSearchSolver::PositionCorrection(
	std::vector<Hunter>& HG, const HuntingSearchSpace& SearchSpace)
{
	// Get new leader index
	const int LeaderIndex = GetLeadersIndex(HG, SearchSpace);

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
			srand(time(0) * (i + 1) * (j + 1) + 1);
			if (RandValue <= m_SolverParams.m_HGCR)
			{
				NewVector[j] = HG[rand() % HG.size()].GetValueAtIndex(j);
			}
			else
			{
				// TODO: add RA value reduction
				NewVector[j] = HG[i].GetValueAtIndex(j) + ((rand() % 2) * 2 - 1) * m_SolverParams.m_RaMax;
			}
			NewVector[j] = SearchSpace.ClampByDesignSpace(NewVector[j], j);
		}
		if (SearchSpace.Calculate_ObjectiveFuncConstrained(NewVector) <
			SearchSpace.Calculate_ObjectiveFuncConstrained(HG[i].GetHunterPosition()))
		{
			HG[i] = NewVector;
		}
	}
}

void HuntingSearchSolver::ReorganizeHG(
	std::vector<Hunter>& HG, const HuntingSearchSpace& SearchSpace, int LeaderIndex, int EpochesBefore)
{
	for (int i = 0; i < HG.size(); ++i)
	{
		if (i == LeaderIndex)
			continue;
		DecisionVars NewVector(SearchSpace.m_DecisionVarsCount);
		for (int j = 0; j < NewVector.size(); ++j)
		{
			srand(time(0) * (i + 1) * (j + 1));
			const double Offset = ((rand() % 2) * 2 - 1) * RandomFloat() *
				(SearchSpace.m_DesignSpace[j].second - SearchSpace.m_DesignSpace[j].first) *
				m_SolverParams.m_ReorgAlpha * std::exp(-m_SolverParams.m_ReorgBeta * EpochesBefore);
			NewVector[j] = HG[LeaderIndex].GetValueAtIndex(j) + Offset;
			NewVector[j] = SearchSpace.ClampByDesignSpace(NewVector[j], j);
		}
		HG[i] = NewVector;
	}
}

bool HuntingSearchSolver::CheckTerminationCriterion(
	const std::vector<Hunter>& HG, const HuntingSearchSpace& SearchSpace) const
{
	int LeaderIndex, Worst;
	GetBestAndWorstHunters(HG, SearchSpace, LeaderIndex, Worst);
	const double MaxDiff = std::abs(
		SearchSpace.Calculate_ObjectiveFuncConstrained(HG[LeaderIndex].GetHunterPosition()) -
		SearchSpace.Calculate_ObjectiveFuncConstrained(HG[Worst].GetHunterPosition()));

	return MaxDiff < Epsilon;
}

int HuntingSearchSolver::GetLeadersIndex(
	const std::vector<Hunter>& HG, const HuntingSearchSpace& SearchSpace) const
{
	int LeaderIndex = 0;
	double MinFuncValue = std::numeric_limits<double>::max();
	for (int i = 0; i < HG.size(); ++i) 
	{
		const double FuncValue = 
			SearchSpace.Calculate_ObjectiveFuncConstrained(HG[i].GetHunterPosition());
		if (FuncValue < MinFuncValue)
		{
			MinFuncValue = FuncValue;
			LeaderIndex = i;
		}
	}

	return LeaderIndex;
}

void HuntingSearchSolver::GetBestAndWorstHunters(
	const std::vector<Hunter>& HG, const HuntingSearchSpace& SearchSpace, int& Best, int& Worst) const
{
	Best = 0;
	Worst = 0;
	double MinFuncValue = std::numeric_limits<double>::max();
	double MaxFuncValue = std::numeric_limits<double>::min();
	for (int i = 0; i < HG.size(); ++i)
	{
		const double FuncValue = 
			SearchSpace.Calculate_ObjectiveFuncConstrained(HG[i].GetHunterPosition());
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
	const double Result = VarValue + 
		RandomFloat() * m_SolverParams.m_MML * (LeadersVarValue - VarValue);
	return SearchSpace.ClampByDesignSpace(Result, VarIndex);
}

void HuntingSearchSolver::LogHuntersPositions(
	const std::vector<Hunter>& HG, const HuntingSearchSpace& SearchSpace, std::ofstream& Output) const
{
	const int LeaderIndex = GetLeadersIndex(HG, SearchSpace);
	Output << '{';
	LogPosition(HG[LeaderIndex].GetHunterPosition(), Output);
	Output << ", ";
	for (int i = 0; i < HG.size(); ++i) 
	{
		if (i == LeaderIndex)
			continue;
		LogPosition(HG[i].GetHunterPosition(), Output);
		if (i < HG.size() - 1)
		{
			Output << ", ";
		}
	}
	Output << '}' << '\n';
}

void HuntingSearchSolver::LogPosition(const DecisionVars& Pos, std::ofstream& Output) const
{
	Output << '{' << Pos[0];
	for (int j = 1; j < Pos.size(); ++j)
	{
		Output << ", " << Pos[j];
	}
	Output << '}';
}

inline double HuntingSearchSolver::RandomFloat()
{
	return (double)rand() / RAND_MAX;
}
