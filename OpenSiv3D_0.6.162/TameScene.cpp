#include "TameScene.hpp"

TameScene::TameScene(const InitData& init)
	: IScene{ init }
{
	uiBar = UIBar{ getData().maxST, getData().curST };
	modelIdx = getData().currentID;
	model = fbx.dragonModels[modelIdx];
	model.setAnim(fbx.dragonAnims[animIdx]);
	SunDirReset(camera);
	audioManager.tameAudio.play();
}

void TameScene::update()
{
	Graphics3D::SetCameraTransform(camera);

	uiBar.update();

	switch (curTameActState) {
	case TameActState::メイン:
		for (auto& button : tameActBtns)
		{
			if (button.update() == true) {
				curTameActState = static_cast<TameActState>(&button - &tameActBtns[0] + 1);
			}
		}
		break;
	case TameActState::訓練:
		for (auto& button : trainButtons)
		{
			if (button.update() == true) {
				training(button.m_trainIdx);
			}
		}
		// 右クリックされたら
		if (MouseR.down())
		{
			curTameActState = static_cast<TameActState>(0);
		}
		break;
	case TameActState::交流:
		communication();
		curTameActState = static_cast<TameActState>(0);
		break;
	case TameActState::寝かせる:
		sleep();
		animIdx = 1;
		model.setOneShotAnim(fbx.dragonAnims[animIdx], fbx.dragonAnims[0]);
		audioManager.tameAudio.stop(2s);
		changeScene(State::TameScene);
		break;
	default:
		break;
	}


	if (getData().age >= getData().lifespan) {
		if (getData().death) {
			audioManager.tameAudio.stop(2s);
			changeScene(State::DeathScene);
		}
	}

	if (getData().nextID >= fbx.dragonModels.size())
		return;

	if (getData().age >= statsInfos[getData().nextID].enableEvoAge) {
		int32 condClearStatus = 0;
		for (int i = 0; i < 6; i++) {
			if (getData().curStatus[i] >= statsInfos[getData().nextID].evolutionTrigger[i])
			{
				condClearStatus++;
			}
		}
		if (condClearStatus >= statsInfos[getData().nextID].evolutionTrigger[6]) {
			audioManager.tameAudio.stop(2s);
			changeScene(State::EvoScene);
		}
	}

	if (getData().currentDay >= getData().battleSchedule[getData().enemyIdx]) {
		if (getData().currentDay > getData().battleSchedule[getData().enemyIdx]) {
			audioManager.tameAudio.stop(2s);
			changeScene(State::BattleScene);
		}
		else {
			if (getData().timeOfDay >= getData().battleTime) {
				audioManager.tameAudio.stop(2s);
				changeScene(State::BattleScene);
			}
		}
	}

}

void TameScene::draw() const
{
	// 3D 描画
	{
		const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };
		// 背景クリアと地面
		Scene::SetBackground(backgroundColor);
		groundPlane.draw(groundTexture);
		//, fbx.bindRot
		model.draw(Vec3{ 0, 0.5, -10 }, Quaternion::RotateY(Math::ToRadians(-80.0)));
		model.updateFbx(Scene::DeltaTime());
	}

	// [RenderTexture を 2D シーンに描画]
	{
		Graphics3D::Flush();
		renderTexture.resolve();
		Shader::LinearToScreen(renderTexture);
	}

	// 2D UI オーバーレイ
	{
		// 現在の選択表示
		const Vec2 statusPos = Vec2{ 1100, 20 };
		RoundRect{ Arg::center(1150,200),200, 375,10 }.draw(ColorF{ 0.1, 0.1, 0.1, 0.8 });
		RoundRect{ Arg::center(1150,200),190, 365,10 }.drawFrame(3, ColorF{ 0.9, 0.9, 0.9, 1.0 });
		for (int i = 0; i < 6; i++) {
			titleFont(starNames[i]).draw(Arg::center(statusPos + Vec2{ 0,50 + i * 50 }), Palette::White);
			titleFont(getData().curStatus[i]).draw(Arg::center(statusPos + Vec2{ 75,50 + i * 50 }), Palette::White);
		}

		const Size windowSize = Scene::Size();
		int h = windowSize.y / 2.25;
		Rect{ Arg::center(windowSize.x / 2 , windowSize.y), windowSize.x, h }.draw(ColorF{ 0.1, 0.1, 0.1, 0.8 });
		Rect{ Arg::center(windowSize.x / 2 , windowSize.y), windowSize.x, h - 10 }.drawFrame(3, ColorF{ 0.9, 0.9, 0.9, 1.0 });
		{
			titleFont(getData().age).draw(Arg::leftCenter(175, 600), Palette::White);
			titleFont(U"才").draw(Arg::leftCenter(210, 600), Palette::White);
			titleFont(getData().currentDay).draw(Arg::leftCenter(250, 600), Palette::White);
			titleFont(U"日").draw(Arg::leftCenter(285, 600), Palette::White);
			titleFont((getData().timeOfDay - 1) * 2).draw(Arg::leftCenter(325, 600), Palette::White);
			titleFont(U"時").draw(Arg::leftCenter(360, 600), Palette::White);

			titleFont(U"次の戦闘:").draw(Arg::leftCenter(600, 600), Palette::White);
			titleFont(getData().battleSchedule[getData().enemyIdx]).draw(Arg::leftCenter(725, 600), Palette::White);
			titleFont(U"日").draw(Arg::leftCenter(775, 600), Palette::White);

		}

		titleFont(U"行動力: ").draw(barRect.movedBy(-120, 0), Palette::White);
		uiBar.draw(barRect, Palette::Lightgreen);

		switch (curTameActState) {
		case TameActState::メイン:
			for (auto& button : tameActBtns)
			{
				button.draw();
			}
			break;
		case TameActState::訓練:
			for (const auto& button : trainButtons)
			{
				button.draw();
			}
			break;
		case TameActState::交流:
			break;
		case TameActState::寝かせる:
			break;
		default:
			break;
		}

	}
}

void TameScene::training(int32 trainIdx) {
	if (getData().curST <= 20)
		return;
	for (int i = 0; i < 6; i++) {
		getData().curStatus[i] += ceil(trainingInfos[trainIdx].increaseAmounts[i] * statsInfos[modelIdx].increaseModifier[i]);
	}
	getData().ProgressTime(1);
	getData().ChangeToST(uiBar, -15);
	//getData().curST - 20;
	//uiBar.damage(20);
}

void TameScene::communication() {
	int32 rand = Random(0, 100);
	if (rand >= 40) {
		getData().ChangeToST(uiBar, 30 + (getData().gen - 1) * 30);
		model.setOneShotAnim(fbx.dragonAnims[2], fbx.dragonAnims[0]);
	}
	else {
		getData().ChangeToST(uiBar, -10);
		model.setOneShotAnim(fbx.dragonAnims[5], fbx.dragonAnims[0]);
	}
	getData().ProgressTime(1);
}
void TameScene::sleep() {
	//getData().ProgressTime(36);
	getData().ProgressTime(5);
	getData().ChangeToST(uiBar, 70 + (getData().gen - 1) * 50);
	//uiBar.heal(50);
	//getData().curST + 50;
	//uiBar.damage(20);
}
