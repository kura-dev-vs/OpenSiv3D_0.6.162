#pragma once
# include "Common.hpp"

// タイトルシーン
class BattleScene : public App::Scene
{
public:
	BattleScene(const InitData& init);

	void update() override;

	void draw() const override;

private:
	void ExtendedTime(double effectValue);

	void CardOperation();
	void ShuffleDeckAndCompactLanes();
	void RecalculateLanePositions(Array<CardTransform>& cards);
	void RecalculateHandPositions(Array<CardTransform>& cards);
	int32 PlayerAttackCalc(CardInfo cI);
	int32 EnemyAttackCalc(EnemySkillInfo eSI);
	void PlayerAttackTurn();
	void PlayerAction(CardInfo ci);
	void EnemyAction();
	void ChangeToHP(UIBar& uiBar, int32 hp, int32& curHP, int32& maxHP);


	FbxModelManager& fbx = FbxModelManager::Instance();
	AudioManager& audioManager = AudioManager::Instance();
	UIBar playerHPBar, enemyHPBar;
	const RectF pBarRect = RectF{ 100, 20, 400, 40 };
	const RectF eBarRect = RectF{ 800, 20, 400, 40 };

	int32 playerModelIdx;
	int32 enemyModelIdx = 0;
	Model playerModel;
	Model enemyModel;
	int32 playerAnimIdx = 0;
	int32 enemyAnimIdx = 0;
	int32 playerMaxHP = 0;
	int32 playerCurHP = 0;
	int32 enemyMaxHP = 0;
	int32 enemyCurHP = 0;
	double enemyInterval = 5;
	double curInterval = 0;

	int32 currentPickingCardIdx = -1;
	//ダメージ係数
	const int32 Base = 30;
	Array<DamageText> damages;

	double laneSpeed = 150;
	Array<int32> handCards;
	Array<int32> handCardsTimer;
	Array<CardTransform> handCardMap;
	Array<int32> handOrder;
	Array<int32> allDeck;
	Array<int32> currentPlayerStatus;
	Array<int32> currentEnemyStatus;

	Array<CardTransform> deckCardMap = {
		{CardTransform(0)},
		{CardTransform(0)},
		{CardTransform(7)},
		{CardTransform(24)},
		{CardTransform(18)},
		{CardTransform(0)},
	};

	int32 maxCP = 0;
	int32 curCP = 0;
	int32 enemyActionInterval = 0;
	int32 enemyActionTimer = 0;

	bool playerActionFlag = false;
	bool enemyActionFlag = false;

	bool playerDeath = false;
	bool enemyDeath = false;

	bool playerWinFlag = false;

	bool calcuratePlayerTurn = false;

	Size windowSize = Scene::Size();
	int32 h = windowSize.y / 2.25;

	Font m_font{ FontMethod::MSDF, 48, U"example/font/RocknRoll/RocknRollOne-Regular.ttf" };
	const Font titleFont{ 40 , Typeface::Heavy };
	const Font labelFont{ 24 };
	Vec2 cpPos = Vec2(1150, 425);
	const Rect handRect{ Arg::center(windowSize.x / 2 , windowSize.y - 300), windowSize.x, h - 50 };
	const Rect laneRect{ Arg::center(windowSize.x / 2 , windowSize.y), windowSize.x, h };

	BasicCamera3D camera{ Graphics3D::GetRenderTargetSize(), 40_deg, Vec3{ 0, 1, -14 } };
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };
	const Mesh groundPlane{ MeshData::OneSidedPlane(2000, { 400, 400 }) };
	const Texture groundTexture{ U"example/texture/ground.jpg", TextureDesc::MippedSRGB };
	const ColorF backgroundColor = ColorF{ 0.114, 0.184, 0.49 }.removeSRGBCurve();

	TameActBtn attackBtn = { Emoji{ U"⚔️"_emoji }, 20, U"攻撃", 750 };
	const Font statsFont{ 12 };
	const Vec2 pStatsPos = Vec2{ 50, 130 };
	const Vec2 eStatsPos = pStatsPos + Vec2{ 1175,0 };
	CardTemplate templ;
	Vec2 cursor;
};
