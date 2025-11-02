# include "Test.hpp"

Test::Test(const InitData& init)
	: IScene{ init }
{
	// モデルに付随するテクスチャをアセット管理に登録
	Model::RegisterDiffuseTextures(treeModel, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(pineModel, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(siv3dkunModel, TextureDesc::MippedSRGB);
	Model::RegisterDiffuseTextures(fbxModel, TextureDesc::MippedSRGB);
}

void Test::update()
{
	camera.update(4.0);
	Graphics3D::SetCameraTransform(camera);

	// [3D シーンの描画]
	{
		const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };

		// [モデルの描画]
		{
			// 地面の描画
			groundPlane.draw(groundTexture);

			// 球の描画
			Sphere{ { 0, 1, 0 }, 1 }.draw(ColorF{ 0.75 }.removeSRGBCurve());

			// 鍛冶屋の描画
			blacksmithModel.draw(Vec3{ 8, 0, 4 });

			// 風車の描画
			millModel.draw(Vec3{ -8, 0, 4 });

			// 木の描画
			{
				const ScopedRenderStates3D renderStates{ BlendState::OpaqueAlphaToCoverage, RasterizerState::SolidCullNone };
				treeModel.draw(Vec3{ 16, 0, 4 });
				pineModel.draw(Vec3{ 16, 0, 0 });
			}

			// Siv3D くんの描画
			siv3dkunModel.draw(Vec3{ 2, 0, -2 }, Quaternion::RotateY(180_deg));
			fbxModel.draw(Vec3{ 5, 0, -2 });
			curTime += Scene::DeltaTime();
			fbxModel.setAnimIdx(0);
			/*
			if (curTime > 1.0f) {
				animIdx++;
				if (!fbxModel.setAnimIdx(animIdx, false)) {
					animIdx = 0;
					fbxModel.setAnimIdx(animIdx, false);
				}
				curTime = 0;
			}
			*/
			fbxModel.updateFbx(Scene::DeltaTime());
		}
	}

	// [RenderTexture を 2D シーンに描画]
	{
		Graphics3D::Flush();
		renderTexture.resolve();
		Shader::LinearToScreen(renderTexture);
	}
}

void Test::draw() const
{
	Scene::SetBackground(backgroundColor);
}
