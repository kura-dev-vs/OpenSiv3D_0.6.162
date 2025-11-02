#pragma once
# include <Siv3D.hpp>

class TameActBtn
{
public:

	struct Palette
	{
		ColorF background = ColorF{ 0.96 };    // ボタン背景（レンダーテクスチャのベース）
		ColorF pattern = ColorF{ 0.85 };       // パターン色（テクスチャ作成時）
		ColorF primary = ColorF{ 0.3, 0.5, 1.0 }; // メインカラー（アイコン色・枠色）
		ColorF emojiIdle = ColorF{ 0.3, 0.25, 0.2 }; // 絵文字の非ホバー色
	};

	TameActBtn() = default;

	TameActBtn(const Emoji& emoji,
		double fontSize,
		const String& text,
		const int32 interval,
		const int32 basePosY = 475,
		int32 cornerRadius = 10)
		: m_emoji{ emoji }
		, m_bufferedEmoji{ MakeRoundBuffer(CreateEmojiPolygons(emoji), 5).scaled(EmojiScale) }
		, m_fontSize{ fontSize }
		, m_text{ text }
		, m_cornerRadius{ cornerRadius }
		, m_buttonTexture{} // rect サイズに合わせたテクスチャ作成
	{
		m_rect = Rect{ basePosX + interval, basePosY, BtnWidth, BtnHeight };
		m_buttonTexture = CreateButtonTexture(Size{ m_rect.w, m_rect.h }, m_palette);
	}

	// update: 押下処理とマウス操作の状態遷移を管理。押下確定で true を返す。
	bool update()
	{
		const RoundRect roundRect{ m_rect, static_cast<RoundRect::value_type>(m_cornerRadius) };
		const bool mouseOver = roundRect.mouseOver();

		// カーソル変更
		if (mouseOver)
		{
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		// マウス押下 / リリースの管理（TrainingButton と同様の挙動）
		bool pushed = false;

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
				// ドラッグで外れたら押下解除
				m_pressed = false;
			}
		}

		// 押下トランジション更新（縮小/拡大のアニメーション用）
		m_transitionPressed.update(m_pressed);

		// ホバー用トランジション（絵文字の振動/回転に利用）
		m_hoverTransition.update((not roundRect.intersects(Cursor::PreviousPos())) && mouseOver);

		return pushed;
	}

	// draw: const メソッドにして描画だけ行う（TrainingButton と同様の役割分離）
	void draw() const
	{
		const RoundRect roundRect{ m_rect, static_cast<RoundRect::value_type>(m_cornerRadius) };
		const bool mouseOver = roundRect.mouseOver();

		// 押下トランジション値（0..1）
		const double tp = m_transitionPressed.value();

		// 軽いスケール変化を付ける
		const Transformer2D transform{ Mat3x2::Scale((1.0 + tp * -0.04), m_rect.center()) };

		// 背景（テクスチャ） + フレーム
		if (mouseOver)
		{
			// ホバー時は白っぽい乗算で明るく見せ、太めのフレームを描画
			roundRect(m_buttonTexture).draw(MouseL.pressed() ? ColorF{ 0.95 } : ColorF{ 1.05 }).drawFrame(0, 3, m_palette.primary);
		}
		else
		{
			roundRect(m_buttonTexture).draw().drawFrame(2);
		}

		// 絵文字の回転アニメーション
		{
			double hv = m_hoverTransition.value();
			double angle = Math::Sin(hv * 8_pi) * 5_deg * hv;
			const Vec2 emojiCenter = m_rect.getRelativePoint(0.5, 0.15);

			const Transformer2D transformer{ Mat3x2::Rotate(angle, emojiCenter) };

			// バッファポリゴン（太めのシルエット）を描画
			m_bufferedEmoji.draw(emojiCenter, mouseOver ? m_palette.primary : m_palette.emojiIdle);

			// 実際の Emoji テクスチャを重ねる
			m_emoji.scaled(EmojiScale).rotated(angle).drawAt(emojiCenter);
		}

		// ラベル描画（アウトライン付き）
		{
			m_font(m_text).drawAt(TextStyle::Outline(0.0, 0.2, ColorF{ 1.0 }), static_cast<int32>(m_fontSize), m_rect.getRelativePoint(0.5, 0.75), m_palette.primary);
		}
	}

	bool getMouseOver() {
		RoundRect roundRect{ m_rect, static_cast<RoundRect::value_type>(m_cornerRadius) };
		return roundRect.mouseOver();
	}
	Rect m_rect;
	String m_text;

private:
	Texture m_emoji;
	MultiPolygon m_bufferedEmoji;

	Transition m_hoverTransition{ 0.0s, 0.8s };           // ホバーでの絵文字振動用
	Transition m_transitionPressed{ 0.09s, 0.12s };       // 押下アニメーション

	static constexpr double EmojiScale = 0.5;

	Font m_font{ FontMethod::MSDF, 48, U"example/font/RocknRoll/RocknRollOne-Regular.ttf" };
	double m_fontSize = 26.0;

	Palette m_palette = Palette();
	int32 m_cornerRadius = 10;

	bool m_pressed = false;
	const int32 basePosX = 300;
	const int32 BasePosY = 475;
	const int32 BtnWidth = 200;
	const int32 BtnHeight = 80;

	// ボタン背景テクスチャ（Rect サイズに合わせて作成）
	Texture m_buttonTexture;

	// ---- static utility functions ----

	static MultiPolygon CreateEmojiPolygons(const Emoji& emoji)
	{
		// 画像をポリゴン化（元コードの160は分解能の目安）
		return Image{ emoji }.alphaToPolygonsCentered(160, AllowHoles::No);
	}

	static MultiPolygon MakeRoundBuffer(const MultiPolygon& polygons, int32 distance)
	{
		MultiPolygon result;

		for (const auto& polygon : polygons)
		{
			result = Geometry2D::Or(result, polygon.calculateRoundBuffer(distance));
		}

		return result;
	}

	// rectSize に合わせたボタン背景テクスチャを作る（元の CreateButtonTexture を一般化）
	static Texture CreateButtonTexture(const Size& rectSize, const Palette& palette)
	{
		// サイズが小さい場合は最低サイズを確保
		const Size size = Size{ std::max(32, rectSize.x), std::max(16, rectSize.y) };

		MSRenderTexture renderTexture{ size, palette.background };
		{
			const ScopedRenderTarget2D renderTarget{ renderTexture };

			const ColorF PatternColor = palette.pattern;

			// 元コードのパターン生成ロジックを踏襲（幅に合わせてタイル数を計算）
			const int32 step = 20;
			const int32 cols = (size.x / step) + 2;

			for (int32 x = 0; x <= cols; ++x)
			{
				RectF{ Arg::center((x * step), 25), 2 }.rotated(45_deg).draw(PatternColor);
				RectF{ Arg::center((10 + x * step), 30), 3 }.rotated(45_deg).draw(PatternColor);
				RectF{ Arg::center((x * step), 35), 4 }.rotated(45_deg).draw(PatternColor);
				RectF{ Arg::center((10 + x * step), 40), 5 }.rotated(45_deg).draw(PatternColor);
				RectF{ Arg::center((x * step), 45), 6 }.rotated(45_deg).draw(PatternColor);
				RectF{ Arg::center((10 + x * step), 50), 7 }.rotated(45_deg).draw(PatternColor);
				RectF{ Arg::center((x * step), 55), 8 }.rotated(45_deg).draw(PatternColor);
				RectF{ Arg::center((10 + x * step), 60), 9 }.rotated(45_deg).draw(PatternColor);
			}
		}

		// MSAA 解決とフラッシュ
		Graphics2D::Flush();
		renderTexture.resolve();

		return renderTexture;
	}
};
