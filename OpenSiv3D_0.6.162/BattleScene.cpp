#include "BattleScene.hpp"

BattleScene::BattleScene(const InitData& init)
	: IScene{ init }
{
	playerModelIdx = getData().currentID;
	enemyModelIdx = getData().enemyIdx;
	playerModel = fbx.dragonModels[playerModelIdx];
	enemyModel = fbx.enemyModels[enemyModelIdx];

	currentPlayerStatus = getData().curStatus;
	currentEnemyStatus = enemyInfos[enemyModelIdx].status;
	getData().CalculateCP();
	maxCP = getData().cp;
	curCP = maxCP;
	playerCurHP = getData().curStatus[0];
	playerMaxHP = getData().curStatus[0];
	playerHPBar = UIBar{ playerMaxHP, playerCurHP };
	enemyCurHP = currentEnemyStatus[0];
	enemyMaxHP = currentEnemyStatus[0];
	enemyHPBar = UIBar{ enemyMaxHP,enemyCurHP };
	SunDirReset(camera);
	playerModel.setAnim(fbx.dragonAnims[playerAnimIdx]);
	enemyModel.setAnim(fbx.enemyAnims[playerAnimIdx]);


	deckCardMap.clear();
	for (int32 i = 0; i < getData().currentDeck.size(); i++) {
		deckCardMap.emplace_back(CardTransform(getData().currentDeck[i]));
	}

	for (int i = 0; i < deckCardMap.size(); i++) {
		deckCardMap[i].InitLaneTransform(i);
		deckCardMap[i].cardPos = deckCardMap[i].laneCardPos;
	}
	ShuffleDeckAndCompactLanes();
	audioManager.battleAudio.play();
}


void BattleScene::update()
{
	Graphics3D::SetCameraTransform(camera);
	cursor = Cursor::PosF();

	playerHPBar.update();
	enemyHPBar.update();

	if (enemyDeath) {
		playerWinFlag = true;
		if (getData().enemyIdx != 2) {
			if (MouseL.down()) // ゲームへ
			{
				getData().enemyIdx++;
				audioManager.battleAudio.stop(2s);
				changeScene(State::TameScene);
				return;
			}
		}
		else {
			if (MouseL.down()) // ゲームへ
			{
				audioManager.battleAudio.stop(2s);
				changeScene(State::Title);
				return;
			}
			return;
		}

	}
	else {
		if (playerCurHP <= 0) {
			playerDeath = true;
			playerModel.setOneShotAnim(fbx.dragonAnims[4]);
			if (MouseL.down()) // ゲームへ
			{
				audioManager.battleAudio.stop(2s);
				changeScene(State::BattleScene);
			}
			else if (MouseR.down()) // ゲームへ
			{
				audioManager.battleAudio.stop(2s);
				changeScene(State::Title);
			}
			return;
		}
		if (enemyCurHP <= 0) {
			enemyDeath = true;
			playerWinFlag = true;
			playerModel.setAnim(fbx.dragonAnims[2]);
			enemyModel.setOneShotAnim(fbx.enemyAnims[4]);
			return;
		}

	}



	if (attackBtn.update() == true) {
		PlayerAttackTurn();
	}

	// 更新：生きているものだけ残す
	for (auto it = damages.begin(); it != damages.end();)
	{
		it->update(Scene::DeltaTime());
		if (!it->alive())
		{
			it = damages.erase(it);
		}
		else
		{
			++it;
		}
	}

	curInterval += Scene::DeltaTime();
	if (curInterval >= enemyInterval) {
		curInterval = 0;
		EnemyAction();
	}

	if (calcuratePlayerTurn)
		return;

	CardOperation();
}

void BattleScene::draw() const
{
	// 3D 描画
	{
		const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };
		// 背景クリアと地面
		Scene::SetBackground(backgroundColor);
		groundPlane.draw(groundTexture);
		//, fbx.bindRot
		playerModel.draw(Vec3{ -1.5, 0.5, -10 }, Quaternion::RotateY(Math::ToRadians(-160.0)));
		playerModel.updateFbx(Scene::DeltaTime());

		//auto enemyModel = fbx.enemyModels[enemyModelIdx];
		enemyModel.draw(Vec3{ 1.5, 0, -10 }, Quaternion::RotateY(Math::ToRadians(40.0)));
		enemyModel.updateFbx(Scene::DeltaTime());
	}

	// [RenderTexture を 2D シーンに描画]
	{
		Graphics3D::Flush();
		renderTexture.resolve();
		Shader::LinearToScreen(renderTexture);
	}

	// 2D UI オーバーレイ
	{
		RoundRect{ Arg::center(cpPos),145, 70,10 }.draw(ColorF{ 0.1, 0.1, 0.1, 0.8 });
		RoundRect{ Arg::center(cpPos),135, 60,10 }.drawFrame(3, ColorF{ 0.9, 0.9, 0.9, 1.0 });
		titleFont(U"{}/{}"_fmt(curCP, maxCP)).draw(Arg::center(cpPos), Palette::Darkorange);

		RoundRect{ Arg::center(pStatsPos),100, 120,10 }.draw(ColorF{ 0.1, 0.1, 0.1, 0.8 });
		RoundRect{ Arg::center(pStatsPos),95, 110,10 }.drawFrame(2, ColorF{ 0.9, 0.9, 0.9, 1.0 });

		RoundRect{ Arg::center(eStatsPos),100, 120,10 }.draw(ColorF{ 0.1, 0.1, 0.1, 0.8 });
		RoundRect{ Arg::center(eStatsPos),95, 110,10 }.drawFrame(2, ColorF{ 0.9, 0.9, 0.9, 1.0 });
		for (int i = 0; i < 6; i++) {
			statsFont(starNames[i]).draw(Arg::center(pStatsPos + Vec2{ -15,-40 + i * 15 }), Palette::White);
			statsFont(currentPlayerStatus[i]).draw(Arg::center(pStatsPos + Vec2{ 15,-40 + i * 15 }), Palette::White);
			statsFont(starNames[i]).draw(Arg::center(eStatsPos + Vec2{ -15,-40 + i * 15 }), Palette::White);
			statsFont(currentEnemyStatus[i]).draw(Arg::center(eStatsPos + Vec2{ 15,-40 + i * 15 }), Palette::White);
		}

		//handRect.draw(ColorF{ 1.0, 0.3, 0.3, 0.3 });
		laneRect.draw(ColorF{ 0.1, 0.1, 0.1, 0.8 });

		attackBtn.draw();
		playerHPBar.draw(pBarRect, Palette::Lightgreen);
		enemyHPBar.draw(eBarRect);

		for (auto& card : deckCardMap) {
			if (!card.isUsed)
				CardDraw(card, templ);
		}

		for (const auto& d : damages)
		{
			d.draw(m_font);
		}

		if (enemyDeath) {
			if (getData().enemyIdx == 2) {
				Rect{ Arg::center(windowSize.x / 2 , windowSize.y / 2), windowSize.x, windowSize.y }.draw(ColorF{ 0.1, 0.1, 0.1, 0.8 });
				titleFont(U"クリア！").draw(Arg::center(windowSize.x / 2, windowSize.y / 2), ColorF{ 1.0 }.removeSRGBCurve());
				labelFont(U"右クリックでタイトルに戻ります").draw(Arg::center(windowSize.x / 2, windowSize.y / 2 + 50), ColorF{ 1.0 }.removeSRGBCurve());
			}
		}

		if (playerDeath) {
			Rect{ Arg::center(windowSize.x / 2 , windowSize.y / 2), windowSize.x, windowSize.y }.draw(ColorF{ 0.1, 0.1, 0.1, 0.8 });
			titleFont(U"敗北！").draw(Arg::center(windowSize.x / 2, windowSize.y / 2), ColorF{ 1.0 }.removeSRGBCurve());
			labelFont(U"左クリックで再戦、右クリックでタイトルに戻ります").draw(Arg::center(windowSize.x / 2, windowSize.y / 2 + 50), ColorF{ 1.0 }.removeSRGBCurve());

		}
	}
}

static void removeIdFromVector(Array<int32>& vec, int id) {
	auto it = std::find(vec.begin(), vec.end(), id);
	if (it != vec.end()) vec.erase(it);
}

void BattleScene::ExtendedTime(double effectValue) {
	for (auto& card : deckCardMap) {
		if (card.inHand) {
			double t = card.restTime + effectValue;
			if (t > normalCardInfos[card.cardID].timeLimit)
				card.restTime = normalCardInfos[card.cardID].timeLimit;
			else
				card.restTime += effectValue;
		}
	}
}


void BattleScene::CardOperation() {
	Array<char> assigned(deckCardMap.size(), 0);
	// 1) --- handOrder に基づき「見た目上の手札位置」を先に確定する ---
	for (size_t idx = 0; idx < handOrder.size(); ++idx) {
		int id = handOrder[idx];

		int foundIndex = -1;
		for (int i = 0; i < static_cast<int>(deckCardMap.size()); ++i) {
			if (assigned[i]) continue;                 // 既に handOrder の別エントリで割当て済み
			if (deckCardMap[i].isUsed) continue;      // 既に使用済みなら除外
			if (deckCardMap[i].instanceID == id) {
				foundIndex = i;
				break;
			}
		}
		if (foundIndex == -1) continue; // 見つからなければスキップ

		assigned[foundIndex] = 1;
		CardTransform& card = deckCardMap[foundIndex];
		/*
		// deckCardMap 内の対応カードを検索（カード数が少なければ線形検索で十分）
		auto it = std::find_if(deckCardMap.begin(), deckCardMap.end(),
			[&](const CardTransform& c) { return c.cardID == id; });
		if (it == deckCardMap.end()) continue;
		CardTransform& card = *it;
		*/
		card.handIdx = static_cast<int>(idx);
		card.HandTransform(); // handIdx を使って handCardPos を計算する想定
		// 手札にあるカードでドラッグしていないなら表示位置も handCardPos に合わせる
		if (!card.dragging) {
			card.cardPos = card.handCardPos;
		}
		card.inHand = true; // handOrder に載っている限り inHand は true とみなす
	}

	// 非手札カードは inHand = false（handOrder にないもの）
	for (auto& card : deckCardMap) {
		if (!card.inHand) {
			if (!card.dragging) {
				card.cardPos = card.laneCardPos;
				card.handIdx = -1;
			}
		}
		/*
		if (std::find(handOrder.begin(), handOrder.end(), card.cardID) == handOrder.end()) {
			card.inHand = false;
			if (!card.dragging) {
				card.cardPos = card.laneCardPos;
				card.handIdx = -1;
			}
		}
		*/
	}

	// 2) --- Mouse down: 画面前面優先（逆順）で一枚だけ掴む（判定には"見た目位置"を使う） ---
	if (MouseL.down()) {
		for (int i = static_cast<int>(deckCardMap.size()) - 1; i >= 0; --i) {
			CardTransform& card = deckCardMap[i];

			if (card.isUsed) continue;


			// このカードの「見た目上の位置」を求める
			Vec2 effectivePos;
			if (card.dragging) effectivePos = card.cardPos;
			else if (card.inHand) effectivePos = card.handCardPos;
			else effectivePos = card.laneCardPos;

			RectF cardRect = templ.FullRect().movedBy(effectivePos);
			if (cardRect.contains(cursor)) {
				// 他の dragging をクリアしてから一枚だけ掴む
				for (auto& c : deckCardMap) c.dragging = false;

				card.dragging = true;
				card.dragOffset = cursor - effectivePos;
				// card.cardPos を即座に effectivePos に合わせておく（ずれ防止）
				card.cardPos = effectivePos;

				break; // 1枚だけ掴む
			}

		}
	}

	// 3) --- ドラッグ中のカード位置更新（押している間） ---
	for (auto& card : deckCardMap) {
		if (card.dragging && MouseL.pressed()) {
			card.cardPos = cursor - card.dragOffset;
		}
	}

	// 4) --- Mouse up: ドロップ判定と handOrder の更新（in/out） ---
	if (MouseL.up()) {
		for (auto& card : deckCardMap) {
			if (!card.dragging) continue;

			card.dragging = false;
			bool wasInHand = card.inHand;

			if (handRect.contains(cursor)) {
				// 手札領域に落とした -> 手札に入れる（入っていなければ末尾に追加：入れた順を保つ）
				if (normalCardInfos[card.cardID].type == CardType::ExtendTime) {
					//if (handOrder.empty()) continue;
					card.isUsed = true;
					ExtendedTime(normalCardInfos[card.cardID].effectValue);
				}
				else {
					if (!wasInHand) {
						if (normalCardInfos[card.cardID].cp <= curCP) {
							curCP -= normalCardInfos[card.cardID].cp;
							//handOrder.push_back(card.cardID);
							handOrder.push_back(card.instanceID);
							card.inHand = true;
						}
					}

					// ドロップ後は handCardPos に合わせる（HandTransform が handIdx を参照するため、
					// handIdx を更新して HandTransform() を呼ぶ必要がある。ここは次フレームに反映しても良い）
				}
			}
			else {
				// 手札領域外に落とした -> 手札から外す（手札リストから削除）
				if (wasInHand) {
					curCP += normalCardInfos[card.cardID].cp;
					//removeIdFromVector(handOrder, card.cardID);
					removeIdFromVector(handOrder, card.instanceID);
					card.inHand = false;
					card.handIdx = -1;
				}
				// 位置は lane に戻す
				card.cardPos = card.laneCardPos;
			}
		}
	}

	// 5) --- handCards（描画用のIDリスト）を handOrder に合わせる ---
	//handCards = handOrder; // 必要ならコピーで扱う

	// 5) handCards（描画用のIDリスト）を handOrder に合わせる
	// handCards が描画で cardID を参照する配列ならここは instance -> cardID に変換する
	handCards.clear();
	for (int instId : handOrder) {
		// instId は instanceID、対応する cardID を探す
		for (auto& c : deckCardMap) {
			if (c.instanceID == instId) {
				handCards.push_back(c.cardID); // handCards は cardID の配列のままならこうする
				break;
			}
		}
	}

	// 6) --- laneCardPos の継続的移動（副作用のない書き方） ---
	const float dx = Scene::DeltaTime() * laneSpeed;
	for (auto& card : deckCardMap) {
		card.laneCardPos.x += dx;
		if (card.inHand) {
			card.restTime -= Scene::DeltaTime();
			if (card.restTime < 0) {
				card.isUsed = true;
				//removeIdFromVector(handOrder, card.cardID);
				removeIdFromVector(handOrder, card.instanceID);
				card.inHand = false;
				card.handIdx = -1;

			}
		}
	}
}

// --- シャッフルして isUsed を除いて lane を詰める関数 ---
void BattleScene::ShuffleDeckAndCompactLanes()
{
	int32 canUseCount = 0;
	for (auto& card : deckCardMap) {
		if (!card.isUsed) {
			canUseCount++;
			break;
		}

	}
	if (canUseCount == 0) {
		for (auto& card : deckCardMap) {
			if (card.isUsed) {
				card.isUsed = false;
				card.inHand = false;
			}

		}
	}

	// 1) deckCardMap をランダムに入れ替える
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(deckCardMap.begin(), deckCardMap.end(), g);

	// 2) シャッフル後、isUsed == true のカードはレーンの詰め対象外にして
	//    残りのカードに対して順番に InitLaneTransform を呼ぶ
	int laneIdx = 0;
	for (auto& card : deckCardMap) {
		if (card.isUsed) {
			// 使用済みカードはレーンに表示したくないので見やすい場所へ退避させる（任意）
			// 例えば画面外に移す、あるいは usedAreaPos に移動するなど
			card.laneCardPos = Vec2(-10000.0, -10000.0); // 非表示にする簡単な方法
			// 手札でもない・ドラッグでもないなら表示位置も退避させる
			if (!card.inHand && !card.dragging) {
				card.cardPos = card.laneCardPos;
			}
		}
		else {
			// laneIdx に基づき laneCardPos を設定
			card.InitLaneTransform(laneIdx++);
			// もし手札に入っていなければ即座に表示位置を laneCardPos に揃える（またはアニメーション）
			if (!card.inHand && !card.dragging) {
				card.cardPos = card.laneCardPos; // 即座に移動させる場合
				// なめらかに移動させたい場合はアニメーション処理を別で行う（下に例あり）
			}
		}
	}

	// 3) handOrder を instanceID ベースで管理しているなら、handOrder と handCards の整合性は必要に応じて更新
	// （通常は次フレーム CardOperation() 側で handOrder に基づいて handIdx 等が更新される）
}


void BattleScene::PlayerAttackTurn() {
	calcuratePlayerTurn = true;
	for (int i = handCards.size() - 1; i >= 0; i--) {
		PlayerAction(normalCardInfos[handCards[i]]);
		//Print << handOrder[i];
	}

	for (auto& card : deckCardMap) {
		if (card.inHand) {
			card.isUsed = true;
			card.inHand = false;
		}
	}
	handOrder.clear();
	curCP = maxCP;
	calcuratePlayerTurn = false;
	EnemyAction();

	ShuffleDeckAndCompactLanes();
}

void BattleScene::PlayerAction(CardInfo ci) {
	int32 heal;
	switch (ci.type) {
	case CardType::Attack:
		playerModel.setOneShotAnim(fbx.dragonAnims[5], fbx.dragonAnims[0]);
		ChangeToHP(enemyHPBar, -PlayerAttackCalc(ci), enemyCurHP, enemyMaxHP);
		break;
	case CardType::Buf:
		playerModel.setOneShotAnim(fbx.dragonAnims[3], fbx.dragonAnims[0]);
		currentPlayerStatus[ci.statusIdx] *= ci.effectValue;
		break;
	case CardType::Debuf:
		playerModel.setOneShotAnim(fbx.dragonAnims[3], fbx.dragonAnims[0]);
		currentEnemyStatus[ci.statusIdx] *= ci.effectValue;
		break;
	case CardType::Heal:
		heal = playerMaxHP * ci.effectValue / 100;
		ChangeToHP(playerHPBar, heal, playerCurHP, playerMaxHP);
		break;
	default:
		break;
	}
}


void BattleScene::EnemyAction() {
	Array<EnemySkillInfo> eSIs = enemyInfos[enemyModelIdx].enemySkillInfos;
	int32 rand = Random(eSIs.size() - 1);
	//int32 rand = 0;
	EnemySkillInfo es = eSIs[rand];

	switch (es.type) {
	case CardType::Attack:
		enemyModel.setOneShotAnim(fbx.enemyAnims[2], fbx.enemyAnims[0]);
		ChangeToHP(playerHPBar, -EnemyAttackCalc(es), playerCurHP, playerMaxHP);
		break;
	case CardType::Buf:
		enemyModel.setOneShotAnim(fbx.enemyAnims[3], fbx.enemyAnims[0]);
		currentEnemyStatus[es.statusIdx] *= es.effectValue;
		break;
	case CardType::Debuf:
		enemyModel.setOneShotAnim(fbx.enemyAnims[3], fbx.enemyAnims[0]);
		currentPlayerStatus[es.statusIdx] *= es.effectValue;
		break;
	default:
		break;
	}
	enemyInterval = es.actionInterval;
	curInterval = 0;
}

void BattleScene::ChangeToHP(UIBar& uiBar, int32 hp, int32& curHP, int32& maxHP) {
	if (hp >= 0) {
		if (curHP + hp > maxHP) {
			uiBar.heal(maxHP - curHP);
			curHP = maxHP;
		}
		else {
			uiBar.heal(hp);
			curHP += hp;
		}
	}
	else {
		if (curHP + hp < 0) {
			uiBar.damage(curHP);
			curHP = 0;
		}
		else {
			uiBar.damage(-hp);
			curHP += hp;
		}
	}
}

int32 BattleScene::PlayerAttackCalc(CardInfo cI) {
	double skillMul = 0.5 + 1.5 * (cI.effectValue / 100);
	int32 effective_def = Max(1, currentEnemyStatus[cI.statusIdx + 1]);
	int32 rawDam = ceil(Base * (currentPlayerStatus[cI.statusIdx] / effective_def) * skillMul);

	int32 damage = Max(1, rawDam);
	Vec2 pos = Vec2(100, 120);
	damages.emplace_back(pos, damage);
	return damage;
};

int32 BattleScene::EnemyAttackCalc(EnemySkillInfo eS) {
	double skillMul = 0.5 + 1.5 * (eS.effectValue / 100);
	int32 effective_def = Max(1, currentPlayerStatus[eS.statusIdx + 1]);
	int32 rawDam = ceil(Base * (currentEnemyStatus[eS.statusIdx] / effective_def) * skillMul);

	int32 damage = Max(1, rawDam);
	Vec2 pos = Vec2(800, 120);
	damages.emplace_back(pos, damage);
	return damage;
};

// ヘルパー: レーン上のカードだけを順番に並べ直す
void BattleScene::RecalculateLanePositions(Array<CardTransform>& cards) {
	int laneIdx = 0;
	for (auto& c : cards) {
		if (!c.inHand && !c.isUsed) {
			c.InitLaneTransform(laneIdx++);   // laneCardPos を更新
			// 位置は即座に反映させる（アニメーションさせるなら lerp 等を使う）
			c.cardPos = c.laneCardPos;
			c.handIdx = -1;
		}
	}
}

// ヘルパー: 手札に入っているカードの handIdx を再割り当てして位置を決める
void BattleScene::RecalculateHandPositions(Array<CardTransform>& cards) {
	int handIndex = 0;
	for (auto& c : cards) {
		if (c.inHand && !c.isUsed) {
			c.handIdx = handIndex++;
			c.HandTransform();                // handCardPos を更新
			c.cardPos = c.handCardPos;
		}
	}
}
