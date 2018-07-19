#pragma once


//-----------------------------------------------------------------------------------------------
#include "Engine/ThirdParty/fmod/fmod.hpp"
#include <string>
#include <vector>
#include <map>


//-----------------------------------------------------------------------------------------------
typedef size_t SoundID;
typedef size_t SoundPlaybackID;
constexpr size_t MISSING_SOUND_ID = (size_t)(-1); // for bad SoundIDs and SoundPlaybackIDs


//-----------------------------------------------------------------------------------------------
class AudioGroup;


/////////////////////////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
public:
	AudioSystem();
	virtual ~AudioSystem();

public:
	virtual void				BeginFrame();
	virtual void				EndFrame();

	virtual SoundID				CreateOrGetSound( const std::string& soundFilePath );
	virtual SoundPlaybackID		PlaySound( SoundID soundID, bool isLooped=false, float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false );
	virtual void				StopSound( SoundPlaybackID soundPlaybackID );
	virtual void				SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume );	// volume is in [0,1]
	virtual void				SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance );	// balance is in [-1,1], where 0 is L/R centered
	virtual void				SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed );		// speed is frequency multiplier (1.0 == normal)

	virtual void				ValidateResult( FMOD_RESULT result );

public:
	static AudioSystem* CreateInstance();
	static AudioSystem* GetInstance();
	static void LoadAudioGroups(const std::string& path);
	static SoundPlaybackID PlayOneOff(const std::string& name, bool isLooped=false, float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false);
	static SoundPlaybackID PlayClipOneOff(const std::string& clipName, bool isLooped=false, float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false);
	static SoundID FindClip(const std::string& clipName);

public:
	static std::map<std::string, AudioGroup*> s_audioGroups;

protected:
	FMOD::System*						m_fmodSystem;
	std::map< std::string, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;

	std::map<std::string, SoundID>		m_audioClips;
};

