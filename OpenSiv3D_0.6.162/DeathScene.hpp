#pragma once
# include "Common.hpp"

class DeathScene : public App::Scene
{
public:

	DeathScene(const InitData& init);

	void update() override;

	void draw() const override;

private:
	int rand = 0;
	int modelIdx = 0;
	double groundDeg = 0;
	const Vec2 center = Scene::CenterF();
	const ColorF backgroundColor = ColorF{ 0, 0, 0 }.removeSRGBCurve();

	const Mesh groundPlane{ MeshData::Disc(2.5, 128) };
	const Texture groundTexture{ U"example/texture/22029897_.png", TextureDesc::MippedSRGB };
	// 3D をレンダリングするターゲット（深度バッファ付き）
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

	// カメラ
	//BasicCamera3D camera{ Graphics3D::GetRenderTargetSize(), 40_deg, Vec3{ 0, 3, -16 } };
	BasicCamera3D camera{ Graphics3D::GetRenderTargetSize(), 20_deg, Vec3{ 0, 2, -8 } };
	FbxModelManager& fbx = FbxModelManager::Instance();
	AudioManager& audioManager = AudioManager::Instance();

	double currentAngleDeg = -70;
	bool deathDone = false;
};
