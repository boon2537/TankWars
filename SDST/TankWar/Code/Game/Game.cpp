#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/ThirdParty/tinyxml2.h"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Sprite.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Game/FlyingCamera.hpp"
#include "Engine/Debug/DebugRender.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Material/Material.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/RenderScene.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Renderer/ParticleSystem.hpp"
#include "Engine/Renderer/Skybox.hpp"
#include "Game/Terrain.hpp"
#include "Game/Projectile.hpp"
#include "Game/Enemy.hpp"
#include "Game/Spawner.hpp"
#include "Engine/UI/Canvas.hpp"
#include "Engine/UI/ImageUI.hpp"
#include "Engine/UI/TextUI.hpp"
#include "Engine/Profiler/ProfileLogScoped.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include <string>

constexpr float LIGHT_ROTATE_RATE = 0.5f;
constexpr float FADE_IN_TIME = 0.85f;
constexpr float FADE_OUT_TIME = 0.85f;
constexpr float BREAD_CRUMB_INTERVAL = 0.5f;

Game::~Game()
{
	delete m_forwardRenderingPath;
	m_forwardRenderingPath = nullptr;
	delete m_renderScene;
	m_renderScene = nullptr;
}

Game::Game()
{
	InitRandomSeed();
}

void Game::Initialize()
{
	m_gameClock = new Clock(GetMasterClock());
	m_fadeStopWatch.SetClock(m_gameClock);

	g_mainFont = g_theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont");
	DevConsole::GetInstance()->SetCurrentFont(g_mainFont);

	//Audio
	AudioSystem::LoadAudioGroups("Data/Audio/audioData.xml");
	m_currentBGM = AudioSystem::PlayClipOneOff("bgm", true, 0.1f);

	//Rendering
	m_forwardRenderingPath = new ForwardRenderingPath(g_theRenderer);
	m_renderScene = new RenderScene();
	m_mainMenuScene = new RenderScene();
	m_particleSystem = ParticleSystem::CreateInstance();

	m_gameCamera = new OrbitCamera();
	m_gameCamera->SetColorTarget(g_theRenderer->GetDefaultColorTarget());
	m_gameCamera->SetDepthStencilTarget(g_theRenderer->GetDefaultDepthTarget());
	m_gameCamera->SetProjectionPerspective(60.f, 16.f / 9.f, 1, 800);
	m_gameCamera->SetLocalRotationEuler(Vector3(0, 0, 0));
	m_renderScene->AddCamera(m_gameCamera);

	DebugRender* dr = DebugRender::GetInstance();
	dr->DebugRenderStartup(g_theRenderer);
	dr->DebugRenderSetClock(m_gameClock);
	dr->SetDebugRender3DCamera(m_gameCamera);

	m_menuCamera = new Camera();
	m_menuCamera->SetProjection(Matrix44::MakeOrtho(Vector2(0, 0), Vector2(1280, 720), 0, 1));
	m_menuCamera->SetColorTarget(g_theRenderer->GetDefaultColorTarget());
	m_menuCamera->SetDepthStencilTarget(g_theRenderer->GetDefaultDepthTarget());
	m_mainMenuScene->AddCamera(m_menuCamera);

	//Set Up HUD
	m_HUD_camera = new Camera();
	m_HUD_camera->SetColorTarget(g_theRenderer->GetDefaultColorTarget());
	m_HUD_camera->SetDepthStencilTarget(g_theRenderer->GetDefaultDepthTarget());
	m_canvas = new Canvas();
	m_canvas->SetUICamera(m_HUD_camera);
	m_canvas->SetDefaultFont(g_mainFont);

	m_canvas->m_canvasGroups.push_back(new CanvasGroup(m_canvas));
	//base count - 2
	TextUI* text = new TextUI(m_canvas);
	text->SetBounds(AABB2(0.025f, 0.05f, 0.2f, 0.1f));
	text->SetText("health");
	text->m_alignment = Vector2(0, 0.5f);
	text->m_height = 30.f;
	m_canvas->m_canvasGroups[0]->m_elements.push_back(text);
	//enemy - 1
	text = new TextUI(m_canvas);
	text->SetBounds(AABB2(0.025f, 0.1f, 0.2f, 0.15f));
	text->SetText("enemy");
	text->m_alignment = Vector2(0, 0.5f);
	text->m_height = 30.f;
	m_canvas->m_canvasGroups[0]->m_elements.push_back(text);
	//health - 0
	text = new TextUI(m_canvas);
	text->SetBounds(AABB2(0.025f, 0.15f, 0.2f, 0.2f));
	text->SetText("base");
	text->m_alignment = Vector2(0, 0.5f);
	text->m_height = 30.f;
	m_canvas->m_canvasGroups[0]->m_elements.push_back(text);

	//victory/defeat HUD
	m_canvas->m_canvasGroups.push_back(new CanvasGroup(m_canvas));
	text = new TextUI(m_canvas);
	text->SetBounds(AABB2::ZERO_TO_ONE);
	text->SetText("VICTORY!\n\nPRESS ENTER TO EXIT");
	text->m_color = Rgba::yellow;
	text->m_alignment = Vector2(0.5f, 0.5f);
	text->m_height = 50.f;
	m_canvas->m_canvasGroups[1]->m_elements.push_back(text);
	m_canvas->m_canvasGroups[1]->m_isActive = false;
}

void Game::Update()
{
	float currentTime = static_cast<float>(m_gameClock->GetClockCurrentTime());	
	float deltaSeconds = MinFloat(currentTime - m_lastTime, 0.05f);
	m_lastTime = currentTime;

	//do fading transition
	if (m_fadingIn || m_fadingOut)
	{
		if (m_fadeStopWatch.HasElapsed())
		{
			if (m_fadingIn)
			{
				// screen all black now, switch to another scene
				m_fadingIn = false;
				m_fadeStopWatch.SetTimer(FADE_OUT_TIME);
				m_blackOverlayAlpha = 255;
				m_currentState = m_transitionToState;
			}
			else if (m_fadingOut)
			{
				// be done with fade transition
				m_fadingIn = false;
				m_fadingOut = false;
				m_blackOverlayAlpha = 0;
				m_fadeStopWatch.Reset();
			}
		}
		else
		{
			//Update the black overlay alpha
			if (m_fadingIn)
			{
				m_blackOverlayAlpha = (unsigned char) Interpolate(0, 255, m_fadeStopWatch.GetNormalizedElapsedTime());
			}
			else if (m_fadingOut)
			{
				m_blackOverlayAlpha = (unsigned char) Interpolate(255, 0, m_fadeStopWatch.GetNormalizedElapsedTime());
			}
		}
	}

	switch (m_currentState)
	{
	case NONE:
		Update_NONE();
		break;
	case INIT:
		Update_INIT();
		break;
	case ATTRACT:
		Update_ATTRACT(deltaSeconds);
		break;
	case SETUP:
		Update_SETUP(deltaSeconds);
		break;
	case PLAYING:
		Update_PLAYING(deltaSeconds);
		break;
	case DEFEAT:
		Update_DEFEAT(deltaSeconds);
		break;
	case VICTORY:
		Update_VICTORY(deltaSeconds);
		break;
	}
}

void Game::Render()
{
	switch (m_currentState)
	{
	case NONE:
		Render_NONE();
		break;
	case INIT:
		Render_INIT();
		break;
	case ATTRACT:
		Render_ATTRACT();
		break;
	case SETUP:
		Render_SETUP();
		break;
	case PLAYING:
		Render_PLAYING();
		break;
	case DEFEAT:
		Render_DEFEAT();
		break;
	case VICTORY:
		Render_VICTORY();
		break;
	}
}

void Game::SetUpToPlay()
{
	if (m_isGameSetUp)
		return;

	m_isGameSetUp = true;

	RenderScene::SetCurrentScene(m_renderScene);
	m_ship = new Ship();

	//Spawn Spawners
	float margin = 50.f;
	for (int i = 0; i < m_spawnersToSpawn; i++)
	{
		Vector2 xy = Vector2(GetRandomFloatInRange(m_terrain->m_extents.mins.x + margin, m_terrain->m_extents.maxs.x - margin), 
			GetRandomFloatInRange(m_terrain->m_extents.mins.y + margin, m_terrain->m_extents.maxs.y - margin));
		Spawner* s = new Spawner(Vector3(xy.x, m_terrain->GetHeight(xy), xy.y), i);
		m_spawners.push_back(s);
	}

	m_canvas->m_canvasGroups[1]->m_isActive = false;
}

void Game::CleanUpPlay()
{
	if (!m_isGameSetUp)
		return;

	delete m_ship;
	m_ship = nullptr;

	for each (Enemy* e in m_enemies)
	{
		if (e != nullptr)
			delete e;
	}
	m_enemies.clear();

	for each (Spawner* s in m_spawners)
	{
		if (s != nullptr)
			delete s;
	}
	m_spawners.clear();

	for each (Projectile* p in m_projectiles)
	{
		if (p != nullptr)
			delete p;
	}
	m_projectiles.clear();

	m_isGameSetUp = false;
}

bool Game::Raycast(RayCastHit3* outResults, const Ray3& ray)
{
	//check raycast against enemies
	for each (Enemy* s in m_enemies)
	{
		if (s != nullptr && !s->IsDead())
		{
			*outResults = RayCheckSphere(ray, s->m_transform.GetWorldPosition(), s->m_radius);
			if (outResults->hit)
				return true;
		}
	}

	//check raycast against spawners
	for each (Spawner* s in m_spawners)
	{
		if (s != nullptr && !s->IsDead())
		{
			*outResults = RayCheckAABB3(ray, s->m_bounds);
			if (outResults->hit)
				return true;
		}
	}

	return m_terrain->Raycast(outResults, ray);
}

GAME_STATE Game::GetCurrentState()
{
	return m_currentState;
}

void Game::StartTransitionToState(GAME_STATE newGameState, bool applyFadeInOut)
{
	if (m_fadingIn || m_fadingOut)
		return;

	m_transitionToState = newGameState;

	if (applyFadeInOut)
	{
		m_fadingIn = true;
		m_fadingOut = true;
		m_fadeStopWatch.SetTimer(FADE_IN_TIME);
	}
	if (!m_fadingIn && !m_fadingOut)
	{
		m_currentState = m_transitionToState;
	}
}

void Game::Update_NONE()
{
	//force transition to INIT
	StartTransitionToState(GAME_STATE::INIT, false);
}

void Game::Update_INIT()
{
	g_terrainSpriteSheet = new SpriteSheet(*g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png"), 8, 8);
	m_litShader = g_theRenderer->CreateOrGetShader("Data/Shaders/lit.xml");
	g_theRenderer->CreateOrGetShader("Data/Shaders/diffuse.xml");
	m_projectileMat = Material::GetOrCreate("Data/Materials/bullet.xml");
	RenderScene::SetCurrentScene(m_renderScene);

	g_theInput->MouseLockToScreen(true);
	g_theInput->SetMouseCursorVisible(false);
	g_theInput->SwitchMouseMode(MOUSEMODE::MOUSEMODE_RELATIVE);

	//Debug shader render
	m_debugTypes.push_back(debug_shader_render_t("DEFAULT", Vector4(1, 1, 1, 0)));
	m_debugTypes.push_back(debug_shader_render_t("COLOR ONLY", Vector4(1, 0, 0, 0)));
	m_debugTypes.push_back(debug_shader_render_t("DIFFUSE LIGHT", Vector4(0, 1, 0, 0)));
	m_debugTypes.push_back(debug_shader_render_t("SPEC LIGHT", Vector4(0, 0, 1, 0)));
	m_debugTypes.push_back(debug_shader_render_t("FULL LIGHT", Vector4(0, 1, 1, 0)));
	m_debugTypes.push_back(debug_shader_render_t("UV", Vector4(1, 1, 1, 1)));
	m_debugTypes.push_back(debug_shader_render_t("VERTEX NORMAL", Vector4(1, 1, 1, 2)));
	m_debugTypes.push_back(debug_shader_render_t("VERTEX TANGENT", Vector4(1, 1, 1, 3)));
	m_debugTypes.push_back(debug_shader_render_t("VERTEX BITANGENT", Vector4(1, 1, 1, 4)));
	m_debugTypes.push_back(debug_shader_render_t("SURFACE NORMAL", Vector4(1, 1, 1, 5)));
	m_debugTypes.push_back(debug_shader_render_t("WORLD NORMAL", Vector4(1, 1, 1, 6)));

	//Lights
	g_theRenderer->SetAmbientLight(0.1f, Rgba::white);
	g_theRenderer->SetFog(Rgba(150, 150, 150, 255), 120, 250, 0.1f, 0.9f);

	m_directionalLight = new Light();
	m_spotLight = new Light();

	Transform directionalLightTransform;
	directionalLightTransform.SetLocalPosition(Vector3(0.f, 160.f, -60));
	directionalLightTransform.SetLocalRotationEuler(Vector3(ConvertDegreesToRadians(40.f), 0, 0));
	m_directionalLight->m_transform = directionalLightTransform;
	m_directionalLight->SetUpAsDirectional(Rgba::white, 0.9f, Vector3(1.f, 0, 0));
	m_directionalLight->m_usesShadow = true;

	m_renderScene->AddLight(m_directionalLight);

	m_skybox = new Skybox("Data/Images/skybox.png");
	m_skybox->SetCamera(m_gameCamera);

	//Terrain
	m_terrain = new Terrain();
	g_theRenderer->CreateOrGetShader("Data/Shaders/rolling.xml");
	m_terrain->SetMaterialStamp("Data/Materials/grass.xml", "Data/Materials/water.xml");
	m_terrain->LoadFromImage("Data/Images/heightmap.jpg", AABB2(-164, -164, 164, 164), 0, 32, IntVector2(16, 16));
	m_terrain->SetUp()

	TODO("remove sleep later");
	Sleep(1000.f);

	//force transition to ATTRACT
	StartTransitionToState(GAME_STATE::ATTRACT, false);
	tempMenuIndex = 0;

	//set up bread crumb timer
	m_breadCrumbTimer.SetClock(m_gameClock);
	m_breadCrumbTimer.SetTimer(BREAD_CRUMB_INTERVAL);
}

void Game::Update_ATTRACT(float deltaSeconds)
{
	ProfilerPush(__FUNCTION__);

	UNUSED(deltaSeconds);

	RenderScene::SetCurrentScene(m_mainMenuScene);
	m_forwardRenderingPath->m_skybox = nullptr;

	if (g_theInput->WasKeyJustPressed(KEY_CODE::RETURN) && tempMenuIndex == 0)
	{
		StartTransitionToState(GAME_STATE::SETUP, true);
	}

	if (g_theInput->WasKeyJustPressed(KEY_CODE::W) || g_theInput->WasKeyJustPressed(KEY_CODE::UP_ARROW))
	{
		tempMenuIndex = MaxInt(0, tempMenuIndex - 1);
	}
	if (g_theInput->WasKeyJustPressed(KEY_CODE::S) || g_theInput->WasKeyJustPressed(KEY_CODE::DOWN_ARROW))
	{
		tempMenuIndex = MinInt(1, tempMenuIndex + 1);
	}

	ProfilerPop();
}

void Game::Update_SETUP(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	RenderScene::SetCurrentScene(m_mainMenuScene);
	m_forwardRenderingPath->m_skybox = nullptr;

	if (g_theInput->WasKeyJustPressed(KEY_CODE::RETURN))
	{
		StartTransitionToState(GAME_STATE::PLAYING, true);
		SetUpToPlay();
	}
}

void Game::Update_PLAYING(float deltaSeconds)
{
	RenderScene::SetCurrentScene(m_renderScene);
	m_forwardRenderingPath->m_skybox = m_skybox;

	if (!m_isGameSetUp)
		return;

	//update camera
	float camRotateSpeed = 0.01f;
	m_rot -= g_theInput->GetMouseDelta().x * camRotateSpeed;
	m_azi += g_theInput->GetMouseDelta().y * camRotateSpeed;
	m_azi = ClampFloat(m_azi, -m_aziLimit, m_aziLimit);

	m_gameCamera->SetTarget(m_ship->m_transform.GetLocalPosition() + (m_ship->m_transform.GetLocalMatrix().GetUp() * 8.f) + 
		(m_ship->m_transform.GetLocalMatrix().GetForward() * 0.f));
	m_gameCamera->SetSphericalCoordinate(25, m_rot, m_azi);

	m_ship->UpdateMovement(deltaSeconds);
	m_ship->UpdateTarget(deltaSeconds);

	if (g_theInput->WasKeyJustPressed(KEY_CODE::F))
	{
		m_currentRenderMode--;
		if (m_currentRenderMode < 0)
			m_currentRenderMode = (int) m_debugTypes.size() - 1;
	}
	if (g_theInput->WasKeyJustPressed(KEY_CODE::G))
	{
		m_currentRenderMode++;
		if (m_currentRenderMode >= m_debugTypes.size())
			m_currentRenderMode = 0;
	}

	DebugLogf("WASD to move", Rgba::red, 0);
	DebugLogf("Left Click to shoot", Rgba::red, 0);
	DebugLogf("Z for Godmode", Rgba::red, 0);
	DebugLogf("X for Instant Win", Rgba::red, 0);

	if (g_theInput->WasKeyJustPressed(KEY_CODE::Z))
	{
		m_DEBUG = !m_DEBUG;
		if (m_DEBUG)
			DebugLogf("Godmode ON", Rgba::green, 4);
		else
			DebugLogf("Godmode OFF", Rgba::blue, 4);
	}

	//Game Objects
	for each (Enemy* e in m_enemies)
	{
		e->Update(deltaSeconds);
	}

	for each (Spawner* s in m_spawners)
	{
		if (s != nullptr)
			s->Update(deltaSeconds);
	}

	//check for enemy vs player overlap
	for each (Enemy* e in m_enemies)
	{
		if (DoSpheresOverlap(e->m_transform.GetLocalPosition(), e->m_radius, m_ship->m_transform.GetLocalPosition(), m_ship->m_physicsRadius))
		{
			m_ship->TakeDamage(e->m_damage);
			e->Destroy();
		}
	}

	int enemiesRemaining = 0;
	for (int i = 0; i < m_enemies.size(); ++i)
	{
		enemiesRemaining++;
		Enemy* e = m_enemies[i];
		for each (Projectile* p in m_projectiles)
		{
			if (p != nullptr)
				e->CheckProjectileOverlap(p);
		}

		if (e->IsDead())
		{
			enemiesRemaining--;
			delete e;

			size_t size = m_enemies.size();
			m_enemies[i] = m_enemies[size - 1];
			m_enemies.pop_back();
			i--;
		}
	}

	//hacky -- don't resize the spawner array
	int spawnersRemaining = 0;
	for (int i = 0; i < m_spawners.size(); ++i)
	{
		Spawner* s = m_spawners[i];

		if (s != nullptr)
		{
			spawnersRemaining++;
			for each (Projectile* p in m_projectiles)
			{
				if (p != nullptr)
					s->CheckProjectileOverlap(p);
			}

			if (s->IsDead())
			{
				spawnersRemaining--;
				delete s;
				m_spawners[i] = nullptr;
			}
		}
	}

	for (int i = 0; i < m_projectiles.size(); ++i)
	{
		Projectile* p = m_projectiles[i];
		p->Update(deltaSeconds);
		if (p->IsDead())
		{
			delete p;

			size_t size = m_projectiles.size();
			m_projectiles[i] = m_projectiles[size - 1];
			m_projectiles.pop_back();
			i--;
		}
	}

	m_ship->ApplyStickToTerrain(m_terrain);

	//DebugRenderBasis(0, m_ship->m_transform.GetLocalMatrix());

	//drop bread crumb
	if (m_breadCrumbTimer.DecrementAll() > 0)
	{
		//DebugRenderPoint(4.0f, m_ship->m_transform.GetWorldPosition(), Rgba::green, Rgba::red);
	}
	
	m_particleSystem->Update(m_gameCamera, deltaSeconds);

	//Update UI Elements
	TextUI* text = (TextUI*) m_canvas->m_canvasGroups[0]->m_elements[2];
	text->SetText("Bases: " + std::to_string(spawnersRemaining));
	text = (TextUI*) m_canvas->m_canvasGroups[0]->m_elements[1];
	text->SetText("Enemies: " + std::to_string(enemiesRemaining));
	text = (TextUI*) m_canvas->m_canvasGroups[0]->m_elements[0];
	text->SetText("Health: " + std::to_string(m_ship->m_currentHP));

	//All enemy dead - VICTORY
	if (g_theInput->WasKeyJustPressed(KEY_CODE::X) || (spawnersRemaining == 0 && enemiesRemaining == 0))
	{
		text = (TextUI*) m_canvas->m_canvasGroups[1]->m_elements[0];
		text->SetText("VICTORY!\n\nPRESS ENTER TO EXIT");
		m_canvas->m_canvasGroups[1]->m_isActive = true;
		StartTransitionToState(GAME_STATE::VICTORY, false);
	}
}

void Game::Update_DEFEAT(float deltaSeconds)
{
	Update_PLAYING(deltaSeconds);

	TextUI* text = (TextUI*) m_canvas->m_canvasGroups[1]->m_elements[0];
	text->SetText("YOU DIED!\n\nRESPAWNING IN " + std::to_string((int) m_timeTilRespawn + 1));
	m_canvas->m_canvasGroups[1]->m_isActive = true;

	m_timeTilRespawn -= deltaSeconds;
	if (m_timeTilRespawn <= 0)
	{
		text->SetText("LEFT CLICK TO RESPAWN");

		if (g_theInput->WasMouseJustPressed(MOUSE_CODE::BUTTON_LEFT))
		{
			m_canvas->m_canvasGroups[1]->m_isActive = false;

			float margin = 50.f;
			Vector2 xy = Vector2(GetRandomFloatInRange(m_terrain->m_extents.mins.x + margin, m_terrain->m_extents.maxs.x - margin), 
				GetRandomFloatInRange(m_terrain->m_extents.mins.y + margin, m_terrain->m_extents.maxs.y - margin));

			m_ship->Respawn(Vector3(xy.x, 0, xy.y));
			StartTransitionToState(GAME_STATE::PLAYING, false);
			m_isRespawning = false;
		}
	}
}

void Game::Update_VICTORY(float deltaSeconds)
{
	Update_PLAYING(deltaSeconds);

	if (g_theInput->WasKeyJustPressed(KEY_CODE::RETURN))
	{
		StartTransitionToState(GAME_STATE::ATTRACT, true);
		CleanUpPlay();
	}
}

void Game::Render_NONE()
{
	g_theRenderer->SetShader(nullptr);
	RenderBlackOverlay();
}

void Game::Render_INIT()
{
	g_theRenderer->SetShader(nullptr);

	g_theRenderer->SetCamera(m_menuCamera);
	g_theRenderer->DisableDepth();
	g_theRenderer->ClearColor(Rgba::black);
	g_theRenderer->ClearDepth(1.0f); 

	g_theRenderer->DrawTextInBox2D(AABB2(0, 0, 1280, 720), "LOADING...", 30.f, Rgba::yellow, 1.f, nullptr, OVERRUN, Vector2(0.5f, 0.5f));
	RenderBlackOverlay();
}

void Game::Render_ATTRACT()
{
	g_theRenderer->SetShader(nullptr);

	g_theRenderer->SetCamera(m_menuCamera);
	g_theRenderer->DisableDepth();
	g_theRenderer->ClearColor(Rgba::black);
	g_theRenderer->ClearDepth(1.0f); 

	g_theRenderer->DrawTextInBox2D(AABB2(0, 0, 1280, 720), "START", 30.f, tempMenuIndex == 0 ? Rgba::yellow : Rgba::white, 1.f, nullptr, OVERRUN, Vector2(0.5f, 0.4f));
	g_theRenderer->DrawTextInBox2D(AABB2(0, 0, 1280, 720), "EXIT", 30.f, tempMenuIndex == 1 ? Rgba::yellow : Rgba::white, 1.f, nullptr, OVERRUN, Vector2(0.5f, 0.6f));
	RenderBlackOverlay();
}

void Game::Render_SETUP()
{
	g_theRenderer->SetShader(nullptr);

	g_theRenderer->SetCamera(m_menuCamera);
	g_theRenderer->DisableDepth();
	g_theRenderer->ClearColor(Rgba::black);
	g_theRenderer->ClearDepth(1.0f); 

	g_theRenderer->DrawTextInBox2D(AABB2(0, 0, 1280, 720), "PRESS ENTER TO CONTINUE", 30.f, Rgba::yellow, 1.f, nullptr, OVERRUN, Vector2(0.5f, 0.5f));
	RenderBlackOverlay();
}

void Game::Render_PLAYING()
{
	//set dangling uniforms
	g_theRenderer->SetShader(m_litShader);
	g_theRenderer->SetUniform("EYE_POSITION", m_gameCamera->m_transform.GetWorldPosition());
	g_theRenderer->SetUniform("RENDER_MIX", m_debugTypes[m_currentRenderMode].mix);
	g_theRenderer->UpdateTimeBlock(static_cast<float>(m_gameClock->GetClockCurrentTime()));

	if (m_ship != nullptr)
		ForwardRenderingPath::s_lightFocalPoint = &m_ship->m_transform;

	m_forwardRenderingPath->Render(m_renderScene);

	m_directionalLight->DebugRender();

	m_canvas->Render();

	g_theRenderer->SetShader(nullptr);
	g_theRenderer->SetCamera(m_menuCamera);
	g_theRenderer->DisableDepth();
	RenderBlackOverlay();

	ForwardRenderingPath::s_lightFocalPoint = nullptr;
}

void Game::Render_DEFEAT()
{
	Render_PLAYING();
}

void Game::Render_VICTORY()
{
	Render_PLAYING();
}

void Game::RenderBlackOverlay()
{
	if (m_fadingIn || m_fadingOut)
	{
		g_theRenderer->DrawAABB(AABB2(0, 0, 2000, 2000), Rgba(0, 0, 0, m_blackOverlayAlpha));
	}
}

void Game::SetDefeat()
{
	if (m_isRespawning)
		return;

	StartTransitionToState(GAME_STATE::DEFEAT, false);
	m_timeTilRespawn = m_respawnTime;
	m_isRespawning = true;
}

Enemy* Game::SpawnEnemy(const Vector3& pos, Spawner* spawner)
{
	Enemy* e = new Enemy(pos, spawner->m_id);
	m_enemies.push_back(e);
	return e;
}

bool Game::IsSpawnerAlive(int id)
{
	return m_spawners[id] != nullptr;
}
