# include <Siv3D.hpp>
#include "Siv3D/AddScripts/AssimpHelper.h"
#include "Siv3D/AddScripts/FBXModelHelper.hpp"
#include "Siv3D/AddScripts/FBXModelComponent.hpp"

# include "Common.hpp"
# include "Title.hpp"
# include "TameScene.hpp"
# include "Test.hpp"
# include "EvoScene.hpp"
# include "DeathScene.hpp"
# include "BattleScene.hpp"
# include "Egg.hpp"


#include "FbxModelManager.hpp"
//# include "Ranking.hpp"


void Main()
{
	Window::Resize(1280, 720);
	FontAsset::Register(U"TitleFont", FontMethod::MSDF, 80, U"example/font/RocknRoll/RocknRollOne-Regular.ttf");
	FontAsset(U"TitleFont").setBufferThickness(4);

	FontAsset::Register(U"Bold", FontMethod::MSDF, 48, Typeface::Bold);

	App manager;
	manager.add<Title>(State::Title);
	manager.add<TameScene>(State::TameScene);
	manager.add<Test>(State::Test);
	manager.add<Egg>(State::Egg);
	manager.add<EvoScene>(State::EvoScene);
	manager.add<DeathScene>(State::DeathScene);
	manager.add<BattleScene>(State::BattleScene);
	//manager.add<Ranking>(State::Ranking);

	auto& fbxManager = FbxModelManager::Instance();
	fbxManager.RegisterTextures();

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}

