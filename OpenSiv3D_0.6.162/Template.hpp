#pragma once
# include "Common.hpp"

// タイトルシーン
class Template : public App::Scene
{
public:
	Template(const InitData& init);

	void update() override;

	void draw() const override;

private:

};
