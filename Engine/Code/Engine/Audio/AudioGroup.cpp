#include "Engine/Audio/AudioGroup.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Math/MathUtils.hpp"

AudioGroup::AudioGroup(const tinyxml2::XMLElement& element)
{
	m_name = ParseXmlAttribute(element, "name", "empty");

	const tinyxml2::XMLElement* elementToUse = element.FirstChildElement();
	while (elementToUse != nullptr)
	{
		std::string elementName = elementToUse->Name();
		if (elementName == "Clip")
		{
			std::string clipName = ParseXmlAttribute(*elementToUse, "name", "empty");
			float weight = ParseXmlAttribute(*elementToUse, "weight", 1.f);
			m_sounds.push_back(audio_group_sound_t(clipName, AudioSystem::FindClip(clipName), weight));

			if (weight != 1.f)
				m_isUniformWeight = false;
		}

		elementToUse = elementToUse->NextSiblingElement();
	}
}

size_t AudioGroup::PlayOneOff(bool isLooped, float volume, float balance, float speed, bool isPaused)
{
	if (m_sounds.size() == 0)
		return MISSING_SOUND_ID;

	if (m_isUniformWeight)
	{
		return AudioSystem::GetInstance()->PlaySound(m_sounds[GetRandomIntLessThan(m_sounds.size())].soundID, isLooped, volume, balance, speed, isPaused);
	}
	else
	{
		//simple weighted random, update later
		SoundID idToUse = MISSING_SOUND_ID;
		float sumWeight = 0;
		for(int i = 0; i < m_sounds.size(); i++)
		{
			sumWeight += m_sounds[i].weight;
		}
		float rnd = GetRandomFloatInRange(0, sumWeight);
		for(int i = 0; i < m_sounds.size(); i++) 
		{
			if(rnd < m_sounds[i].weight)
			{
				idToUse = m_sounds[i].soundID;
				break;
			}

			rnd -= m_sounds[i].weight;
		}

		return AudioSystem::GetInstance()->PlaySound(idToUse, isLooped, volume, balance, speed, isPaused);
	}
}
