#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

class Game;

class Entity
{
public:
	Vec2			m_position;
	Vec2			m_velocity;
	float			m_orientationDegrees	= 0.0f;
	float			m_scale					= 1.0f;
	float			m_angularVelocity		= 0.0f;
	float			m_physicsRadius			= 1.0f;
	float			m_cosmeticRadius		= 2.0f;
	int				m_health				= 3;
	Rgba8			m_color					= Rgba8(0, 0, 0, 255);
	bool			m_isDead				= false;
	bool			m_isGarbage				= false;
	Game*			m_game					= nullptr;
	float			m_aliveTime				= 0.0f;
public:
					Entity() {}
					Entity(Game* owner, Vec2 startPos);
	virtual			~Entity();

	virtual void	Update(float deltaseconds) = 0;
	virtual void	Render() const = 0;
	virtual void	Die();

	bool			IsOffScreen();
	bool			IsAlive() const;
	bool			IsGarbage() const;
	Vec2			GetPosition() const;
	Vec2			GetVelocity() const;
	float			GetOrientation() const;
	Vec2			GetForwardNormal() const;
	float			GetAliveTime() const;
};