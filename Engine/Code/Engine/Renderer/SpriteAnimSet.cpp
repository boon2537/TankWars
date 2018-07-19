#include "Engine/Renderer/SpriteAnimSet.hpp"
#include "Engine/Renderer/SpriteAnim.hpp"

SpriteAnimSet::SpriteAnimSet(SpriteAnimSetDefinition * animSetDef)
	: m_animSetDef(animSetDef)
{
	std::map<std::string, SpriteAnimDefinition*>::iterator it;

	for (it = m_animSetDef->m_namedAnimDefs.begin(); it != m_animSetDef->m_namedAnimDefs.end(); it++)
	{
		SpriteAnim* anim = new SpriteAnim(it->second);
		m_namedAnims[it->first] = anim;
	}
	StartAnim(m_animSetDef->m_defaultAnimName);
}

SpriteAnimSet::~SpriteAnimSet()
{
	std::map<std::string, SpriteAnim*>::iterator it;
	for (it = m_namedAnims.begin(); it != m_namedAnims.end(); it++)
		delete it->second;
}

void SpriteAnimSet::Update(float deltaSeconds)
{
	m_currentAnim->Update(deltaSeconds);
}

void SpriteAnimSet::StartAnim(const std::string & animName)
{
	SetCurrentAnim(animName);
	m_currentAnim->PlayFromStart();
}

void SpriteAnimSet::SetCurrentAnim(const std::string & animName)
{
	if (HasAnim(animName))
		m_currentAnim = m_namedAnims[animName];
}

void SpriteAnimSet::SetCurrentAnimSpeed(float speed)
{
	if (m_currentAnim != nullptr)
		m_currentAnim->m_speed = speed;
}

void SpriteAnimSet::SetElapsedSeconds(float seconds)
{
	if (m_currentAnim != nullptr)
		m_currentAnim->SetSecondsElapsed(seconds);
}

float SpriteAnimSet::GetCurrentAnimSpeed()
{
	if (m_currentAnim != nullptr)
		return m_currentAnim->m_speed;
	
	return 1;
}

bool SpriteAnimSet::HasAnim(const std::string & animName)
{
	std::map<std::string, SpriteAnimDefinition*>::iterator search = m_animSetDef->m_namedAnimDefs.find(animName);
	return search != m_animSetDef->m_namedAnimDefs.end();
}

const Texture& SpriteAnimSet::GetCurrentTexture() const
{
	return m_currentAnim->GetTexture();
}

AABB2 SpriteAnimSet::GetCurrentUVs() const
{
	return m_currentAnim->GetCurrentUVs();
}

float SpriteAnimSet::GetElapsedSeconds() const
{
	if (m_currentAnim != nullptr)
		return m_currentAnim->GetElapsedSeconds();

	return 0;
}

float SpriteAnimSet::GetCurrentAnimDuration() const
{
	if (m_currentAnim != nullptr)
		return m_currentAnim->GetDuration();

	return 0;
}

bool SpriteAnimSet::ShouldAutoOrient() const
{
	return m_currentAnim->ShouldAutoOrient();
}

std::string SpriteAnimSet::GetCurrentAnimName() const
{
	if (m_currentAnim == nullptr)
		return "";

	return m_currentAnim->GetName();
}
