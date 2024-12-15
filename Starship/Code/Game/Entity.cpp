#include "Game/Entity.hpp"

#include "Game/GameCommon.hpp"

#include <math.h>

Entity::Entity(Game* owner, Vec2 startPos)
	: m_game(owner), m_position(startPos)
{
}

Entity::~Entity()
{
}

void Entity::Update(float deltaseconds)
{
	UNUSED(deltaseconds);
}

void Entity::Render() const
{
}

void Entity::Die()
{
}

bool Entity::IsOffScreen()
{
	if((m_position.x >= WORLD_SIZE_X + m_cosmeticRadius) || (m_position.x <= -m_cosmeticRadius))
		return true;

	if ((m_position.y >= WORLD_SIZE_Y + m_cosmeticRadius) || (m_position.y <= -m_cosmeticRadius))
		return true;

	return false;
}

bool Entity::IsAlive() const
{
	return !m_isDead;
}

bool Entity::IsGarbage() const
{
	return m_isGarbage;
}

Vec2 Entity::GetPosition() const
{
	return m_position;
}

Vec2 Entity::GetVelocity() const
{
	return m_velocity;
}

float Entity::GetOrientation() const
{
	return m_orientationDegrees;
}

Vec2 Entity::GetForwardNormal() const
{
	Vec2 radians;

	radians.x = m_orientationDegrees * (3.14f / 180.0f);
	radians.y = m_orientationDegrees * (3.14f / 180.0f);

	return Vec2(cosf(radians.x), sinf(radians.y));
}

float Entity::GetAliveTime() const
{
	return m_aliveTime;
}
