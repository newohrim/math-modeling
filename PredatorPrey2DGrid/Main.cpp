#include <vector>
#include <iostream>
#include <time.h>

#define EMPTY false
#define BUSY true
#define PREDATOR false
#define PREY true

using GridRow = std::vector<bool>;
using GridSpace = std::vector<GridRow>;

struct Pos 
{
	int x = 0;
	int y = 0;
};

Pos operator+(const Pos& A, const Pos& B)
{
	return Pos{ A.x + B.x, A.y + B.y };
}

bool operator==(const Pos& A, const Pos& B)
{
	return A.x == B.x && A.y == B.y;
}

bool operator!=(const Pos& A, const Pos& B)
{
	return !(A == B);
}

struct Grid 
{
	int mWidth = 0;
	int mHeight = 0;

	GridSpace mContent;

	Grid(const int Width, const int Height) 
	{
		mWidth = Width;
		mHeight = Height;
		mContent = GridSpace(Height);
		for (int i = 0; i < Height; ++i) 
		{
			mContent[i] = GridRow(Width, false);
		}
	}

	bool Get(const Pos Target) const
	{
		return mContent[Target.y][Target.x];
	}

	void Set(const Pos Target, bool Value) 
	{
		mContent[Target.y][Target.x] = Value;
	}

	Pos GetRandomPos() const
	{
		return 
		{ 
			std::rand() % mWidth, 
			std::rand() % mHeight 
		};
	}

	Pos GetRandomEmptyPos() const
	{
		Pos Result;
		int i = 10000;
		do 
		{
			Result = GetRandomPos();
		} while (--i > 0 && Get(Result) != EMPTY);

		return Result;
	}

	void ClearGrid() 
	{
		for (int i = 0; i < mContent.size(); ++i) 
		{
			mContent[i].clear();
		}
		mContent.clear();
	}
};

Pos GetRandomNearbyPos(const Pos& InitPos, const Grid& Bounds, const bool IsPrey, const std::vector<Pos>& Predators)
{
	const Pos NearbyPoses[8] = {
		InitPos + Pos{ -1, -1 },
		InitPos + Pos{ 0, -1 },
		InitPos + Pos{ 1, -1 },
		InitPos + Pos{ 1, 0 },
		InitPos + Pos{ 1, 1 },
		InitPos + Pos{ 0, 1 },
		InitPos + Pos{ -1, 1 },
		InitPos + Pos{ -1, 0 },
	};
	std::vector<int> ValidPoses;
	for (int i = 0; i < 8; ++i) 
	{
		const Pos PickedPos = NearbyPoses[i];
		if (PickedPos.x > 0 &&
			PickedPos.x < Bounds.mWidth - 1 &&
			PickedPos.y > 0 &&
			PickedPos.y < Bounds.mHeight - 1) 
		{
			if (IsPrey) 
			{
				bool IsValid = true;
				for (const Pos PredPos : Predators) 
				{
					if (PredPos == PickedPos) 
					{
						IsValid = false;
						break;
					}
				}
				if (!IsValid) 
				{
					continue;
				}
			}

			ValidPoses.push_back(i);
		}
	}

	if (ValidPoses.size() > 0) 
	{
		return NearbyPoses[ValidPoses[std::rand() % ValidPoses.size()]];
	}
	else 
	{
		return InitPos;
	}
}

bool RndChance(const float TargetRate) 
{
	return (std::rand() % 100 + 1) / 100.0f <= TargetRate;
}

int main() 
{
	std::srand(time(0));
	constexpr int MaxPreys = 10;
	constexpr int MaxPredators = 10;
	constexpr float PredatorBornRate = 0.25f;
	constexpr float PredatorDieRate = 0.25f;
	constexpr float PreyBornRate = 0.25f;
	constexpr float PreyDieRate = 0.25f;

	Grid Bounds = { 100, 100 };

	std::vector<Pos> Preys(5000);//std::rand() % MaxPreys + 1);
	std::vector<Pos> Predators(3500);//std::rand() % MaxPredators + 1);

	for (int i = 0; i < Preys.size(); ++i) 
	{
		Preys[i] = Bounds.GetRandomPos();
		Bounds.Set(Preys[i], BUSY);
	}
	for (int i = 0; i < Predators.size(); ++i) 
	{
		Predators[i] = Bounds.GetRandomEmptyPos();
	}
	Bounds.ClearGrid();

	int Step = 0;

	std::cout << "Preys: " << Preys.size() << ", Predators = " << Predators.size() << '\n';

	while (Step++ < 10000 && Preys.size() > 0 && Predators.size() > 0) 
	{
		for (int i = Predators.size() - 1; i >= 0; --i)
		{
			const Pos NearbyPos = 
				GetRandomNearbyPos(Predators[i], Bounds, PREDATOR, Predators);
			if (NearbyPos != Predators[i]) 
			{
				bool FoundPrey = false;
				for (int j = Preys.size() - 1; j >= 0; --j) 
				{
					if (Preys[j] == NearbyPos) 
					{
						FoundPrey = true;
						Preys.erase(Preys.begin() + j);
						//std::cout << "Prey was eaten" << '\n';
					}
				}
				Predators[i] = NearbyPos;

				if (FoundPrey) 
				{
					continue;
				}
			}

			if (RndChance(PredatorDieRate))
			{
				Predators.erase(Predators.begin() + i);
			}
		}

		for (int i = Preys.size() - 1; i >= 0; --i)
		{
			const Pos NearbyPos =
				GetRandomNearbyPos(Preys[i], Bounds, PREY, Predators);
			if (NearbyPos != Preys[i]) 
			{
				Preys[i] = NearbyPos;
			}

			if (RndChance(PreyBornRate)) 
			{
				Preys.push_back(Preys[i]);
			}
			else if (RndChance(PreyDieRate)) 
			{
				Preys.erase(Preys.begin() + i);
			}
		}

		std::cout << Preys.size() << ';' << Predators.size() << '\n';
	}

	if (Step == 10000) 
	{
		std::cout << "Out of steps!" << '\n';
	}

	return 0;
}