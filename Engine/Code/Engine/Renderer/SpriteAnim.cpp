#include "Engine/Renderer/SpriteAnim.hpp"

SpriteAnim::SpriteAnim(const SpriteAnimDefinition * animDef)
	: m_animDef(animDef)
{

}

SpriteAnim::~SpriteAnim()
{

}

void SpriteAnim::Update(float deltaSeconds)
{
	if (m_isPlaying && !m_isFinished)
	{
		m_elapsedSeconds += (deltaSeconds * m_speed);

		if (m_elapsedSeconds >= m_animDef->GetDuration())
		{
			if (m_animDef->m_mode == SPRITE_ANIM_MODE::SPRITE_ANIM_MODE_PLAY_TO_END)
			{			
				m_isPlaying = false;
				m_isFinished = true;
			}
			else if (m_animDef->m_mode == SPRITE_ANIM_MODE::SPRITE_ANIM_MODE_LOOPING)
			{
				m_elapsedSeconds = 0;
			}
		}
	}
}

void SpriteAnim::PlayFromStart()
{
	m_elapsedSeconds = 0;
	m_isPlaying = true;
	m_isFinished = false;
}

void SpriteAnim::Pause()
{
	m_isPlaying = false;
}

void SpriteAnim::Resume()
{
	m_isPlaying = true;
}

float SpriteAnim::GetElapsedFraction() const
{
	return m_elapsedSeconds / m_animDef->GetDuration();
}

float SpriteAnim::GetRemainingSeconds() const
{
	return m_animDef->GetDuration() - m_elapsedSeconds;
}

float SpriteAnim::GetRemainingFraction() const
{
	return 1.f - GetElapsedFraction();
}

float SpriteAnim::GetDuration() const
{
	return m_animDef->GetDuration();
}

void SpriteAnim::SetSecondsElapsed(float secondsElapsed)
{
	m_elapsedSeconds = secondsElapsed; //TODO: perhaps change this for loop-mode?
}

void SpriteAnim::SetFractionElapsed(float fractionElapsed)
{
	m_elapsedSeconds = ClampFloat(fractionElapsed, 0, 1) * m_animDef->GetDuration(); //TODO: perhaps change this for loop-mode so you can go over 1?
}

const Texture& SpriteAnim::GetTexture() const
{
	return m_animDef->m_spriteSheet->GetTexture();
}

AABB2 SpriteAnim::GetCurrentUVs() const
{
	int spriteIndex = m_animDef->GetSpriteIndexAtTime(m_elapsedSeconds);
	return m_animDef->m_spriteSheet->GetTexCoordsForSpriteIndex(spriteIndex);
}

std::string SpriteAnim::GetName() const
{
	return m_animDef->m_name;
}

bool SpriteAnim::ShouldAutoOrient() const
{
	return m_animDef->m_shouldAutoOrient;
}
