#include "Game/Game.hpp"

#include "Game/PlayerShip.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Bullet.hpp"
#include "Game/Debris.hpp"
#include "Game/TieFighter.hpp"
#include "Game/TieBomber.hpp"
#include "Game/Star.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Game::Game()
{
}

Game::~Game()
{
}

void Game::StartUp()
{
	if (m_isAttractMode)
	{
		m_attractModeMusic = g_theAudio->CreateOrGetSound("Data/Audio/Star Wars Theme.mp3");
		m_attractModePlayback = g_theAudio->StartSound(m_attractModeMusic, true, 0.8f);
	}
	else if (m_isMainMenu)
	{
		m_mainMenuMusic = g_theAudio->CreateOrGetSound("Data/Audio/MainTheme.mp3");
		m_mainMenuPlayback = g_theAudio->StartSound(m_mainMenuMusic, true);
	}

	m_screenCamera = new Camera();
	m_worldCamera = new Camera();

	m_playerShip = new PlayerShip(this, Vec2(WORLD_CENTER_X, WORLD_CENTER_Y));
	m_numOfPlayerLives = 3;

	GenerateStarMap();

	SoundID ambient = g_theAudio->CreateOrGetSound("Data/Audio/Ambient.mp3");
	g_theAudio->StartSound(ambient, true, 0.3f);
}

void Game::Shutdown()
{
	DELETE_PTR(m_worldCamera);
	DELETE_PTR(m_screenCamera);
	DELETE_PTR(m_playerShip);

	for (int index = 0; index < MAX_BULLETS; index++)
	{
		DELETE_PTR(m_bullets[index])
	}

	for (int index = 0; index < MAX_ASTEROIDS; index++)
	{
		DELETE_PTR(m_asteroids[index])
	}

	for (int index = 0; index < MAX_DEBRIS; index++)
	{
		DELETE_PTR(m_debris[index])
	}

	for (int index = 0; index < MAX_BOMBERS; index++)
	{
		DELETE_PTR(m_tieBombers[index])
	}

	for (int index = 0; index < MAX_STARS; index++)
	{
		DELETE_PTR(m_starMap[index])
	}

	for (int index = 0; index < MAX_FIGHTERS; index++)
	{
		DELETE_PTR(m_tieFighters[index])
	}
}

void Game::Update(float deltaseconds)
{
	if (m_isAttractMode)
	{
		UpdateAttractMode(deltaseconds);

		m_screenCamera->SetOrthoView(Vec2(0.0f, 0.0f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
	}
	else if (m_isMainMenu)
	{
		UpdateMainMenu(deltaseconds);

		m_screenCamera->SetOrthoView(Vec2(0.0f, 0.0f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
	}
	else
	{
		g_theAudio->StopSound(m_mainMenuPlayback);

		if (m_numOfPlayerLives < 0 || m_waveMode == 6)
		{
			m_gameEndTimer -= deltaseconds;
		}

		if (m_isPlayerShieldActive)
		{
			m_isPlayerShieldActive = false;
		}

		if (m_gameEndTimer <= 0.0f)
		{
			m_isMainMenu = true;
		}
		
		if (!m_isHyperSpace)
		{
			SpawnWave();
		}
		else
		{
			UpdateHyperSpace(deltaseconds);
		}

		UpdateEntities(deltaseconds);

		CheckCollisionBulletsVsEnemies();
		CheckCollisionShipVsEnemies();
		CheckCollisionShipVsBullets();
		CheckDebrisAliveTime();
		CheckBulletAliveTime();

		DeleteGarbageEntities();

		m_screenCamera->SetOrthoView(Vec2(0.0f, 0.0f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
		m_worldCamera->SetOrthoView(Vec2(0.0f, 0.0f), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));

		UpdateCameraShake(deltaseconds);
	}

	HandleInput();
	UpdateFromController(deltaseconds);
}

void Game::Render() const
{
	if (m_isAttractMode)
	{
		g_theRenderer->BeginCamera(*m_screenCamera, RootSig::DEFAULT_PIPELINE);

		RenderAttractMode();

		g_theRenderer->EndCamera(*m_screenCamera);
	}
	else if(m_isMainMenu)
	{
		g_theRenderer->BeginCamera(*m_screenCamera, RootSig::DEFAULT_PIPELINE);
		
		RenderMainMenu();

		g_theRenderer->EndCamera(*m_screenCamera);
	}
	else
	{
		g_theRenderer->BeginCamera(*m_worldCamera, RootSig::DEFAULT_PIPELINE);

		RenderEntities();

		g_theRenderer->EndCamera(*m_worldCamera);
	}
}

void Game::RenderEntities() const
{
	RenderStarMap();

	RenderEntityList(MAX_DEBRIS, (Entity const**)m_debris);
	RenderEntityList(MAX_ASTEROIDS, (Entity const**)m_asteroids);
	RenderEntityList(MAX_BOMBERS, (Entity const**)m_tieBombers);
	RenderEntityList(MAX_FIGHTERS, (Entity const**)m_tieFighters);

	if (m_playerShip)
	{
		if (m_playerShip->IsAlive())
		{
			m_playerShip->Render();
		}
	}

	RenderEntityList(MAX_BULLETS, (Entity const**)m_bullets);
}

void Game::RenderEntityList(int numList, Entity const** entity) const
{
	for (int index = 0; index < numList; index++)
	{
		if (entity[index])
		{
			if (entity[index]->IsAlive())
			{
				entity[index]->Render();
			}
		}
	}
}

void Game::RenderAttractMode() const
{
	std::vector<Vertex_PCU> textVerts;
	AddVertsForTextTriangles2D(textVerts, "A long time ago, in a Guildhall far", m_attractModePosition, 20.0f, Rgba8(21, 242, 253, m_attractModeAlpha));
	AddVertsForTextTriangles2D(textVerts, "far away...", m_attractModePosition - Vec2(0.0f, 20.0f), 20.0f, Rgba8(21, 242, 253, m_attractModeAlpha));
	
	AddVertsForTextTriangles2D(textVerts, "It is a period of civil war.", m_attractModePosition - Vec2(-50.0f, 520.0f), 20.0f, Rgba8(255, 255, 0, m_attractModeAlpha));
	AddVertsForTextTriangles2D(textVerts, "Rebel SDs, striking from a hidden perforce server,", m_attractModePosition - Vec2(100.0f, 550.0f), 20.0f, Rgba8(255, 255, 0, m_attractModeAlpha));
	AddVertsForTextTriangles2D(textVerts, "have won their first successful build,", m_attractModePosition - Vec2(20.0f, 580.0f), 20.0f, Rgba8(255, 255, 0, m_attractModeAlpha));
	AddVertsForTextTriangles2D(textVerts, "against the EVIL EMPEROR EISERLOH.", m_attractModePosition - Vec2(10.0f, 610.0f), 20.0f, Rgba8(255, 255, 0, m_attractModeAlpha));
	
	AddVertsForTextTriangles2D(textVerts, "During the famed battle of 23',", m_attractModePosition - Vec2(-50.0f, 710.0f), 20.0f, Rgba8(255, 255, 0, m_attractModeAlpha));
	AddVertsForTextTriangles2D(textVerts, "SDs managed to discover the Emperor's weakness", m_attractModePosition - Vec2(75.0f, 740.0f), 20.0f, Rgba8(255, 255, 0, m_attractModeAlpha));
	AddVertsForTextTriangles2D(textVerts, "LOOPS", m_attractModePosition - Vec2(-200.0f, 770.0f), 20.0f, Rgba8(255, 255, 0, m_attractModeAlpha));
	
	AddVertsForTextTriangles2D(textVerts, "Pursued by the Emperor's sinister game engine,", m_attractModePosition - Vec2(70.0f, 870.0f), 20.0f, Rgba8(255, 255, 0, m_attractModeAlpha));
	AddVertsForTextTriangles2D(textVerts, "SDs race home aboard their ", m_attractModePosition - Vec2(10.0f, 900.0f), 20.0f, Rgba8(255, 255, 0, m_attractModeAlpha));
	AddVertsForTextTriangles2D(textVerts, "STARSHIP", m_attractModePosition - Vec2(-350.0f, 900.0f), 20.0f, Rgba8(255, 215, 0, m_attractModeAlpha));

	g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE);
	g_theRenderer->BindTexture();
	g_theRenderer->BindShader();
	g_theRenderer->DrawVertexArray(static_cast<int>(textVerts.size()), textVerts.data());
}

void Game::RenderMainMenu() const
{
	//DrawDebugLine(Vec2(475.0f, 625.0f), Vec2(1140.0f, 625.0f), 20.0f, Rgba8(255, 215, 0, 255));
	//DrawDebugLine(Vec2(475.0f, 625.0f), Vec2(475.0f, 210.0f), 20.0f, Rgba8(255, 215, 0, 255));
	//DrawDebugLine(Vec2(1140.0f, 625.0f), Vec2(1140.0f, 210.0f), 20.0f, Rgba8(255, 215, 0, 255));
	//DrawDebugLine(Vec2(475.0f, 210.0f), Vec2(515.0f, 210.0f), 20.0f, Rgba8(255, 215, 0, 255));
	//DrawDebugLine(Vec2(1140.0f, 210.0f), Vec2(1090.0f, 210.0f), 20.0f, Rgba8(255, 215, 0, 255));
	
	std::vector<Vertex_PCU> textVerts;
	AddVertsForTextTriangles2D(textVerts, "CODE", m_mainMenuPosition, 200.0f, Rgba8(255, 215, 0, 255), 0.56f, false, 0.5f);
	AddVertsForTextTriangles2D(textVerts, "WARS", m_mainMenuPosition - Vec2(0.0f, 200.0f), 200.0f, Rgba8(255, 215, 0, 255), 0.56f, false, 0.5f);
	AddVertsForTextTriangles2D(textVerts, "THE PERFORCE AWAKENS", m_mainMenuPosition - Vec2(-35.0f, 210.0f), 32.5f, Rgba8(21, 242, 253, 255), 0.56f, false, 0.5f);
	
	AddVertsForTextTriangles2D(textVerts, "PRESS SPACEBAR OR A TO PLAY", m_mainMenuPosition - Vec2(60.0f, 310.0f), 32.5f, Rgba8(47, 249, 35, m_playButtonAlpha), 0.56f, false, 0.5f);
	AddVertsForTextTriangles2D(textVerts, "PRESS ESC OR B TO QUIT", m_mainMenuPosition - Vec2(490.0f, -370.0f), 20.0f, Rgba8(255, 0, 0, 255));
	
	g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE);
	g_theRenderer->BindTexture();
	g_theRenderer->BindShader();
	g_theRenderer->DrawVertexArray(static_cast<int>(textVerts.size()), textVerts.data());
}

void Game::RenderStarMap() const
{
	for (int index = 0; index < MAX_STARS; index++)
	{
		m_starMap[index]->Render();
	}
}

void Game::HandleInput()
{
	if (m_isAttractMode)
	{
		if (g_theInputSystem->WasKeyJustPressed(KEYCODE_ESC))
		{
			g_theApp->HandleQuitRequested();
		}
		if (g_theInputSystem->WasKeyJustPressed(KEYCODE_SPACE))
		{
			m_attractModeTimer = 46.0f;
		}
	}
	else if (m_isMainMenu)
	{
		if (g_theInputSystem->WasKeyJustPressed(KEYCODE_SPACE))
		{
			m_isMainMenu = false;
		}

		if (g_theInputSystem->WasKeyJustPressed(KEYCODE_ESC))
		{
			g_theApp->HandleQuitRequested();
		}
	}
	else
	{
		if (g_theInputSystem->WasKeyJustPressed(KEYCODE_ESC))
		{
			m_isMainMenu = true;
		}

		if (g_theInputSystem->WasKeyJustPressed('I'))
		{
			SpawnAsteroid(1);
		}

		if (g_theInputSystem->WasKeyJustPressed(KEYCODE_F1))
		{
			m_developerMode = !m_developerMode;
		}

		if (g_theInputSystem->WasKeyJustPressed('N'))
		{
			delete m_playerShip;
			m_playerShip = new PlayerShip(this, Vec2(WORLD_CENTER_X, WORLD_CENTER_Y));
		}

		if (m_numOfBullets == MAX_BULLETS)
			ERROR_RECOVERABLE("MAX LIMIT OF BULLETS REACHED!!!");

		if (m_numOfAsteroids == MAX_ASTEROIDS)
			ERROR_RECOVERABLE("MAX LIMIT OF ASTEROIDS REACHED!!!");
	}
}

void Game::CheckDebrisAliveTime()
{
	for (int index = 0; index < MAX_DEBRIS; index++)
	{
		if (m_debris[index])
		{
			if (m_debris[index]->GetAliveTime() > 1.0f)
			{
				m_debris[index]->Die();
			}
		}
	}
}


void Game::CheckBulletAliveTime()
{
	for (int index = 0; index < MAX_BULLETS; index++)
	{
		if (m_bullets[index])
		{
			if (m_bullets[index]->GetAliveTime() > 2.0f)
			{
				m_bullets[index]->Die();
			}
		}
	}
}

void Game::UpdateCameraShake(float deltaseconds)
{
	if (m_isCameraShake)
	{
		m_cameraShakeTimer += deltaseconds;
	}

	if (m_isCameraShake && m_cameraShakeTimer < 2.0f)
	{
		m_cameraShakeRate -= CAMERA_SHAKE_RATE * deltaseconds;

		RandomNumberGenerator random = RandomNumberGenerator();
		m_worldCameraOffsetX = random.RollRandomFloatInRange(-m_cameraShakeRate * deltaseconds, m_cameraShakeRate * deltaseconds);
		m_worldCameraOffsetY = random.RollRandomFloatInRange(-m_cameraShakeRate * deltaseconds, m_cameraShakeRate * deltaseconds);

		m_worldCamera->Translate2D(Vec2(m_worldCameraOffsetX, m_worldCameraOffsetY));
	}
	else
	{
		m_isCameraShake = false;
		m_cameraShakeTimer = 0.0f;
		m_cameraShakeRate = 40.0f;
	}
}

void Game::UpdateHyperSpace(float deltaseconds)
{
	m_hyperSpaceTimer += deltaseconds;

	if (m_hyperSpaceTimer > 6.0f)
	{
		m_waveMode += 2;
		m_hyperSpaceTimer = 0.0f;
		m_isHyperSpace = false;
	}
}

void Game::GenerateStarMap()
{
	for (int index = 0; index < MAX_STARS; index++)
	{
		RandomNumberGenerator random = RandomNumberGenerator();

		Rgba8 color;
		float parallaxEffect = 0.0f;

		if (index % 2 == 0)
		{
			color.r = 255;
			color.g = 255;
			color.b = 255;
			color.a = 127;
			parallaxEffect = 0.05f;
		}
		else if (index % 3 == 0)
		{
			color.r = 93;
			color.g = 250;
			color.b = 251;
			color.a = 250;
			parallaxEffect = 0.1f;
		}
		else
		{
			color.r = 200;
			color.g = 190;
			color.b = 100;
			color.a = 100;
			parallaxEffect = 0.01f;
		}

		m_starMap[index] = new Star(this, Vec2(random.RollRandomFloatInRange(0.0f, WORLD_SIZE_X), random.RollRandomFloatInRange(0.0f, WORLD_SIZE_Y)), color, parallaxEffect);
	}
}

void Game::UpdateEntities(float deltaseconds)
{
	if (m_playerShip)
	{
		if (m_playerShip->IsAlive())
		{
			m_playerShip->Update(deltaseconds);
		}
	}
	
	UpdateStarMap(deltaseconds);

	UpdateEntityList(MAX_FIGHTERS, m_tieFighters, deltaseconds);
	UpdateEntityList(MAX_BOMBERS, m_tieBombers, deltaseconds);
	UpdateEntityList(MAX_ASTEROIDS, m_asteroids, deltaseconds);
	UpdateEntityList(MAX_DEBRIS, m_debris, deltaseconds);
	UpdateEntityList(MAX_BULLETS, m_bullets, deltaseconds);
}

void Game::UpdateEntityList(int numList, Entity** entity, float deltaseconds)
{
	for (int index = 0; index < numList; index++)
	{
		if (entity[index])
		    if (entity[index]->IsAlive())
				entity[index]->Update(deltaseconds);
	}
}

void Game::UpdateStarMap(float deltaseconds)
{
	for (int index = 0; index < MAX_STARS; index++)
	{
		m_starMap[index]->Update(deltaseconds);
	}
}

void Game::UpdateAttractMode(float deltaseconds)
{
	UNUSED(deltaseconds);

	m_attractModeTimer += deltaseconds;

	if (m_attractModeAlpha < 255)
	{
		m_attractModeAlpha++;
	}

	if (m_attractModeAlpha == 255)
	{
		m_attractModePosition += m_attractModeOffset * deltaseconds;
	}

	if (m_attractModeTimer > 44.5f)
	{
		m_isAttractMode = false;
		g_theAudio->StopSound(m_attractModePlayback);
		m_attractModeTimer = 0.0f;

		m_isMainMenu = true;
		m_mainMenuMusic = g_theAudio->CreateOrGetSound("Data/Audio/MainTheme.mp3");
		m_mainMenuPlayback = g_theAudio->StartSound(m_mainMenuMusic, true);
	}
}

void Game::UpdateMainMenu(float deltaseconds)
{
	static float alphaOffset = 0.0f;

	alphaOffset += 100.0f * deltaseconds;

	UNUSED(deltaseconds);

	m_playButtonAlpha = 50 + unsigned char(200 * fabsf(SinDegrees(alphaOffset)));
}

void Game::UpdateFromController(float deltaseconds)
{
	UNUSED(deltaseconds);

	XboxController const& controller = g_theInputSystem->GetController(0);

	if (m_isAttractMode)
	{
		if (controller.WasButtonJustPressed(XboxButtonID::BUTTON_START))
		{
			m_attractModeTimer = 46.0f;
		}

		if (controller.WasButtonJustPressed(XboxButtonID::BUTTON_B))
		{
			g_theApp->HandleQuitRequested();
		}
	}
	else if(m_isMainMenu)
	{
		if (controller.WasButtonJustPressed(XboxButtonID::BUTTON_START))
		{
			m_isMainMenu = false;
		}

		if (controller.WasButtonJustPressed(XboxButtonID::BUTTON_B))
		{
			g_theApp->HandleQuitRequested();
		}
	}
}

void Game::CheckCollisionBulletsVsEntity(int numList, Entity** entity)
{

	for (int i = 0; i < MAX_BULLETS; i++)
	{
		for (int j = 0; j < numList; j++)
		{
			if (entity[j] && m_bullets[i])
			{
				if (m_bullets[i]->m_color.r == 255 && m_bullets[i]->m_color.b == 0)
				{
					if (DoDiscsOverlap(entity[j]->GetPosition(), entity[j]->m_physicsRadius, m_bullets[i]->GetPosition(), BULLET_PHYSICS_RADIUS))
					{
						if (m_bullets[i]->IsAlive() && entity[j]->IsAlive())
						{
							SpawnDebrisOnBulletCollision(m_bullets[i], entity[j]);
						}
					}
				}

				if (m_bullets[i]->m_color.g == 255 && entity[j] == m_asteroids[j])
				{
					if (DoDiscsOverlap(entity[j]->GetPosition(), entity[j]->m_physicsRadius, m_bullets[i]->GetPosition(), BULLET_PHYSICS_RADIUS))
					{
						if (m_bullets[i]->IsAlive() && entity[j]->IsAlive())
						{
							SpawnDebrisOnBulletCollision(m_bullets[i], entity[j]);
						}
					}
				}
			}
		}
	}
}

void Game::CheckCollisionShipVsEntity(int numList, Entity** entity)
{
	for (int i = 0; i < numList; i++)
	{
		if (entity[i] && m_playerShip->IsAlive())
		{
			if (DoDiscsOverlap(entity[i]->GetPosition(), entity[i]->m_physicsRadius, m_playerShip->GetPosition(), PLAYER_SHIP_PHYSICS_RADIUS))
			{
				m_isPlayerShieldActive = true;
				SpawnDebrisOnShipCollision(entity[i]);
			}
		}
	}
}

void Game::CheckCollisionShipVsBullets()
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (m_playerShip && m_bullets[i])
		{
			if (m_bullets[i]->m_color.g == 255)
			{
				if (DoDiscsOverlap(m_playerShip->GetPosition(), m_playerShip->m_physicsRadius, m_bullets[i]->GetPosition(), BULLET_PHYSICS_RADIUS))
				{
					if (m_bullets[i]->IsAlive() && m_playerShip->IsAlive())
					{
						SpawnDebrisOnBulletCollision(m_bullets[i], m_playerShip);
					}
				}
			}
		}
	}
}

void Game::CheckCollisionBulletsVsEnemies()
{
	CheckCollisionBulletsVsEntity(MAX_ASTEROIDS, m_asteroids);
	CheckCollisionBulletsVsEntity(MAX_FIGHTERS, m_tieFighters);
	CheckCollisionBulletsVsEntity(MAX_BOMBERS, m_tieBombers);
}

void Game::CheckCollisionShipVsEnemies()
{
	CheckCollisionShipVsEntity(MAX_ASTEROIDS,	m_asteroids);
	CheckCollisionShipVsEntity(MAX_FIGHTERS, m_tieFighters);
	CheckCollisionShipVsEntity(MAX_BOMBERS, m_tieBombers);
}

void Game::SpawnWave()
{
	if (m_numOfAsteroids + m_numOfBombers + m_numOfFighters == 0)
	{
		static int tempWaveMode = 0;

		if (m_waveMode < tempWaveMode)
		{
			m_isHyperSpace = true;

			SoundID hyperSpeed = g_theAudio->CreateOrGetSound("Data/Audio/HyperSpeed.mp3");
			g_theAudio->StartSound(hyperSpeed);

			SoundID falcon = g_theAudio->CreateOrGetSound("Data/Audio/Falcon.mp3");
			g_theAudio->StartSound(falcon, false, 0.5f);

			return;
		}

		if (m_waveMode == 1)
		{
			SpawnFighters(2);
			SpawnBombers(1);
			SpawnAsteroid(2);			
			tempWaveMode = m_waveMode;
			m_waveMode -= 1;
			return;
		}
		
		if (m_waveMode == 2)
		{
			SpawnFighters(3);
			SpawnBombers(2);
			SpawnAsteroid(5);
			tempWaveMode = m_waveMode;
			m_waveMode -= 1;
			return;
		}
		
		if (m_waveMode == 3)
		{
			SpawnFighters(4);
			SpawnBombers(3);
			SpawnAsteroid(6);
			tempWaveMode = m_waveMode;
			m_waveMode -= 1;
			return;
		}
		
		if (m_waveMode == 4)
		{
			SpawnFighters(4);
			SpawnBombers(5);
			SpawnAsteroid(6);
			tempWaveMode = m_waveMode;
			m_waveMode -= 1;
			return;
		}
		
		if (m_waveMode == 5)
		{
			SpawnFighters(6);
			SpawnBombers(6);
			SpawnAsteroid(7);
			tempWaveMode = m_waveMode;
			m_waveMode -= 1;
			return;
		}
		
		if (m_waveMode == 6 && m_gameEndTimer <= 0.0f)
		{
			m_isMainMenu = true;
			return;
		}
	}
}

void Game::SpawnBullet(Vec2 const& position, float const& orientation, Rgba8 const& color)
{
	for (int index = 0; index < MAX_BULLETS; index++)
	{
		if (!m_bullets[index])
		{
			m_bullets[index] = new Bullet(this, position, orientation, color);
			m_numOfBullets++;
			break;
		}
	}
}

void Game::SpawnAsteroid(int numOfAsteroids)
{
	for (int index = 0; index < numOfAsteroids; index++)
	{
		Vec2 position;

		RandomNumberGenerator random = RandomNumberGenerator();

		int edge = random.RollRandomIntInRange(0, 3);

		if (edge == 0)
		{
			position = Vec2(-ASTEROID_COSMETIC_RADIUS, random.RollRandomFloatInRange(0.0f, WORLD_SIZE_Y));
		}
		else if (edge == 1)
		{
			position = Vec2(random.RollRandomFloatInRange(0.0f, WORLD_SIZE_X), WORLD_SIZE_Y + ASTEROID_COSMETIC_RADIUS);
		}
		else if (edge == 2)
		{
			position = Vec2(WORLD_SIZE_X + ASTEROID_COSMETIC_RADIUS, random.RollRandomFloatInRange(0.0f, WORLD_SIZE_Y));
		}
		else if (edge == 3)
		{
			position = Vec2(random.RollRandomFloatInRange(0.0f, WORLD_SIZE_X), -ASTEROID_COSMETIC_RADIUS);
		}

		if (!m_asteroids[index + m_numOfAsteroids])
		{
			m_asteroids[index + m_numOfAsteroids] = new Asteroid(this, position);
		}
	}

	m_numOfAsteroids += numOfAsteroids;
}

void Game::SpawnFighters(int numOfFigthers)
{
	for (int index = 0; index < numOfFigthers; index++)
	{
		Vec2 position;

		RandomNumberGenerator random = RandomNumberGenerator();

		int edge = random.RollRandomIntInRange(0, 3);

		if (edge == 0)
		{
			position = Vec2(-TIE_FIGHTER_COSMETIC_RADIUS, random.RollRandomFloatInRange(0.0f, WORLD_SIZE_Y));
		}
		else if (edge == 1)
		{
			position = Vec2(random.RollRandomFloatInRange(0.0f, WORLD_SIZE_X), WORLD_SIZE_Y + TIE_FIGHTER_COSMETIC_RADIUS);
		}
		else if (edge == 2)
		{
			position = Vec2(WORLD_SIZE_X + TIE_FIGHTER_COSMETIC_RADIUS, random.RollRandomFloatInRange(0.0f, WORLD_SIZE_Y));
		}
		else if (edge == 3)
		{
			position = Vec2(random.RollRandomFloatInRange(0.0f, WORLD_SIZE_X), -TIE_FIGHTER_COSMETIC_RADIUS);
		}

		if (!m_tieFighters[index + m_numOfFighters])
		{
			m_tieFighters[index + m_numOfFighters] = new TieFighter(this, position);
		}
	}

	m_numOfFighters += numOfFigthers;
}

bool Game::IsHyperSpace() const
{
	return m_isHyperSpace;
}

void Game::SpawnBombers(int numOfBombers)
{
	for (int index = 0; index < numOfBombers; index++)
	{
		Vec2 position;

		RandomNumberGenerator random = RandomNumberGenerator();

		int edge = random.RollRandomIntInRange(0, 3);

		if (edge == 0)
		{
			position = Vec2(-TIE_BOMBER_COSMETIC_RADIUS, random.RollRandomFloatInRange(0.0f, WORLD_SIZE_Y));
		}
		else if (edge == 1)
		{
			position = Vec2(random.RollRandomFloatInRange(0.0f, WORLD_SIZE_X), WORLD_SIZE_Y + TIE_BOMBER_COSMETIC_RADIUS);
		}
		else if (edge == 2)
		{
			position = Vec2(WORLD_SIZE_X + TIE_BOMBER_COSMETIC_RADIUS, random.RollRandomFloatInRange(0.0f, WORLD_SIZE_Y));
		}
		else if (edge == 3)
		{
			position = Vec2(random.RollRandomFloatInRange(0.0f, WORLD_SIZE_X), -TIE_BOMBER_COSMETIC_RADIUS);
		}

		if (!m_tieBombers[index + m_numOfBombers])
		{
			m_tieBombers[index + m_numOfBombers] = new TieBomber(this, position);
		}
	}

	m_numOfBombers += numOfBombers;
}

void Game::SpawnDebrisOnShipCollision(Entity* other)
{
	m_playerShip->m_health--;
	m_isCameraShake = true;

	if (m_playerShip->m_health == 0)
	{
		m_playerShip->Die();
		m_numOfPlayerLives--;
	}

	Vec2 normal = m_playerShip->GetPosition() - other->GetPosition();
	RandomNumberGenerator random = RandomNumberGenerator();

	if (other->m_health > 1)
	{
		SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Asteroid_Explosion.wav");
		g_theAudio->StartSound(testSound);

		other->m_health--;
		SpawnDebris(random.RollRandomIntInRange(1, 3), Vec2((other->GetPosition().x + (other->m_cosmeticRadius * CosDegrees(normal.GetOrientationDegrees()))), (other->GetPosition().y + (other->m_cosmeticRadius * SinDegrees(normal.GetOrientationDegrees())))), Vec2(0.0f, 0.0f), random.RollRandomFloatInRange(0.2f, 0.8f), other->m_color);
		return;
	}
	else
	{
		other->Die();
		m_isCameraShake = true;
	}
}

void Game::SpawnDebrisOnBulletCollision(Entity* bullet, Entity* other)
{
	bullet->Die();
	Vec2 normal = bullet->GetPosition() - other->GetPosition();
	RandomNumberGenerator random = RandomNumberGenerator();

	if (other->m_health > 1)
	{
		SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Hit.wav");
		g_theAudio->StartSound(testSound);

		other->m_health--;
		SpawnDebris(random.RollRandomIntInRange(1, 3), Vec2((other->GetPosition().x + (other->m_cosmeticRadius * CosDegrees(normal.GetOrientationDegrees()))),
			(other->GetPosition().y + (other->m_cosmeticRadius * SinDegrees(normal.GetOrientationDegrees())))), Vec2(0.0f, 0.0f), 0.2f, bullet->m_color);
	}
	else
	{
		other->Die();
		m_isCameraShake = true;
	}
}

void Game::SpawnDebris(int numOfDebris, Vec2 const& position, Vec2 const& velocity, float const& scale, Rgba8 const& color)
{
	for (int index = 0; index < numOfDebris; index++)
	{
		if (!m_debris[index + m_numOfDebris])
		{
			m_debris[index + m_numOfDebris] = new Debris(this, position, velocity, scale, color);
		}
	}
	m_numOfDebris += numOfDebris;
}

void Game::DeleteGarbageEntityList(int numList, Entity** entity, int& currentTotal)
{
	for (int index = 0; index < numList; index++)
	{
		if (entity[index])
		{
			if (entity[index]->IsGarbage())
			{
				DELETE_PTR(entity[index])
					currentTotal--;
			}
		}
	}
}

void Game::DeleteGarbageEntities()
{
	DeleteGarbageEntityList(MAX_ASTEROIDS,	m_asteroids,	m_numOfAsteroids);
	DeleteGarbageEntityList(MAX_FIGHTERS,	m_tieFighters,	m_numOfFighters);
	DeleteGarbageEntityList(MAX_BOMBERS,	m_tieBombers,	m_numOfBombers);
	DeleteGarbageEntityList(MAX_BULLETS,	m_bullets,		m_numOfBullets);
	DeleteGarbageEntityList(MAX_DEBRIS,		m_debris,		m_numOfDebris);
}

bool Game::IsDeveloperModeOn() const
{
	return m_developerMode;
}

Entity* Game::GetShip() const
{
	return m_playerShip;
}