#pragma once
#include <Siv3D.hpp>
#include "Siv3D/AddScripts/AssimpHelper.h"

class FbxModelManager
{
public:
	static FbxModelManager& Instance()
	{
		static FbxModelManager instance;
		return instance;
	}

	FbxModelManager(const FbxModelManager&) = delete;
	FbxModelManager& operator=(const FbxModelManager&) = delete;

	const Array<Model> eggModels = {
		Model{ U"example/fbx/Egg/T_Egg_Toon_Red.fbx", ColorOption::Default, 0.01f  },
		Model{ U"example/fbx/Egg/T_Egg_Toon_BlueMarine.fbx", ColorOption::Default, 0.01f  },
		Model{ U"example/fbx/Egg/T_Egg_Toon_Green.fbx", ColorOption::Default, 0.01f  },
		Model{ U"example/fbx/Egg/T_Egg_Toon_Orange.fbx", ColorOption::Default, 0.01f  },
		Model{ U"example/fbx/Egg/T_Egg_Toon_Grey.fbx", ColorOption::Default, 0.01f  },
	};

	const Array<aiAnimation*> aiEggAnims = {
		eggModels[0].getAnim(0)
	};

	const Array<Model> dragonModels = {
		Model{ U"example/fbx/Dragon/Simple/Simple_Orange.fbx", ColorOption::Default, 0.007f  },
		Model{ U"example/fbx/Dragon/Simple/Simple_Blue.fbx", ColorOption::Default, 0.007f  },
		Model{ U"example/fbx/Dragon/Simple/Simple_Green.fbx", ColorOption::Default, 0.007f  },
		Model{ U"example/fbx/Dragon/Simple/Simple_Yellow.fbx", ColorOption::Default, 0.007f  },
		Model{ U"example/fbx/Dragon/Simple/Simple_Gray.fbx", ColorOption::Default, 0.007f  },

		Model{ U"example/fbx/Dragon/PolyArt/PolyArt_Orange.fbx", ColorOption::Default, 0.008f  },
		Model{ U"example/fbx/Dragon/PolyArt/PolyArt_Blue.fbx", ColorOption::Default, 0.008f  },
		Model{ U"example/fbx/Dragon/PolyArt/PolyArt_Green.fbx", ColorOption::Default, 0.008f  },
		Model{ U"example/fbx/Dragon/PolyArt/PolyArt_Yellow.fbx", ColorOption::Default, 0.008f  },
		Model{ U"example/fbx/Dragon/PolyArt/PolyArt_Gray.fbx", ColorOption::Default, 0.008f  },

		Model{ U"example/fbx/Dragon/Toon/Toon_Orange.fbx", ColorOption::Default, 0.01f  },
		Model{ U"example/fbx/Dragon/Toon/Toon_Blue.fbx", ColorOption::Default, 0.01f  },
		Model{ U"example/fbx/Dragon/Toon/Toon_Green.fbx", ColorOption::Default, 0.01f  },
		Model{ U"example/fbx/Dragon/Toon/Toon_Yellow.fbx", ColorOption::Default, 0.01f  },
		Model{ U"example/fbx/Dragon/Toon/Toon_Gray.fbx", ColorOption::Default, 0.01f  },


		Model{ U"example/fbx/Dragon/Realistic/Realistic_Orange.fbx", ColorOption::Default, 0.013f  },
		Model{ U"example/fbx/Dragon/Realistic/Realistic_Blue.fbx", ColorOption::Default, 0.013f  },
		Model{ U"example/fbx/Dragon/Realistic/Realistic_Green.fbx", ColorOption::Default, 0.013f  },
		Model{ U"example/fbx/Dragon/Realistic/Realistic_Yellow.fbx", ColorOption::Default, 0.013f  },
		Model{ U"example/fbx/Dragon/Realistic/Realistic_Brown.fbx", ColorOption::Default, 0.013f  },
		/*
		* ボーンが異なる可能性があるため、アニメーションがうまく適用されない
		*
		Model{ U"example/fbx/Dragon/RBaby2/RBaby2_Orange.fbx", ColorOption::Default, 0.01f  },
		Model{ U"example/fbx/Dragon/RBaby2/RBaby2_Blue.fbx", ColorOption::Default, 0.01f  },
		Model{ U"example/fbx/Dragon/RBaby2/RBaby2_Green.fbx", ColorOption::Default, 0.01f  },
		Model{ U"example/fbx/Dragon/RBaby2/RBaby2_Yellow.fbx", ColorOption::Default, 0.01f  },
		Model{ U"example/fbx/Dragon/RBaby2/RBaby2_Brown.fbx", ColorOption::Default, 0.01f  },
		*/
	};

	const Array<Model> dragonAnimModels = {
		Model{ U"example/fbx/Dragon/DragonAnims/LD_C_Fly_Idle.fbx", ColorOption::Default, 0.01f,true  },
		Model{ U"example/fbx/Dragon/DragonAnims/LD_C_Sleep_Enter.fbx", ColorOption::Default, 0.01f,true  },
		Model{ U"example/fbx/Dragon/DragonAnims/LD_C_Happy.fbx", ColorOption::Default, 0.01f,true  },
		Model{ U"example/fbx/Dragon/DragonAnims/LD_C_Level_Up.fbx", ColorOption::Default, 0.01f,true  },
		Model{ U"example/fbx/Dragon/DragonAnims/LD_C_Death1.fbx", ColorOption::Default, 0.01f,true  },
		Model{ U"example/fbx/Dragon/DragonAnims/LD_C_Fly_Attack_Fireball.fbx", ColorOption::Default, 0.01f,true  },
	};

	const Array<aiAnimation*> dragonAnims = {
		dragonAnimModels[0].getAnim(0),
		dragonAnimModels[1].getAnim(0),
		dragonAnimModels[2].getAnim(0),
		dragonAnimModels[3].getAnim(0),
		dragonAnimModels[4].getAnim(0),
		dragonAnimModels[5].getAnim(0),
	};

	const Array<Model> enemyModels = {
		Model{ U"example/fbx/Enemy/Mutant Idle.fbx", ColorOption::Default, 0.012f  },
		Model{ U"example/fbx/Enemy/Mutant Idle.fbx", ColorOption::Default, 0.012f  },
		Model{ U"example/fbx/Enemy/Mutant Idle.fbx", ColorOption::Default, 0.012f  },
	};

	const Array<Model> enemyAnimModels = {
		Model{ U"example/fbx/Enemy/Mutant Flexing Muscles.fbx", ColorOption::Default, 0.01f,true  },
		Model{ U"example/fbx/Enemy/Mutant Punch.fbx", ColorOption::Default, 0.01f,true  },
		Model{ U"example/fbx/Enemy/Mutant Swiping.fbx", ColorOption::Default, 0.01f,true  },
		Model{ U"example/fbx/Enemy/Mutant Roaring.fbx", ColorOption::Default, 0.01f,true  },
		Model{ U"example/fbx/Enemy/Mutant Dying.fbx", ColorOption::Default, 0.01f,true  },

	};

	const Array<aiAnimation*> enemyAnims = {
		enemyAnimModels[0].getAnim(0),
		enemyAnimModels[1].getAnim(0),
		enemyAnimModels[2].getAnim(0),
		enemyAnimModels[3].getAnim(0),
		enemyAnimModels[4].getAnim(0),
	};
	void RegisterTextures() {
		for (int i = 0; i < eggModels.size(); ++i)
		{
			Model::RegisterDiffuseTextures(eggModels[i], TextureDesc::MippedSRGB);
		}

		for (int i = 0; i < dragonModels.size(); ++i)
		{
			Model::RegisterDiffuseTextures(dragonModels[i], TextureDesc::MippedSRGB);
		}

		for (int i = 0; i < enemyModels.size(); ++i)
		{
			Model::RegisterDiffuseTextures(enemyModels[i], TextureDesc::MippedSRGB);
		}
	}

	Quaternion bindRot = Quaternion::RotateX(Math::ToRadians(90.0));
	const Model testModel{ U"example/fbx/Standing Idle.fbx", ColorOption::Default, 0.01f };
private:
	FbxModelManager() = default; // 外部から直接newできないようにする
};
