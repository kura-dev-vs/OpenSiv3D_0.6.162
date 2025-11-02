#pragma once
# include <Siv3D.hpp> // OpenSiv3D v0.6+ を想定

struct Particle
{
	Vec2 pos;
	Vec2 vel;
	double life = 0.0;
	double maxLife = 0.6;

	Particle(const Vec2& p, const Vec2& v)
		: pos(p), vel(v) {
	}

	bool update(double dt)
	{
		life += dt;
		pos += vel * dt;
		vel.y += 600.0 * dt; // 重力っぽく
		return life < maxLife;
	}

	void draw() const
	{
		const double t = life / maxLife;
		const double a = 1.0 - t;
		Circle(pos, 2.0 + 2.0 * (1.0 - t)).draw(ColorF(1.0, 0.9, 0.6, a));
	}
};

class DamageText
{
private:
	Vec2 m_pos;
	int32_t m_damage;
	double m_time = 0.0;
	double m_duration = 1.0; // 表示時間（秒）
	double m_floatSpeed = 40.0; // 上に移動する速度（px/sec）
	double m_baseScale = 1.0;
	double m_rotation = 0.0;
	bool m_critical = false;

	Array<Particle> m_particles;

public:
	// コンストラクタ : int32_t を受け取る
	DamageText(const Vec2& pos, int32_t damage, bool critical = false)
		: m_pos(pos), m_damage(damage), m_critical(critical)
	{
		// クリティカルなら演出を強める
		if (m_critical)
		{
			m_duration = 1.2;
			m_floatSpeed = 80.0;
			m_baseScale = 1.5;
			// パーティクルをいくつか生成
			const int n = 12;
			for (int i = 0; i < n; ++i)
			{
				const double ang = Random(0.0, Math::TwoPi);
				const double speed = Random(120.0, 320.0);
				const Vec2 v = Vec2(std::cos(ang), std::sin(ang)) * speed;
				m_particles.emplace_back(m_pos, v);
			}
		}
	}

	bool alive() const
	{
		return m_time < m_duration;
	}

	void update(double dt)
	{
		m_time += dt;
		const double t = Saturate(m_time / m_duration);

		// 浮き上がり（イージングつき）
		// ease out: 1 - (1 - t)^2
		const double easeOut = 1.0 - (1.0 - t) * (1.0 - t);
		m_pos.y -= m_floatSpeed * dt * (1.0 + 0.5 * (1.0 - t));

		// スケール（最初にポップして縮む）
		m_baseScale = m_critical ? (1.5 - 0.6 * t) : (1.0 + 0.6 * (1.0 - t));

		// 回転（微妙な揺らぎ）
		m_rotation = (0.6 - t * 0.6) * (std::sin(m_time * 10.0) * 0.15);

		// パーティクル更新（クリティカルのみが多く持つ）
		for (auto it = m_particles.begin(); it != m_particles.end();)
		{
			if (!it->update(dt))
			{
				it = m_particles.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void draw(const Font& font) const
	{
		const double t = Saturate(m_time / m_duration);
		const double alpha = 1.0 - t; // フェードアウト
		const String str = Format(U"{}"_fmt(m_damage));

		// 色：クリティカルは赤系、通常は白系
		ColorF col = m_critical ? ColorF(1.0, 0.4, 0.2, alpha) : ColorF(1.0, 1.0, 1.0, alpha);
		ColorF glow = m_critical ? ColorF(1.0, 0.6, 0.3, 0.25 * alpha) : ColorF(0.6, 0.8, 1.0, 0.18 * alpha);

		// 背景のぼかし / グロー（楕円で表現）
		const double glowSize = 40.0 * (1.0 + (1.0 - t));
		Ellipse(m_pos.x, m_pos.y, glowSize, glowSize * 0.6).draw(glow);

		// アウトライン（簡易）：テキストを少しずらして濃い色で重ねる
		const Vec2 offs[8] = {
			{-1,0},{1,0},{0,-1},{0,1},
			{-1,-1},{1,-1},{-1,1},{1,1}
		};
		for (const auto& o : offs)
		{
			//font(str).drawAt(m_pos + o * 1.2, ColorF(0.0, 0.0, 0.0, 0.7 * alpha), TextStyle::DrawShadow);
			font(str).drawAt(m_pos + o * 1.2, ColorF(0.0, 0.0, 0.0, 0.7 * alpha));
		}

		// 本体（スケールと回転を適用）
		// Siv3D の Font は transform を直接受けないので、ScopedRenderStates2D で回転/拡縮を行う
		ScopedRenderStates2D sr;
		Transformer2D t2(Mat3x2::Translate(m_pos) * Mat3x2::Rotate(m_rotation) * Mat3x2::Scale(m_baseScale) * Mat3x2::Translate(-m_pos));
		//font(str).drawAt(m_pos, col, TextStyle::DrawShadow);
		font(str).drawAt(m_pos, col);
		// パーティクル描画
		for (const auto& p : m_particles)
		{
			p.draw();
		}
	}
};
