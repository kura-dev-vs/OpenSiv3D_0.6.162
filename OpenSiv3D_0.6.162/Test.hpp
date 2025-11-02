# pragma once
# include "Common.hpp"

// ゲームシーン
class Test : public App::Scene
{
public:

	Test(const InitData& init);

	void update() override;

	void draw() const override;


	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();

	const Mesh groundPlane{ MeshData::OneSidedPlane(2000, { 400, 400 }) };
	const Texture groundTexture{ U"example/texture/ground.jpg", TextureDesc::MippedSRGB };

	// モデルデータをロード
	const Model blacksmithModel{ U"example/obj/blacksmith.obj" };
	const Model millModel{ U"example/obj/mill.obj" };
	const Model treeModel{ U"example/obj/tree.obj" };
	const Model pineModel{ U"example/obj/pine.obj" };
	const Model siv3dkunModel{ U"example/obj/siv3d-kun.obj" };
	//const Model fbxModel{ U"example/fbx/the Holy Blade.fbx", ColorOption::Default, 0.01f };
	const Model fbxModel{ U"example/fbx/Standing Idle.fbx", ColorOption::Default, 0.01f };
	//const Model fbxModel{ U"example/fbx/Mutant.fbx", ColorOption::Default, 0.01f };



	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };
	DebugCamera3D camera{ Graphics3D::GetRenderTargetSize(), 40_deg, Vec3{ 0, 3, -16 } };

	float curTime = 0;
	int animIdx = 0;
private:



};
