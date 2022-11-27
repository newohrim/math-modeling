#include "HuntingSearchSolver.h"

#include <ctime>
#include <limits>
#include <algorithm>
#include <cmath>

HuntingSearchSolver::HuntingSearchSolver(const HSParams Params)
	: m_SolverParams(Params), m_SearchSpace() {  }

HSResult HuntingSearchSolver::Solve()
{
	HSResult Result;

	// HG init
	std::vector<std::pair<double, double>> HG = InitializeHG();

	int EpochesBefore = 0;
	int Iter = 10000;
	int IterPerEpoch = 0;
	int LeaderIndex;
	while (Iter-- > 0) // TODO: add Epsilon termination criterion
	{
		// Move hunters
		LeaderIndex = GetLeadersIndex(HG);
		for (int i = 0; i < HG.size(); ++i) 
		{
			if (i == LeaderIndex) 
			{
				continue;
			}

			const double TempVarX1 = MoveDeciesionVar(HG[i].first, HG[LeaderIndex].first);
			const double TempVarX2 = MoveDeciesionVar(HG[i].second, HG[LeaderIndex].second);
			if (m_SearchSpace.Calculate_ObjectiveFuncConstrained(TempVarX1, TempVarX2) <
				m_SearchSpace.Calculate_ObjectiveFuncConstrained(HG[i].first, HG[i].second)) 
			{
				HG[i].first = TempVarX1;
				HG[i].second = TempVarX2;
			}
		}
		LeaderIndex = GetLeadersIndex(HG);

		// Position correction
		for (int i = 0; i < HG.size(); ++i) 
		{
			if (i == LeaderIndex)
				continue;
			srand(time(0) * i);
			double NewX1, NewX2;
			float RandValue = RandomFloat();
			if (RandValue <= m_SolverParams.m_HGCR) 
			{
				srand(time(0) * i);
				NewX1 = HG[rand() % HG.size()].first;
			}
			else 
			{
				srand(time(0) * i);
				// TODO: add RA value reduction
				NewX1 = HG[i].first + ((rand() % 2) * 2 - 1) * m_SolverParams.m_RaMax;
			}
			srand(time(0) * i);
			RandValue = RandomFloat();
			if (RandValue <= m_SolverParams.m_HGCR)
			{
				srand(time(0) * i);
				NewX2 = HG[rand() % HG.size()].second;
			}
			else
			{
				srand(time(0) * i);
				// TODO: add RA value reduction
				NewX2 = HG[i].second + ((rand() % 2) * 2 - 1) * m_SolverParams.m_RaMax;
			}
			if (NewX1 < 0.0f)
				NewX1 = 0.0f;
			if (NewX1 > 6.0f)
				NewX1 = 6.0f;
			if (NewX1 < 0.0f)
				NewX1 = 0.0f;
			if (NewX1 > 6.0f)
				NewX1 = 6.0f;
			if (m_SearchSpace.Calculate_ObjectiveFuncConstrained(NewX1, NewX2) <
				m_SearchSpace.Calculate_ObjectiveFuncConstrained(HG[i].first, HG[i].second))
			{
				HG[i].first = NewX1;
				HG[i].second = NewX2;
			}
		}
		LeaderIndex = GetLeadersIndex(HG);

		if (m_SearchSpace.AreValidArguments(HG[LeaderIndex].first, HG[LeaderIndex].second) && 
			(Iter == 0 || EpochesBefore >= m_SolverParams.m_NumberOfEpochs))
			break;

		// Reorganize HG
		int Worst;
		GetBestAndWorstHunters(HG, LeaderIndex, Worst);
		const double MaxDiff = std::abs(
			m_SearchSpace.Calculate_ObjectiveFuncConstrained(HG[LeaderIndex].first, HG[LeaderIndex].second) - 
			m_SearchSpace.Calculate_ObjectiveFuncConstrained(HG[Worst].first, HG[Worst].second));
		if (MaxDiff < Epsilon || ++IterPerEpoch >= m_SolverParams.m_IterationPerEpoch) 
		{
			for (int i = 0; i < HG.size(); ++i) 
			{
				// TODO: make design space constraints vars
				if (i == LeaderIndex)
					continue;
				srand(time(0) * i);
				double NewX1, NewX2;
				NewX1 = HG[LeaderIndex].first +
					((rand() % 2) * 2 - 1) * RandomFloat() *
					6 * m_SolverParams.m_ReorgAlpha * std::exp(-m_SolverParams.m_ReorgBeta * EpochesBefore);
				srand(time(0) * i + 1);
				NewX2 = HG[LeaderIndex].second +
					((rand() % 2) * 2 - 1) * RandomFloat() *
					6 * m_SolverParams.m_ReorgAlpha * std::exp(-m_SolverParams.m_ReorgBeta * EpochesBefore);
				if (NewX1 < 0.0f)
					NewX1 = 0.0f;
				if (NewX1 > 6.0f)
					NewX1 = 6.0f;
				if (NewX1 < 0.0f)
					NewX1 = 0.0f;
				if (NewX1 > 6.0f)
					NewX1 = 6.0f;
				HG[i].first = NewX1;
				HG[i].second = NewX2;
			}
			EpochesBefore++;
			IterPerEpoch = 0;
			GetBestAndWorstHunters(HG, LeaderIndex, Worst);
			const double MaxDiff2 = std::abs(
				m_SearchSpace.Calculate_ObjectiveFuncConstrained(HG[LeaderIndex].first, HG[LeaderIndex].second) -
				m_SearchSpace.Calculate_ObjectiveFuncConstrained(HG[Worst].first, HG[Worst].second));
			if (MaxDiff2 < Epsilon) 
			{
				break;
			}
		}
	}

	Result.m_FuncMin = 
		m_SearchSpace.Calculate_ObjectiveFunc(HG[LeaderIndex].first, HG[LeaderIndex].second);
	Result.x1 = HG[LeaderIndex].first;
	Result.x2 = HG[LeaderIndex].second;

	return Result;
}

std::vector<std::pair<double, double>> HuntingSearchSolver::InitializeHG()
{
	std::vector<std::pair<double, double>> HG(m_SolverParams.m_HGSize);
	for (int i = 0; i < HG.size(); ++i) 
	{
		srand(time(0) * i);
		HG[i] = { RandomFloat() * 6.0f, RandomFloat() * 6.0f };
	}

	return HG;
}

int HuntingSearchSolver::GetLeadersIndex(const std::vector<std::pair<double, double>>& HG) const
{
	int LeaderIndex = 0;
	double MinFuncValue = std::numeric_limits<double>::max();
	for (int i = 0; i < HG.size(); ++i) 
	{
		const double FuncValue = m_SearchSpace.Calculate_ObjectiveFuncConstrained(HG[i].first, HG[i].second);
		if (FuncValue < MinFuncValue)
		{
			MinFuncValue = FuncValue;
			LeaderIndex = i;
		}
	}

	return LeaderIndex;
}

void HuntingSearchSolver::GetBestAndWorstHunters(const std::vector<std::pair<double, double>>& HG, int& Best, int& Worst) const
{
	Best = 0;
	Worst = 0;
	double MinFuncValue = std::numeric_limits<double>::max();
	double MaxFuncValue = std::numeric_limits<double>::min();
	for (int i = 0; i < HG.size(); ++i)
	{
		const double FuncValue = m_SearchSpace.Calculate_ObjectiveFuncConstrained(HG[i].first, HG[i].second);
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

double HuntingSearchSolver::MoveDeciesionVar(const double VarValue, const double LeadersVarValue) const
{
	const double Result = VarValue + RandomFloat() * m_SolverParams.m_MML * (LeadersVarValue - VarValue);
	if (Result < 0.0f)
		return 0.0f;
	if (Result > 6.0f)
		return 6.0f;

	return Result;
}

double HuntingSearchSolver::RandomFloat()
{
	return (double)rand() / RAND_MAX;
}
