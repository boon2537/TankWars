#pragma once

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include <vector>

struct audio_group_sound_t
{
	audio_group_sound_t(const std::string& name, SoundID id, float weight)
		: soundID(id), weight(weight) {}

	std::string name;
	SoundID soundID;
	float weight = 1.f;
};

class AudioGroup
{
public:
	~AudioGroup() {};
	AudioGroup(const tinyxml2::XMLElement& element);

	SoundPlaybackID PlayOneOff(bool isLooped=false, float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false); 

public:
	std::string m_name;
	std::vector<audio_group_sound_t> m_sounds;

private:
	bool m_isUniformWeight = true;
};
