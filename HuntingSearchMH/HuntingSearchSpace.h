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
		/*
		return Calculate_ObjectiveFunc(x1, x2) - 
			std::min(0.0, Calculate_InequalityConstraint1(x1, x2)) * 100 + 
			std::max(0.0, Calculate_InequalityConstraint2(x1, x2)) * 100;
			*/
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
		/*
		return Calculate_DesignSpaceX1(x1) &&
			Calculate_DesignSpaceX2(x2) &&
			(Calculate_InequalityConstraint1(x1, x2) >= 0) &&
			(Calculate_InequalityConstraint2(x1, x2) <= 0);
			*/
	}

	double Calculate_InequalityConstraint1(double x1, double x2) const
	{
		const double a = x1 - 0.05f;
		const double b = x2 - 2.5f;
		return 4.84f - a * a - b * b;
	}

	double Calculate_InequalityConstraint2(double x1, double x2) const
	{
		const double a = x1;
		const double b = x2 - 2.5f;
		return a * a + b * b - 4.84f;
	}

	bool Calculate_DesignSpaceX1(double x1) const
	{
		return x1 >= 0.0f && x1 <= 6.0f;
	}

	bool Calculate_DesignSpaceX2(double x2) const
	{
		return x2 >= 0.0f && x2 <= 6.0f;
	}
};

