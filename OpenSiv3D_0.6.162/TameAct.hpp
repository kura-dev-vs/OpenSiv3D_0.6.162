#pragma once

struct TameActInfo {
	String name;
	int32 timeCost;
};

inline Array<TameActInfo> actionInfos = {
	{ U"メイン", 0},
	{ U"訓練" , 1},
	{ U"交流" , 1},
	{ U"寝かせる" , 5},
};
