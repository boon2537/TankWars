#pragma once

#include "Engine/Renderer/SpriteAnimDefinition.hpp"

class SpriteAnim
{
public:
	SpriteAnim( const SpriteAnimDefinition* animDef );
	~SpriteAnim();

	void Update( float deltaSeconds );
	void PlayFromStart(); // Rewinds to time 0 and starts playing/replaying
	bool IsFinished() const { return m_isFinished; }
	void Pause();				// Starts unpaused (playing) by default
	void Resume();				// Resume after pausing	
	float GetElapsedSeconds() const { return m_elapsedSeconds; }
	float GetElapsedFraction() const;
	float GetRemainingSeconds() const;
	float GetRemainingFraction() const;
	float GetDuration() const;
	void SetSecondsElapsed( float secondsElapsed );	    // Jump to specific time
	void SetFractionElapsed( float fractionElapsed );   // e.g. 0.33f for one-third in
	const Texture& GetTexture() const;
	AABB2 GetCurrentUVs() const;
	std::string GetName() const;

	bool ShouldAutoOrient() const;

public:
	float m_speed = 1.f;

protected:
	const SpriteAnimDefinition*		m_animDef = nullptr;
	bool					m_isPlaying = true;
	bool					m_isFinished = false;
	float					m_elapsedSeconds = 0.f;
};
