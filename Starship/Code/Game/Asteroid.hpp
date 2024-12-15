#pragma once

#include "Game/Entity.hpp"

#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

constexpr int NUM_ASTEROID_TRIANGLES = 16;
constexpr int NUM_ASTEROID_VERTS = 3 * NUM_ASTEROID_TRIANGLES;

class VertexBuffer;

class Asteroid : public Entity
{
	Vertex_PCU					m_vertices[NUM_ASTEROID_VERTS] = {};
	RandomNumberGenerator		m_Rand;
	VertexBuffer*				m_gpuMesh = nullptr;
public:
								Asteroid(Game* owner, Vec2 const& startPos);
								~Asteroid();

	void						Initialize();

	virtual void				Update(float deltaseconds) override;
	virtual void				Render() const override;
	virtual void				Die() override;
};