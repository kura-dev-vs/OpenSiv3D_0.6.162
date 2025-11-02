#pragma once
# include <Siv3D.hpp>

class TrainingButton
{
public:

	struct Palette
	{
		ColorF tileColor1 = ColorF{ 0.3, 0.2, 0.0 };
		ColorF tileColor2 = ColorF{ 1.0, 0.95, 0.75 };
		ColorF borderColor1 = ColorF{ 1.0, 0.4 };
		ColorF borderColor2 = ColorF{ 1.0, 0.8, 0.4 };
	};

	TrainingButton() = default;

	TrainingButton(const Icon& icon, int32 iconSize, double fontSize, const String& text, const RectF& rect, int32 trainIdx)
		: TrainingButton{ Texture{ icon, iconSize }, iconSize, fontSize, text, rect, trainIdx } {
	}

	// Texture からアイコンを作成
	TrainingButton(const TextureRegion& textureRegion, int32 iconSize, double fontSize, const String& text, const RectF& rect, int32 trainIdx)
		: m_icon{ textureRegion }
		, m_iconSize{ iconSize }
		, m_fontSize{ fontSize }
		, m_text{ text }
		, m_rect{ rect }
		, m_trainIdx{ trainIdx } {
	}

	bool update()
	{
		const bool mouseOver = m_rect.mouseOver();

		bool pushed = false;

		if (mouseOver)
		{
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		if (not m_pressed)
		{
			if (m_rect.leftClicked())
			{
				m_pressed = true;
			}
		}
		else
		{
			if (m_rect.leftReleased())
			{
				m_pressed = false;
				pushed = true;
			}
			else if (not m_rect.mouseOver())
			{
				m_pressed = false;
			}
		}

		m_transitionPressed.update(m_pressed);

		return pushed;
	}

	void draw() const
	{
		const double t = m_transitionPressed.value();

		const Transformer2D transform{ Mat3x2::Scale((1 + t * 0.06), m_rect.center()) };

		// タイル
		{
			m_rect.draw(m_palette.tileColor1.lerp(m_palette.tileColor2, t));

			m_rect.stretched(Math::Lerp(-InnerBorderMargin, 0, t))
				.drawFrame(0.1, (1.0 + t * 2.0), m_palette.borderColor1.lerp(m_palette.borderColor2, t));
		}

		// アイコン
		{
			m_icon
				.drawAt(m_rect.getRelativePoint(0.5, 0.4), m_palette.tileColor2.lerp(m_palette.tileColor1, t));
		}

		// ラベル
		{
			m_font(m_text)
				.drawAt(m_fontSize, m_rect.getRelativePoint(0.5, 0.8), m_palette.tileColor2.lerp(m_palette.tileColor1, t));
		}
	}


	int32 m_trainIdx = 0;

private:

	static constexpr double InnerBorderMargin = 3.0;

	TextureRegion m_icon;

	int32 m_iconSize = 0;

	Font m_font{ FontMethod::MSDF, 48, U"example/font/RocknRoll/RocknRollOne-Regular.ttf" };

	double m_fontSize = 0;

	String m_text;

	RectF m_rect;

	Transition m_transitionPressed{ 0.09s, 0.12s };

	Palette m_palette = Palette();

	bool m_pressed = false;


};
