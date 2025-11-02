# include "DeathScene.hpp"

DeathScene::DeathScene(const InitData& init)
	: IScene{ init }
{
	modelIdx = getData().currentID;
	audioManager.deathAudio.play();
}

void DeathScene::update()
{
	// カメラ更新
	Graphics3D::SetCameraTransform(camera);


	// ボタンのクリック処理
	if (MouseL.down()) // ゲームへ
	{
		if (deathDone) {
			audioManager.deathAudio.stop(2s);
			changeScene(State::Egg);
			return;
		}

		deathDone = true;
		auto model = fbx.dragonModels[modelIdx];
		model.setOneShotAnim(fbx.dragonAnims[4]);
		auto& data = getData();
		data.DeathReset();
	}

	groundDeg -= Scene::DeltaTime() * 2;
}

void DeathScene::draw() const
{
	// [3D シーンの描画]
	{
		const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };
		groundPlane.draw(Vec3{ 0,-0.5,0 }, Quaternion::RotateY(Math::ToRadians(groundDeg)), groundTexture);

		// [モデルの描画]
		auto model = fbx.dragonModels[modelIdx];
		model.draw(Vec3{ 0,-0.5,0 }, Quaternion::RotateY(currentAngleDeg * 1.0_deg));
		model.updateFbx(Scene::DeltaTime());
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
	FontAsset(U"TitleFont")(U"DEATH").drawAt(center.x - 250, center.y - 260.0, ColorF{ 1.0 }.removeSRGBCurve());

}
