#pragma once

struct EggInfo {
	String name;
	int nextID;
	// 説明欄の星の数
	std::array<int, 6> stars;
};

inline Array<EggInfo> eggInfos = {
	{ U"Red", 0,
	{3, 3, 4, 2, 4, 2} },

	{ U"Blue", 1,
	{3, 3, 2, 4, 2, 4} },

	{ U"Green", 2,
	{3, 3, 5, 3, 1, 3} },

	{ U"Orange", 3,
	{3, 3, 1, 3, 5, 3} },

	{ U"Gray", 4,
	{1, 5, 4, 2, 4, 2} },
};

inline Array<String> statNames = {
	U"HP",
	U"INT",
	U"ATK",
	U"DEF",
	U"SPATK",
	U"SPDEF"
};

inline Array<String> starNames = {
	U"体力",
	U"知性",
	U"攻撃",
	U"防御",
	U"特攻",
	U"特防"
};
