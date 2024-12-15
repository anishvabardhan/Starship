#pragma once

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"

#include "Game/App.hpp"

#define UNUSED(x) (void)x
#define DELETE_PTR(x) if(x) { delete x; x = nullptr; }

extern Renderer* g_theRenderer;
extern InputSystem* g_theInputSystem;
extern AudioSystem* g_theAudio;
extern App* g_theApp;

struct Vec2;
struct Rgba8;

constexpr int			MAX_ASTEROIDS						= 30;
constexpr int			MAX_BULLETS							= 200;
constexpr int			MAX_DEBRIS							= 1000;
constexpr int			MAX_FIGHTERS						= 50;
constexpr int			MAX_BOMBERS							= 50;
constexpr int			MAX_STARS							= 500;
constexpr float			SCREEN_SIZE_X						= 1600.0f;
constexpr float			SCREEN_SIZE_Y						= 800.0f;
constexpr float			WORLD_SIZE_X						= 300.0f;
constexpr float			WORLD_SIZE_Y						= 150.0f;
constexpr float			WORLD_CENTER_X						= WORLD_SIZE_X / 2.0f;
constexpr float			WORLD_CENTER_Y						= WORLD_SIZE_Y / 2.0f;
constexpr float			DEBRIS_SPEED						= 15.0f;
constexpr float			DEBRIS_PHYSICS_RADIUS				= 0.1f;
constexpr float			DEBRIS_COSMETIC_RADIUS				= 2.0f;
constexpr float			CAMERA_SHAKE_RATE					= 15.0f;
constexpr float			ASTEROID_SPEED						= 10.0f;
constexpr float			ASTEROID_PHYSICS_RADIUS				= 5.6f;
constexpr float			ASTEROID_COSMETIC_RADIUS			= 5.0f;
constexpr float			BULLET_LIFETIME_SECONDS				= 2.0f;
constexpr float			BULLET_SPEED						= 120.0f;
constexpr float			BULLET_PHYSICS_RADIUS				= 0.75f;
constexpr float			BULLET_COSMETIC_RADIUS				= 1.0f;
constexpr float			STAR_SPEED							= 10.0f;
constexpr float			STAR_PHYSICS_RADIUS					= 0.5f;
constexpr float			STAR_COSMETIC_RADIUS				= 0.5f;
constexpr float			TIE_FIGHTER_SPEED					= 10.0f;
constexpr float			TIE_FIGHTER_PHYSICS_RADIUS			= 3.0f;
constexpr float			TIE_FIGHTER_COSMETIC_RADIUS			= 4.5f;
constexpr float			TIE_BOMBER_ACCELERATION				= 5.0f;
constexpr float			TIE_BOMBER_PHYSICS_RADIUS			= 2.5f;
constexpr float			TIE_BOMBER_COSMETIC_RADIUS			= 4.0f;
constexpr float			PLAYER_SHIP_ACCELERATION			= 50.0f;
constexpr float			PLAYER_SHIP_TURN_SPEED				= 300.0f;
constexpr float			PLAYER_SHIP_PHYSICS_RADIUS			= 3.5f;
constexpr float			PLAYER_SHIP_COSMETIC_RADIUS			= 4.25f;

void					DrawDebugRing(Vec2 const& center, float const& radius, float const& orientation, float const& thickness, Rgba8 const& color);
void					DrawDebugLine(Vec2 const& startPos, Vec2 const& endPos, float const& thickness, Rgba8 const& color);