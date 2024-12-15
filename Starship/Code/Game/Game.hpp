#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"

#include "Game/GameCommon.hpp"

class Entity;

class Game
{
public:
	Entity*				m_playerShip					= nullptr;
	Entity*				m_asteroids[MAX_ASTEROIDS]		= { nullptr };
	Entity*				m_tieFighters[MAX_FIGHTERS]		= { nullptr };
	Entity*				m_tieBombers[MAX_BOMBERS]		= { nullptr };
	Entity*				m_bullets[MAX_BULLETS]			= { nullptr };
	Entity*				m_debris[MAX_DEBRIS]			= { nullptr };
	Entity*				m_starMap[MAX_STARS]			= {	nullptr	};
	Camera*				m_worldCamera					= nullptr;
	Camera*				m_screenCamera					= nullptr;

	Vec2				m_attractModeOffset				= Vec2(0.0f, 34.0f);
	Vec2				m_attractModePosition			= Vec2(550.0f, 400.0f);
	Vec2				m_mainMenuPosition				= Vec2(500.0f, 400.0f);

	SoundID				m_attractModeMusic;
	SoundPlaybackID		m_attractModePlayback;

	SoundID				m_mainMenuMusic;
	SoundPlaybackID		m_mainMenuPlayback;

	unsigned char		m_attractModeAlpha				= 0;
	unsigned char		m_playButtonAlpha				= 0;
	int					m_playerShield					= 10;
	int					m_waveMode						= 1;
	float				m_attractModeTimer				= 0.0f;
	float				m_gameEndTimer					= 2.0f;
	float				m_worldCameraOffsetX			= 0.0f;
	float				m_worldCameraOffsetY			= 0.0f;
	int					m_numOfBullets					= 0;
	int					m_numOfPlayerLives				= 0;
	int					m_numOfAsteroids				= 0;
	int					m_numOfFighters					= 0;
	int					m_numOfBombers					= 0;
	int					m_numOfDebris					= 0;
	bool				m_developerMode					= false;
	bool				m_isAttractMode					= true;
	bool				m_isMainMenu					= false;
	bool				m_isHyperSpace					= false;
	bool				m_isCameraShake					= false;
	bool				m_isPlayerShieldActive			= false;
	float				m_cameraShakeTimer				= 0.0f;
	float				m_hyperSpaceTimer				= 0.0f;
	float				m_cameraShakeRate				= 40.0f;
public:
						Game();
						~Game();

	void				StartUp();
	void				Shutdown();

	void				Update(float deltaseconds);
	void				Render() const ;

	void				HandleInput();
	void				CheckCollisionBulletsVsEnemies();
	void				CheckCollisionShipVsEnemies();
	void				CheckCollisionBulletsVsEntity(int numList, Entity** entity);
	void				CheckCollisionShipVsEntity(int numList, Entity** entity);
	void				CheckCollisionShipVsBullets();
	void				CheckBulletAliveTime();
	void				CheckDebrisAliveTime();
	void				UpdateCameraShake(float deltaseconds);
	void                UpdateHyperSpace(float deltaseconds);
	void				GenerateStarMap();
	void				UpdateFromController(float deltaseconds);
	void				UpdateEntities(float deltaseconds);
	void				UpdateEntityList(int numList, Entity** entity, float deltaseconds);
	void				UpdateStarMap(float deltaseconds);
	void				UpdateAttractMode(float deltaseconds);
	void				UpdateMainMenu(float deltaseconds);

	void				RenderEntities() const;
	void				RenderEntityList(int numList, Entity const** entity) const;
	void				RenderStarMap() const;
	void				RenderAttractMode() const;
	void				RenderMainMenu() const;

	void				SpawnWave();
	void				SpawnBullet(Vec2 const& position, float const& orientation, Rgba8 const& color);
	void				SpawnAsteroid(int numOfAsteroids);
	void				SpawnFighters(int numOfFigthers);
	void				SpawnBombers(int numOfBombers);
	void				SpawnDebris(int numOfDebris, Vec2 const& position, Vec2 const& velocity, float const& scale, Rgba8 const& color);
	void				SpawnDebrisOnShipCollision(Entity* other);
	void				SpawnDebrisOnBulletCollision(Entity* bullet, Entity* other);

	void				DeleteGarbageEntities();
	void				DeleteGarbageEntityList(int numList, Entity** entity, int& currentTotal);

	bool				IsHyperSpace() const;
	bool				IsDeveloperModeOn() const;

	Entity*				GetShip() const;
};