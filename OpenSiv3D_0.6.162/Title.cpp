# include "Title.hpp"

Title::Title(const InitData& init)
	: IScene{ init }
{
	rand = Random(fbx.dragonModels.size() - 1);
	getData().ResetData();
}

void Title::update()
{
	// カメラ更新
	Graphics3D::SetCameraTransform(camera);
	// ボタンの更新
	{
		m_eggTransition.update(m_eggButton.mouseOver());
		//m_tameTransition.update(m_tameButton.mouseOver());
		//m_evoTransition.update(m_evoButton.mouseOver());
		//m_battleTransition.update(m_battleButton.mouseOver());
		m_exitTransition.update(m_exitButton.mouseOver());

		if (m_eggButton.mouseOver()/* || m_tameButton.mouseOver() || m_evoButton.mouseOver()*/ || m_exitButton.mouseOver())
		{
			Cursor::RequestStyle(CursorStyle::Hand);
		}
	}

	if (m_randButton.leftClicked())
	{
		rand = Random(fbx.dragonModels.size() - 1);
	}

	// ボタンのクリック処理
	if (m_eggButton.leftClicked()) // ゲームへ
	{
		changeScene(State::Egg);
	}
	else if (m_tameButton.leftClicked()) // ゲームへ
	{
		changeScene(State::TameScene);
	}
	else if (m_evoButton.leftClicked()) // ランキングへ
	{
		changeScene(State::EvoScene);
	}
	else if (m_deathButton.leftClicked()) // ランキングへ
	{
		changeScene(State::DeathScene);
	}
	else if (m_battleButton.leftClicked()) {
		changeScene(State::BattleScene);
	}
	else if (m_exitButton.leftClicked()) // 終了
	{
		System::Exit();
	}

}

void Title::draw() const
{
	//Scene::SetBackground(ColorF{ 0.2, 0.8, 0.4 });

	// [3D シーンの描画]
	{
		const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };
		// [モデルの描画]
		//titleModel.draw(Vec3{ 0.4, 1.25, -15 }, Quaternion::RotateY(40_deg));
		auto model = fbx.dragonModels[rand];
		model.draw(Vec3{ 0.75, 0.25, -12 }, Quaternion::RotateY(-40_deg));
		model.setAnim(fbx.dragonAnims[0]);
		model.updateFbx(Scene::DeltaTime(), fbx.dragonAnims[0]);
	}

	// [RenderTexture を 2D シーンに描画]
	{
		Graphics3D::Flush();
		renderTexture.resolve();
		Shader::LinearToScreen(renderTexture);
	}


	// -----------------------------
	// ここから 2D（UI）描画。3D の上に重ねる
	// -----------------------------
	const Vec2 cursor = Cursor::PosF();

	// 半透明の背景パネル（タイトル領域）
	//RoundRect{ center.x - 620.0, center.y - 260.0, 840.0, 360.0, 24.0 }.draw(ColorF{ 0.0, 0.0, 0.0, 0.45 });

	// タイトルテキスト
	FontAsset(U"TitleFont")(U"進竜蘇心！").drawAt(center.x - 300, center.y - 150.0, ColorF{ 1.0 }.removeSRGBCurve());

	// ボタン描画
	{
		m_randButton.draw(ColorF{ 1.0, 0.8 }).drawFrame(2);
		m_eggButton.draw(ColorF{ 1.0, m_eggTransition.value() }).drawFrame(2);
		//m_tameButton.draw(ColorF{ 1.0, m_tameTransition.value() }).drawFrame(2);
		//m_evoButton.draw(ColorF{ 1.0, m_evoTransition.value() }).drawFrame(2);
		//m_deathButton.draw(ColorF{ 1.0, m_deathTransition.value() }).drawFrame(2);
		//m_battleButton.draw(ColorF{ 1.0, m_battleTransition.value() }).drawFrame(2);
		m_exitButton.draw(ColorF{ 1.0, m_exitTransition.value() }).drawFrame(2);

		const Font& boldFont = FontAsset(U"Bold");
		boldFont(U"Start").drawAt(36, m_eggButton.center(), ColorF{ 0.1 });
		//boldFont(U"GAME").drawAt(36, m_tameButton.center(), ColorF{ 0.1 });
		//boldFont(U"TEST").drawAt(36, m_rankingButton.center(), ColorF{ 0.1 });
		//boldFont(U"Evo").drawAt(36, m_evoButton.center(), ColorF{ 0.1 });
		//boldFont(U"Death").drawAt(36, m_deathButton.center(), ColorF{ 0.1 });
		//boldFont(U"BATTLE").drawAt(36, m_battleButton.center(), ColorF{ 0.1 });
		boldFont(U"EXIT").drawAt(36, m_exitButton.center(), ColorF{ 0.1 });
	}
}
