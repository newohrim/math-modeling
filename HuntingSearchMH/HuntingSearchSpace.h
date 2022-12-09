#pragma once

#include <algorithm>
#include <vector>

using DecisionVars = std::vector<double>;
using ParametricFunc = double (*)(DecisionVars);
using FRange = std::pair<double, double>;

struct HuntingSearchSpace
{
	int m_DecisionVarsCount = 2;

	ParametricFunc f_ObjectiveFunc;

	std::vector<ParametricFunc> f_InequalityConstraints;

	std::vector<FRange> m_DesignSpace;

	double Calculate_ObjectiveFunc(const DecisionVars& Vars) const
	{
		return f_ObjectiveFunc(Vars);
	}

	double Calculate_ObjectiveFuncConstrained(const DecisionVars& Vars) const
	{
		double Result = f_ObjectiveFunc(Vars);
		for (ParametricFunc Func : f_InequalityConstraints) 
		{
			Result -= std::min(0.0, Func(Vars)) * 100.0f;
		}

		return Result;
	}

	double ClampByDesignSpace(double Var, int VarIndex) const
	{
		if (Var < m_DesignSpace[VarIndex].first)
			Var = m_DesignSpace[VarIndex].first;
		if (Var > m_DesignSpace[VarIndex].second)
			Var = m_DesignSpace[VarIndex].second;

		return Var;
	}

	bool AreValidArguments(const DecisionVars& Vars) const
	{
		bool Result = true;
		for (ParametricFunc Func : f_InequalityConstraints)
		{
			Result &= (Func(Vars) >= 0);
		}

		return Result;
	}
};

