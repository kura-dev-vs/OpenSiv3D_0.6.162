# include "EvoScene.hpp"

EvoScene::EvoScene(const InitData& init)
	: IScene{ init }
{
	modelIdx = getData().currentID;
	beforeIdx = getData().currentID;
	afterIdx = getData().nextID;
	SunDirReset(camera);
	audioManager.evoAudio.play();
}

void EvoScene::update()
{
	// カメラ更新
	Graphics3D::SetCameraTransform(camera);


	// ボタンのクリック処理
	if (MouseL.down()) // ゲームへ
	{
		if (evoDone) {
			audioManager.evoAudio.stop(2s);
			changeScene(State::TameScene);
			return;
		}

		rot.Start(-100.0, 4500.0, 300.0, 400.0);
		evoDone = true;
		getData().ChangeToNextID();
		currentTime = 0;
	}

	if (evoDone) {
		currentAngleDeg = rot.UpdateAndGetAngle();
	}
	groundDeg += Scene::DeltaTime() * 5;

	if (rot.IsMaxReached())
		modelIdx = afterIdx;
	else
		modelIdx = beforeIdx;

	if (rot.IsFinished())
		currentTime += Scene::DeltaTime();
}

void EvoScene::draw() const
{
	// [3D シーンの描画]
	{
		const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };
		groundPlane.draw(Vec3{ 0,-1.0,0 }, Quaternion::RotateY(Math::ToRadians(groundDeg)), groundTexture);

		// [モデルの描画]
		auto model = fbx.dragonModels[modelIdx];
		model.draw(Vec3{ 0,-0.5,0 }, Quaternion::RotateY(currentAngleDeg * 1.0_deg));
		if (rot.IsFinished() && currentTime >= 0.5f) {
			model.setAnim(fbx.dragonAnims[2]);
			model.updateFbx(Scene::DeltaTime(), fbx.dragonAnims[2]);
		}
		else {
			model.setAnim(fbx.dragonAnims[0]);
			model.updateFbx(Scene::DeltaTime(), fbx.dragonAnims[0]);
		}
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
	FontAsset(U"TitleFont")(U"EVOLUTION").drawAt(center.x - 250, center.y - 260.0, ColorF{ 1.0 }.removeSRGBCurve());

}
