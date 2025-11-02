# pragma once
# include "Common.hpp"

// タイトルシーン
class Title : public App::Scene
{
public:

	Title(const InitData& init);

	void update() override;

	void draw() const override;

private:
	RoundRect m_eggButton{ Arg::center(300, 400), 300, 60, 8 };
	RoundRect m_tameButton{ Arg::center(300, 300), 300, 60, 8 };
	RoundRect m_evoButton{ Arg::center(300, 400), 300, 60, 8 };
	RoundRect m_deathButton{ Arg::center(300, 200), 300, 60, 8 };
	RoundRect m_battleButton{ Arg::center(300, 700), 300, 60, 8 };
	RoundRect m_exitButton{ Arg::center(300, 500), 300, 60, 8 };

	Transition m_eggTransition{ 0.4s, 0.2s };
	Transition m_tameTransition{ 0.4s, 0.2s };
	Transition m_evoTransition{ 0.4s, 0.2s };
	Transition m_deathTransition{ 0.4s, 0.2s };
	Transition m_battleTransition{ 0.4s, 0.2s };
	Transition m_exitTransition{ 0.4s, 0.2s };

	RoundRect m_randButton{ Arg::center(1100, 50), 60, 60, 8 };

	int rand = 0;

	const Vec2 center = Scene::CenterF();
	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();

	// 3D をレンダリングするターゲット（深度バッファ付き）
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

	// カメラ
	//BasicCamera3D camera{ Graphics3D::GetRenderTargetSize(), 40_deg, Vec3{ 0, 3, -16 } };
	BasicCamera3D camera{ Graphics3D::GetRenderTargetSize(), 20_deg, Vec3{ 0, 1, -16 } };
	FbxModelManager& fbx = FbxModelManager::Instance();
};
