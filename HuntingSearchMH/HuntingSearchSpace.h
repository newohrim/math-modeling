#pragma once

#include <algorithm>

class HuntingSearchSpace
{
public:
	double Calculate_ObjectiveFunc(double x1, double x2) const
	{
		const double a = x1 * x1 + x2 - 11;
		const double b = x1 + x2 * x2 - 7;
		return a * a + b * b;
	}

	double Calculate_ObjectiveFuncConstrained(double x1, double x2) const
	{
		return Calculate_ObjectiveFunc(x1, x2) - 
			std::min(0.0, Calculate_InequalityConstraint1(x1, x2)) * 100 + 
			std::max(0.0, Calculate_InequalityConstraint2(x1, x2)) * 100;
	}

	bool AreValidArguments(double x1, double x2) const
	{
		return Calculate_DesignSpaceX1(x1) &&
			Calculate_DesignSpaceX2(x2) &&
			(Calculate_InequalityConstraint1(x1, x2) >= 0) &&
			(Calculate_InequalityConstraint2(x1, x2) <= 0);
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

