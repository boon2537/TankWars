#pragma once

#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"
#include <string>
#include <map>

class SpriteAnimSet
{
public:
	SpriteAnimSet( SpriteAnimSetDefinition* animSetDef );
	~SpriteAnimSet();

	void 			Update( float deltaSeconds );
	void 			StartAnim( const std::string& animName );
	void 			SetCurrentAnim( const std::string& animName );
	void			SetCurrentAnimSpeed( float speed );
	void			SetElapsedSeconds( float seconds );
	float			GetCurrentAnimSpeed();
	bool			HasAnim( const std::string& animName );
	const Texture& 	GetCurrentTexture() const;
	AABB2 			GetCurrentUVs() const;
	float			GetElapsedSeconds() const;
	float			GetCurrentAnimDuration() const;
	bool			ShouldAutoOrient() const;
	std::string		GetCurrentAnimName() const;

protected:
	SpriteAnimSetDefinition* m_animSetDef = nullptr;
	std::map<std::string, SpriteAnim*> m_namedAnims;
	SpriteAnim* m_currentAnim = nullptr;
};
