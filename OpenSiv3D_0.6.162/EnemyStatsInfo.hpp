#pragma once
# include <Siv3D.hpp>
# include "Card.hpp"

struct EnemySkillInfo {
	int32 id;
	CardType type;
	const int32 statusIdx;
	double effectValue;
	double actionInterval = 3;
};

struct EnemyInfo {
	String name;
	int32 id;
	Array<EnemySkillInfo> enemySkillInfos;
	Array<int32> status;
};



inline Array<EnemySkillInfo> mutantSkillInfos = {
	{ 0, CardType::Attack, 2, 20, 5 },
	{ 1, CardType::Attack, 2, 50, 10 },
	{ 2, CardType::Buf, 2, 1.2, 7 },
	{ 3, CardType::Debuf, 3, 0.9, 7 },
};

inline Array<EnemySkillInfo> slimeSkillInfos = {
	{ 0, CardType::Attack, 2, 0.1, 10 },
	{ 1, CardType::Attack, 2, 1, 20 },
	{ 2, CardType::Buf, 2, 1.2, 10 },
	{ 3, CardType::Debuf, 3, 0.9, 10 },
};

inline Array<EnemyInfo> enemyInfos = {
	{ U"Slime", 0, mutantSkillInfos,
		{500, 200, 200, 200, 200, 200}, },
	{ U"Goblin", 1, mutantSkillInfos,
		{1000, 500, 500, 500, 500, 500}, },
	{ U"Mutant", 2, mutantSkillInfos,
		{5000, 2000, 2000, 2000, 2000, 2000}, },
};
