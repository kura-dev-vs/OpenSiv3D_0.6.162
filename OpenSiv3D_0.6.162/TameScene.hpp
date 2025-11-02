#pragma once
# include "Common.hpp"

// タイトルシーン
class TameScene : public App::Scene
{
public:
	TameScene(const InitData& init);

	void update() override;

	void draw() const override;

	void training(int32 trainIdx);
	void communication();
	void sleep();

private:
	int modelIdx;

	// フォント
	const Font titleFont{ 24 };
	const Font labelFont{ 12 };

	BasicCamera3D camera{ Graphics3D::GetRenderTargetSize(), 40_deg, Vec3{ 0, 1, -13 } };
	// 3D をレンダリングするターゲット（深度バッファ付き）
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

	const Mesh groundPlane{ MeshData::OneSidedPlane(2000, { 400, 400 }) };
	const Texture groundTexture{ U"example/texture/ground.jpg", TextureDesc::MippedSRGB };
	const ColorF backgroundColor = ColorF{ 0.114, 0.184, 0.49 }.removeSRGBCurve();
	Model model;
	int animIdx = 0;

	const int32 IconSize1 = 55;
	const int32 IconSize2 = 40;
	const int32 IconSize3 = 60;
	const double FontSize1 = 20;
	const double FontSize2 = 15.5;
	const double FontSize3 = 24;

	const int32 BasePosY = 430;
	int32 basePosX = 150;
	const int32 Interval = 150;
	const int32 BtnSize = 120;

	TameActState curTameActState = TameActState::メイン;

	const int32 BtnWidth = 160;
	const int32 BtnHeight = 60;
	int32 x0 = 40;

	const int32 actBtnInterval = 250;
	Array<TameActBtn> tameActBtns = {
		{Emoji{ U"💪"_emoji }, FontSize1, U"訓練", 0}, // 訓練
		{Emoji{ U"⚽"_emoji }, FontSize1, U"交流",  actBtnInterval}, // 交流
		{Emoji{ U"💤"_emoji }, FontSize1, U"寝かせる",actBtnInterval * 2}, // 寝かせる

	};

	Array<TrainingButton> trainButtons = {
		{ 0xF034D_icon, IconSize1, FontSize1, U"持久飛", Rect{ basePosX, BasePosY, BtnSize }, 0 },
		{ 0xF018B_icon, IconSize1, FontSize1, U"筋力強化", Rect{ basePosX + Interval, BasePosY, BtnSize }, 1 },
		{ 0xF0E10_icon, IconSize1, FontSize1, U"魔力集中", Rect{ basePosX + Interval * 2, BasePosY, BtnSize }, 2 },
		{ 0xF05DA_icon, IconSize1, FontSize1, U"メンタル", Rect{ basePosX + Interval * 3, BasePosY, BtnSize }, 3 },
		{ 0xF0538_icon, IconSize1, FontSize1, U"滝行", Rect{ basePosX + Interval * 4, BasePosY, BtnSize },4  },
		{ 0xF0493_icon, IconSize1, FontSize1, U"瞑想", Rect{ basePosX + Interval * 5, BasePosY, BtnSize }, 5 },
		{ 0xF0493_icon, IconSize1, FontSize1, U"総合訓練", Rect{ basePosX + Interval * 6, BasePosY, BtnSize }, 6 },

	};

	const RectF barRect = RectF{ 160, 750, 300, 40 }.movedBy(40, -120);
	//UIBar uiBar{ getData().maxST };
	UIBar uiBar;
	FbxModelManager& fbx = FbxModelManager::Instance();
	AudioManager& audioManager = AudioManager::Instance();
};
