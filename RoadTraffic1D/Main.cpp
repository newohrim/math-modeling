#include <vector>
#include <algorithm>
#include <time.h>
#include <iostream>

struct Car 
{
	int Pos = 0;
	int Speed = 0;
};

void ClearLane(int* const Cells, const int LaneLength) 
{
	for (int i = 0; i < LaneLength; ++i)
	{
		Cells[i] = -1;
	}
}

void SetupLane(int* const Cells, const std::vector<Car>& Cars, const int LaneLength) 
{
	ClearLane(Cells, LaneLength);
	for (int i = 0; i < Cars.size(); ++i) 
	{
		if (Cars[i].Pos < LaneLength) 
		{
			Cells[Cars[i].Pos] = i;
		}
	}
}

void ProceedTraffic(std::vector<Car>& Cars, const int LaneLength = 100)
{
	int* Cells = new int[LaneLength];
	int* Cells_2 = new int[LaneLength];

	Cars.push_back({0, 5});
	int Step = 10000;
	while (Step-- > 0) 
	{
		if (std::rand() % 100 < 25)
		{
			Cars.push_back({ 0, 5 });
		}
		SetupLane(Cells, Cars, LaneLength);
		ClearLane(Cells_2, LaneLength);
		for (int i = 0; i < Cars.size(); ++i) 
		{
			int Dist = LaneLength;
			for (int j = Cars[i].Pos + 1; j < LaneLength; ++j) 
			{
				if (Cells[j] >= 0) 
				{
					Dist = j - i;
					break;
				}
			}
			Cars[i].Speed = std::min(Cars[i].Speed + 1, std::min(Dist - 1, 5));
			if (std::rand() % 100 < 25)
			{
				Cars[i].Speed = std::max(0, Cars[i].Speed - 1);
			}
			Cars[i].Pos += Cars[i].Speed;
			if (Cars[i].Pos < LaneLength) 
			{
				Cells_2[Cars[i].Pos] = i;
			}
		}
		for (int i = Cars.size() - 1; i >= 0; --i) 
		{
			if (Cars[i].Pos >= LaneLength) 
			{
				Cars.erase(Cars.begin() + i);
			}
		}
		for (int i = 0; i < LaneLength; ++i) 
		{
			if (Cells[i] >= 0) 
			{
				std::cout << Cars[Cells[i]].Speed;
			}
			else 
			{
				std::cout << '*';
			}
		}
		std::cout << std::endl;

		//std::swap(Cells, Cells_2);
	}

	delete[] Cells;
	delete[] Cells_2;
}

int main() 
{
	std::srand(time(0));
	std::vector<Car> Cars;
	ProceedTraffic(Cars, 100);
	return 0;
}