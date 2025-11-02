#pragma once
# include "Common.hpp"

// タイトルシーン
class Egg : public App::Scene
{
public:
	Egg(const InitData& init);

	void update() override;

	void draw() const override;

private:
	TameActBtn eggBtn = { Emoji{ U"🥚"_emoji }, 30, U"決定", 650, 600 };

	// 円の半径（モデルを配置する距離）
	const double modelRadius = 4.0;
	// カメラの距離（円を外側から見る位置）
	const double cameraRadius = 7.0;
	const double cameraHeight = 2.0;

	Quaternion bindRot = Quaternion::RotateX(Math::ToRadians(90.0));

	// モデル数とアルファベットラベル
	const int N = 5;
	const Array<String> labels = { U"A", U"B", U"C", U"D", U"E" };

	// 現在選択インデックス（0 = A）
	int index = 0;
	bool isAnim = false;
	float curTime = 0.0;

	// 角度 (degree) : targetAngle = index * 360/N
	double currentAngleDeg = 0.0; // 現在のカメラ角（度）
	double targetAngleDeg = 0.0;  // 目標角（度）
	const double rotateSpeed = 8.0; // イージング係数（大きいほど速く追従）

	// フォント
	const Font titleFont{ 24 };
	const Font labelFont{ 12 };

	// 簡易地面
	const Mesh groundPlane{ MeshData::Disc(5.0,128) };
	const Texture groundTexture{ U"example/texture/22352932_.png", TextureDesc::MippedSRGB };

	const double step = 360.0 / N; // 1ステップ分の角度（度）

	// 3D をレンダリングするターゲット（深度バッファ付き）
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

	BasicCamera3D camera{ Graphics3D::GetRenderTargetSize(), 40_deg, Vec3{ 0, cameraHeight, cameraRadius} };
	//const ColorF backgroundColor = ColorF{ 0.114, 0.184, 0.49 }.removeSRGBCurve();
	const ColorF backgroundColor = ColorF{ 0, 0.01, 0.2 }.removeSRGBCurve();
	double groundDeg = 0;
	FbxModelManager& fbx = FbxModelManager::Instance();
	AudioManager& audioManager = AudioManager::Instance();
};
