# pragma once
# include <Siv3D.hpp>
# include "FbxModelManager.hpp"
# include "AudioManager.hpp"
# include "Enum.hpp"

# include "EggInfo.hpp"
# include "DragonStatsInfo.hpp"
# include "EnemyStatsInfo.hpp"
# include "TrainingInfo.hpp"
# include "Card.hpp"
# include "DamageText.hpp"

# include "TrainingButton.hpp"
# include "TameActBtn.hpp"
# include "UIBar.hpp"
// シーンのステート
enum class State
{
	Title,
	Egg,
	TameScene,
	Test,
	EvoScene,
	DeathScene,
	BattleScene,
	Options,
	Quit,
};

void EvolutionStatusIncrease(Array<int32>& curStatus, int currentID);
// 共有するデータ
struct GameData
{
	int currentID = 0;
	int nextID = 5;
	int gen = 0;
	int age = 0;
	int currentDay = 0;//日にち
	int timeOfDay = 1;//時間
	int lifespan = 100;
	const int timeMax = 12;
	Array<int32> battleSchedule = { 6, 12, 18 };
	int battleTime = 6;
	int32 enemyIdx = 0;

	int32 curST = 100;
	int32 maxST = 100;
	int32 cp = 10;
	// モンスターの現在のステータス
	Array<int32> curStatus = { 100,100,10,10,10,10 };
	Array<int32> currentDeck;

	bool death = false;

	void ResetData() {
		currentID = 0;
		nextID = 5;
		gen = 0;
		age = 0;
		currentDay = 0; //日にち
		timeOfDay = 1; //時間
		lifespan = 100;
		enemyIdx = 0;

		curST = 100;
		maxST = 100;
		cp = 10;
		death = false;
		// モンスターの現在のステータス
		curStatus = { 100,100,10,10,10,10 };
		currentDeck.clear();
		for (int32 i = 0; i < normalCardInfos.size(); i++) {
			currentDeck.emplace_back(i);
		}
	}

	void ProgressTime(int prog)
	{
		if (prog == 0) return;
		int newTime = timeOfDay + prog;
		int daysPassed = 0;
		while (newTime > timeMax) { newTime -= timeMax; ++daysPassed; }
		while (newTime <= 0) { newTime += timeMax; --daysPassed; }
		timeOfDay = std::clamp(newTime, 1, timeMax);
		currentDay += daysPassed;
		age += daysPassed;
		int32 rand = Random(0, 100);
		if (rand <= age + 50) {
			death = true;
		}
		else {
			death = false;
		}
		// onDayPassed の呼び出し等もここで行う
	}



	void ChangeToNextID() {
		currentID = nextID;
		nextID = statsInfos[currentID].nextEvoID;
		lifespan = statsInfos[currentID].lifespan;
		curST = 100 + (gen - 1) * 50;
		maxST = 100 + (gen - 1) * 50;
		EvolutionStatusIncrease(curStatus, currentID);
	}
	void ChangeToID(int id) {
		currentID = id;
		nextID = statsInfos[currentID].nextEvoID;
		lifespan = statsInfos[currentID].lifespan;
		curST = 100 + (gen - 1) * 50;
		maxST = 100 + (gen - 1) * 50;
		EvolutionStatusIncrease(curStatus, currentID);
	}
	void ChangeToST(UIBar& uiBar, int32 st) {
		if (st >= 0) {
			if (curST + st > maxST) {
				uiBar.heal(maxST - curST);
				curST = maxST;
			}
			else {
				uiBar.heal(st);
				curST += st;
			}
		}
		else {
			if (curST + st < 0) {
				uiBar.damage(curST);
				curST = 0;
			}
			else {
				uiBar.damage(-st);
				curST += st;
			}
		}
	}
	void DeathReset() {
		for (int i = 0; i < curStatus.size(); ++i) {
			curStatus[i] = static_cast<int32>(std::ceil(static_cast<double>(curStatus[i]) * statsInfos[currentID].inheritanceRate[i]));
		}
	}

	void CalculateCP() {
		int32 INTStatus = curStatus[1];

		if (100 > INTStatus) {
			cp = 10;
		}
		else {
			cp = floor(sqrt(INTStatus));
		}
	}
};

using App = SceneManager<State, GameData>;

inline void EvolutionStatusIncrease(Array<int32>& curStatus, int currentID) {
	for (int i = 0; i < curStatus.size(); ++i) {
		curStatus[i] = ceil((double)curStatus[i] * statsInfos[currentID].evoBonusMultiplier[i]);
	}
}

inline void SunDirReset(BasicCamera3D camera) {
	// カメラの位置と注視点を取得
	const Vec3 eye = camera.getEyePosition();       // カメラの位置
	const Vec3 target = camera.getFocusPosition(); // カメラの注視点

	// カメラの「前方向（視線ベクトル）」を求める
	Vec3 camForward = (target - eye).normalized();

	Graphics3D::SetSunDirection(-camForward);
}
