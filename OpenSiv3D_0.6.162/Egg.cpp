#include "Egg.hpp"

Egg::Egg(const InitData& init)
	: IScene{ init }
{
	for (int i = 0; i < N; ++i)
	{
		fbx.eggModels[i].draw();
		fbx.eggModels[i].setBindPose(fbx.eggModels[i].getAnim(0));
		fbx.eggModels[i].updateFbx(Scene::DeltaTime());
	}
	audioManager.eggAudio.play();
}

void Egg::update()
{
	// --- （イージングはそのまま） ---
	const double dt = Scene::DeltaTime();
	double diff = targetAngleDeg - currentAngleDeg;

	// 正しい最短回転 (角度差を -180..180 に)
	diff = std::fmod(diff, 360.0);
	if (diff > 180.0) diff -= 360.0;
	if (diff <= -180.0) diff += 360.0;

	// 滑らかに追従
	currentAngleDeg += diff * std::min(1.0, rotateSpeed * dt);

	// しきい値で切り捨て（小さくなったら目標に合わせる）
	if (std::abs(diff) < 0.01)
		currentAngleDeg = targetAngleDeg;

	// --- カメラ位置を currentAngleDeg に応じて決定 ---
	const double rad = Math::ToRadians(currentAngleDeg);
	const Vec3 cameraPos = Vec3{ cameraRadius * std::sin(rad), cameraHeight, -cameraRadius * std::cos(rad) };

	// カメラを cameraPos から原点を見るように設定（これで選択中のモデルが真正面に来る）

	camera.setView(cameraPos, Vec3{ 0,-0.5,0 }, Vec3::UnitY());
	Graphics3D::SetCameraTransform(camera);
	Graphics3D::SetSunDirection(cameraPos.normalized());
	groundDeg += Scene::DeltaTime() * 2;

	// ボタンの更新
	if (eggBtn.update() == true) {
		isAnim = true;
		bindRot = Quaternion::RotateX(Math::ToRadians(0.0));
		//changeScene(State::Egg);
		auto& data = getData();
		data.gen++;
		data.age = 0;
		data.death = false;
		data.ChangeToID(eggInfos[index].nextID);

	}

	if (isAnim) {
		fbx.eggModels[index].setAnimIdx(0, false);
		fbx.eggModels[index].updateFbx(Scene::DeltaTime());
		curTime += Scene::DeltaTime();
		if (curTime >= 5.0f)
		{
			audioManager.eggAudio.stop(2s);
			changeScene(State::TameScene);
		}
		return;
	}



	// キーボードでも確認できるように左右キーを割当て
	if (KeyRight.down())
	{
		index = (index + 1) % N;
		targetAngleDeg += step;
	}
	if (KeyLeft.down())
	{
		index = (index - 1 + N) % N;
		targetAngleDeg -= step;
	}

	if (eggBtn.getMouseOver())
		return;

	// 入力：左右クリック（画面の左右判定）
	if (MouseL.down())
	{
		const double mx = Cursor::PosF().x;
		if (mx > Scene::Width() * 0.5)
		{
			// 右へ（次へ） -> target を「相対で」増やす
			index = (index + 1) % N;
			targetAngleDeg += step; // <<--- 重要: 絶対値に設定しない
		}
		else
		{
			// 左へ（前へ） -> target を「相対で」減らす
			index = (index - 1 + N) % N;
			targetAngleDeg -= step; // <<--- 重要
		}
	}
}

void Egg::draw() const
{
	// 3D 描画
	{
		const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };
		// 背景クリアと地面
		Scene::SetBackground(backgroundColor);
		groundPlane.draw(Vec3{ 0,0,0 }, Quaternion::RotateY(Math::ToRadians(groundDeg)), groundTexture);

		// 各モデルを配置・描画
		for (int i = 0; i < N; ++i)
		{
			const double angleDeg = i * (360.0 / N);
			const double a = Math::ToRadians(angleDeg);
			const Vec3 pos = Vec3{ modelRadius * std::sin(a), 0.0, -modelRadius * std::cos(a) };

			Vec3 dir = pos;                     // center = (0,0,0) の場合
			dir.normalize();                    // dir が単位ベクトルになるように

			// Y軸回転（ラジアン）を計算：atan2(x, z) を使う（x が左右、z が前後）
			//double yawRad = std::atan2(dir.x, dir.z);

			// モデルのデフォルト前方向が -Z（多くのケース）ならそのままでOK。
			// 現状「内側を向いている」なら 180度(π)回転させて反転
			//yawRad += Math::ToRadians(180.0);
			//Quaternion rot = Quaternion::RotateY(yawRad);

			double yawRad = std::atan2(dir.x, dir.z);
			yawRad += Math::ToRadians(90.0);
			Quaternion rot = Quaternion::RotateY(yawRad);

			// 選択中のモデルは少し大きく・ハイライト
			const bool isSelected = (i == index);

			if (isSelected)
			{
				// 少し浮かせて目立たせる
				const Vec3 hoverPos = pos + Vec3{ 0, 0.2 + 0.05 * std::sin(Scene::Time() * 4.0), 0 };
				if (curTime <= 3.3f)
					fbx.eggModels[i].draw(hoverPos, rot);
				else {
					auto model = fbx.dragonModels[i];
					// rot * fbx.bindRot
					model.draw(hoverPos + Vec3{ 0,0.5f,0 }, rot);
					model.setAnim(fbx.dragonAnims[0]);
					model.updateFbx(Scene::DeltaTime(), fbx.dragonAnims[0]);

					//spawnModels[i].draw(hoverPos, rot);
					//spawnModels[i].setAnimIdx(0);
					//spawnModels[i].updateFbx(Scene::DeltaTime());
				}
			}
			else
			{
				//fbx.eggModels[i].draw(pos, Quaternion::RotateX(Math::ToRadians(90.0)));
				fbx.eggModels[i].draw(pos, rot);
			}
		}

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
		const String status = U"EggType: "_s + eggInfos[index].name;
		const Vec2 statusPos = Vec2{ 1050, 20 };
		Rect{ Arg::leftCenter(1050,175),200, 400 }.draw(ColorF{ 0.1, 0.1, 0.1, 0.8 });
		titleFont(status).draw(Arg::leftCenter(statusPos), Palette::White);
		for (int i = 0; i < 6; i++) {
			titleFont(starNames[i]).draw(Arg::leftCenter(statusPos + Vec2{ 0,50 + i * 50 }), Palette::White);
			String star = U"";
			for (int j = 0; j < eggInfos[index].stars[i]; j++) {
				star += U"★";
			}
			titleFont(star).draw(Arg::leftCenter(statusPos + Vec2{ 65,50 + i * 50 }), Palette::White);
		}

		eggBtn.draw();
	}
}
