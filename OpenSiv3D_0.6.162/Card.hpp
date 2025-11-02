#pragma once
# include <Siv3D.hpp>

enum class CardType
{
	Attack,
	Buf,
	Debuf,
	Heal,
	ExtendTime,
};

struct CardInfo {
	int32 id;
	const CardType type;
	const String name;
	const int32 cp;
	const int32 statusIdx;
	const double timeLimit;
	const double effectValue;
	const bool dragonCard = false;
};

inline Array<CardInfo> normalCardInfos = {
	{ 0, CardType::Attack, U"筋力攻撃",2, 2, 10.0, 20.0, false},
	{ 1, CardType::Attack, U"筋力攻撃",3, 2, 10.0, 30.0, false },
	{ 2, CardType::Attack, U"筋力攻撃",5, 2, 10.0, 50.0, false },
	{ 3, CardType::Attack, U"特攻攻撃",2, 4, 10.0, 20.0, false },
	{ 4, CardType::Attack, U"特攻攻撃",3, 4, 10.0, 30.0, false },
	{ 5, CardType::Attack, U"特攻攻撃",5, 4, 10.0, 50.0, false },


	{ 6, CardType::Buf, U"筋力強化",2, 2, 10.0, 1.1, false },
	{ 7, CardType::Buf, U"筋力強化",3, 2, 10.0, 1.2, false },
	{ 8, CardType::Buf, U"防御強化",2, 3, 10.0, 1.1, false },
	{ 9, CardType::Buf, U"防御強化",3, 3, 10.0, 1.2, false },
	{ 10, CardType::Buf, U"特攻強化",2, 4, 10.0, 1.1, false },
	{ 11, CardType::Buf, U"特攻強化",3, 4, 10.0, 1.2, false },
	{ 12, CardType::Buf, U"特防強化",2, 5, 10.0, 1.1, false },
	{ 13, CardType::Buf, U"特防強化",3, 5, 10.0, 1.2, false },

	{ 14, CardType::Debuf, U"筋力弱体",2, 2, 10.0, 0.9, false },
	{ 15, CardType::Debuf, U"筋力弱体",3, 2, 10.0, 0.8, false },
	{ 16, CardType::Debuf, U"防御弱体",2, 3, 10.0, 0.9, false },
	{ 17, CardType::Debuf, U"防御弱体",3, 3, 10.0, 0.8, false },
	{ 18, CardType::Debuf, U"特攻弱体",2, 4, 10.0, 0.9, false },
	{ 19, CardType::Debuf, U"特攻弱体",3, 4, 10.0, 0.8, false },
	{ 20, CardType::Debuf, U"特防弱体",2, 5, 10.0, 0.9, false },
	{ 21, CardType::Debuf, U"特防弱体",3, 5, 10.0, 0.8, false },

	{ 22, CardType::Heal, U"体力回復",5, 0, 10.0, 20.0, false },
	{ 23, CardType::Heal, U"体力回復",7, 0, 10.0, 40.0, false },

	{ 24, CardType::ExtendTime, U"時間延長", 0, 0, 10.0, 2.5, false },
	{ 25, CardType::ExtendTime, U"時間延長", 0, 0, 10.0, 5.0, false },
};

inline Array<CardInfo> dragonCardInfos = {
	{ 0, CardType::Attack, U"", 10, 2, 5.0, 10.0, true },
};

struct CardTemplate {
	const Font font1{ 15, Typeface::Heavy }, valueFont{ 20, Typeface::Black }, costFont{ 20, Typeface::Black };
	const RoundRect r0{ 0, 0, 94.5, 132, 6 }; // card background
	const RoundRect r1{ 10, 5, 74.5, 60, 5 }; // card image area

	// カードの描画領域（原点が (0,0) のテンプレート座標系における矩形）
	RectF FullRect() const {
		return r0.rect; // r0 の矩形（幅高さ）を返す
	}

	const Array<Texture> emojis = {
		Texture{ U"👊"_emoji }, Texture{ U"↗️"_emoji }, Texture{ U"↘️"_emoji },
		Texture{ U"💖"_emoji }, Texture{ U"⌛"_emoji }, Texture{ U"🐲"_emoji } };
};

struct CardTransform {
	int32 instanceID;
	int32 cardID;
	int32 handIdx = -1;
	Vec2 cardPos = Vec2::Zero();
	Vec2 initLanePos = Vec2(0, 575);
	Vec2 laneCardPos = Vec2(0, 575);
	Vec2 initHandPos = Vec2(50, 375);
	Vec2 handCardPos = Vec2(0, 375);
	double restTime = 0;
	bool dragging = false;
	bool inHand = false;
	bool isUsed = false;
	Vec2 dragOffset = Vec2::Zero();

	void InitLaneTransform(int idx) {
		laneCardPos = initLanePos.movedBy(idx * -150, 0);
		restTime = normalCardInfos[cardID].timeLimit;
	}

	void HandTransform() {
		handCardPos = initHandPos.movedBy(handIdx * 100, 0);
	}

	CardTransform(int32 ID) {
		cardID = ID;
		restTime = normalCardInfos[cardID].timeLimit;
		static int nextID = 0;
		instanceID = nextID++;
	}
	void SetCardPos(Vec2 pos) {
		cardPos = pos;
	}
};

inline void CardDraw(const CardTransform& c, const CardTemplate& templ) {
	int32 cardID = c.cardID;
	Vec2 pos = c.cardPos;
	const auto& card = normalCardInfos[cardID];

	const Transformer2D t{ Mat3x2::Translate(pos) };

	templ.r0.drawShadow(Vec2{ 4, 4 }, 8, 1)
		.draw(ColorF{ 0.2, 0.25, 0.3 })
		.drawFrame(1, 1, ColorF{ 0.4, 0.5, 0.6 });

	templ.r1.draw(ColorF{ 0.85, 0.9, 0.95 });


	if (card.dragonCard) {
		templ.emojis[5].resized(70).drawAt(templ.r1.center());
	}
	else {
		switch (card.type) {
		case CardType::Attack:
			templ.emojis[0].resized(70).drawAt(templ.r1.center());
			break;
		case CardType::Buf:
			templ.emojis[1].resized(70).drawAt(templ.r1.center());
			break;
		case CardType::Debuf:
			templ.emojis[2].resized(70).drawAt(templ.r1.center());
			break;
		case CardType::Heal:
			templ.emojis[3].resized(70).drawAt(templ.r1.center());
			break;
		case CardType::ExtendTime:
			templ.emojis[4].resized(70).drawAt(templ.r1.center());
			break;
		default:
			break;
		}
	}
	RoundRect{ Arg::center(templ.r1.rect.pos.movedBy(15, 110)), 40, 30, 10 }.draw(ColorF{ 0.2 });
	templ.costFont(ThousandsSeparate(c.restTime, 1)).draw(Arg::center(templ.r1.rect.pos.movedBy(15, 110)), Palette::Darkorange);

	Circle{ Arg::center(templ.r1.rect.pos.movedBy(5, 10)), 15 }.draw(Palette::White);
	templ.costFont(card.cp).draw(Arg::center(templ.r1.rect.pos.movedBy(5, 10)), ColorF{ 0.2 });

	templ.font1(card.name).draw(Arg::center(templ.r1.rect.pos.movedBy(35, 80)));
	templ.valueFont(card.effectValue).draw(Arg::center(templ.r1.rect.pos.movedBy(60, 110)));
}
