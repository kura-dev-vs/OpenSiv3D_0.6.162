#pragma once
#include <Siv3D.hpp>
#include "Siv3D/AddScripts/AssimpHelper.h"

class AudioManager
{
public:
	static AudioManager& Instance()
	{
		static AudioManager instance;
		return instance;
	}

	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;

	const Audio eggAudio{ U"example/audio/egg.mp3", Loop::Yes };
	const Audio tameAudio{ U"example/audio/tame.mp3", Loop::Yes };
	const Audio evoAudio{ U"example/audio/evo.mp3", Loop::Yes };
	const Audio deathAudio{ U"example/audio/death.mp3", Loop::Yes };
	const Audio battleAudio{ U"example/audio/battle.mp3" , Loop::Yes };
private:
	AudioManager() = default; // 外部から直接newできないようにする
};
