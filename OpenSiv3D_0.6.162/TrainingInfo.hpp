#pragma once
# include <Siv3D.hpp>

struct TrainingInfo {
	String name;
	int cost;
	std::array<int, 6> increaseAmounts;
};

inline Array<TrainingInfo> trainingInfos = {
	{ U"持久飛", 10,
	{18, 0, 0, 6, 0, 0} },

	{ U"筋力強化", 10,
	{0, 0, 18, 0, 6, 0} },

	{ U"魔力集中", 15,
	{0, 0, 0, 0, 18, 6} },

	{ U"メンタル", 15,
	{0, 18, 0, 0, 0, 6} },

	{ U"滝行", 15,
	{6, 0, 0, 18, 0, 0} },

	{ U"瞑想", 15,
	{0, 6, 0, 0, 0, 18} },

	{ U"総合訓練", 15,
	{4, 4, 4, 4, 4, 4} },
};
