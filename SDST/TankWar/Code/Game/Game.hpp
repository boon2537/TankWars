#pragma once
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Command.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Math/Ray.hpp"
#include "Engine/Physics/Contact.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/Ship.hpp"

struct debug_shader_render_t
{
	debug_shader_render_t(const std::string& mode, const Vector4& mix)
		: mode(mode), mix(mix) {}

	std::string mode;
	Vector4 mix;
};

class Clock;
class Shader;
class Camera;
class Mesh;
class FlyingCamera;
class Material;
class Renderable;
class ForwardRenderingPath;
class RenderScene;
class TextureCube;
class ParticleEmitter;
class ParticleSystem;
class Skybox;
class Enemy;
class Spawner;
class Projectile;
class Terrain;
class Canvas;

enum GAME_STATE
{
	NONE,
	INIT,
	ATTRACT,
	SETUP,
	PLAYING,
	DEFEAT,
	VICTORY
};

class Game
{
public:
	~Game();
	Game();

	void Initialize();		

	void Update();
	void Render();
	void SetUpToPlay();
	void CleanUpPlay();

	bool Raycast(RayCastHit3* outResults, const Ray3& ray);

	GAME_STATE GetCurrentState();
	void StartTransitionToState(GAME_STATE newGameState, bool applyFadeInOut);

	void Update_NONE(); //only exist for a frame to change state to INIT
	void Update_INIT(); //only exist for a frame to change state to ATTRACT
	void Update_ATTRACT(float deltaSeconds);
	void Update_SETUP(float deltaSeconds);
	void Update_PLAYING(float deltaSeconds);
	void Update_DEFEAT(float deltaSeconds);
	void Update_VICTORY(float deltaSeconds);

	void Render_NONE(); //render an overlay scene here
	void Render_INIT();
	void Render_ATTRACT();
	void Render_SETUP();
	void Render_PLAYING();
	void Render_DEFEAT();
	void Render_VICTORY();
	void RenderBlackOverlay();

	void SetDefeat();
	Enemy* SpawnEnemy(const Vector3& pos, Spawner* spawner);

	//hacky
	bool IsSpawnerAlive(int id);

public:
	bool m_DEBUG = false;

	SoundPlaybackID m_currentBGM = MISSING_SOUND_ID;
	Shader* m_litShader = nullptr;
	Clock* m_gameClock = nullptr;
	bool m_isGameSetUp = false;

	OrbitCamera* m_gameCamera = nullptr;
	float m_aziLimit = CosDegrees(30.f); // min/max of x rot
	float m_camDistanceFromTank = 10.f;
	float m_rot;
	float m_azi;

	Camera* m_menuCamera = nullptr;

	Light m_lights[MAX_LIGHTS];
	int m_currentRenderMode = 0;
	std::vector<debug_shader_render_t> m_debugTypes;

	Light* m_directionalLight = nullptr;
	Light* m_spotLight = nullptr;
	ForwardRenderingPath* m_forwardRenderingPath = nullptr;
	RenderScene* m_renderScene = nullptr;
	RenderScene* m_mainMenuScene = nullptr;
	Skybox* m_skybox = nullptr;
	Terrain* m_terrain = nullptr;

	ParticleSystem* m_particleSystem = nullptr;

	//Game Objects
	Ship* m_ship = nullptr;
	Material* m_projectileMat = nullptr;

	std::vector<Enemy*> m_enemies;
	std::vector<Projectile*> m_projectiles;

	int m_spawnersToSpawn = 7;
	std::vector<Spawner*> m_spawners;

	//Game state
	GAME_STATE m_currentState = GAME_STATE::NONE;
	GAME_STATE m_transitionToState = GAME_STATE::NONE;  
	StopWatch m_fadeStopWatch;
	StopWatch m_breadCrumbTimer;
	bool m_fadingIn = false;
	bool m_fadingOut = false;
	unsigned char m_blackOverlayAlpha = 0;

	float m_timeTilRespawn;
	float m_respawnTime = 3.f;
	bool m_isRespawning = false;

	//Hacky UI --- remove later
	int tempMenuIndex = 0;
	Camera* m_HUD_camera = nullptr;
	Canvas* m_canvas = nullptr;

private:
	float m_lastTime = 0;
};
