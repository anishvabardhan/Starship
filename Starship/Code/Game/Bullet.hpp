#pragma once

#include "Game/Entity.hpp"

#include "Engine/Core/Vertex_PCU.hpp"

constexpr int NUM_OF_BULLET_TRIANGLES = 2;
constexpr int NUM_OF_BULLET_VERTICES = 3 * NUM_OF_BULLET_TRIANGLES;

constexpr int NUM_OF_GLOW_TRIANGLES = 20;
constexpr int NUM_OF_GLOW_VERTICES = 3 * NUM_OF_GLOW_TRIANGLES;

class VertexBuffer;

class Bullet : public Entity
{
	Vertex_PCU		m_vertices[NUM_OF_BULLET_VERTICES] = {};
	Vertex_PCU		m_glowVertices[NUM_OF_GLOW_VERTICES] = {};
	VertexBuffer*	m_gpuMesh = nullptr;
public:
					Bullet(Game* owner, Vec2 const& startPos, float const& orientation, Rgba8 color);
					~Bullet();

	void			Initialize();
	void			RenderGlow() const;

	virtual void	Update(float deltaseconds) override;
	virtual void	Render() const override;
	virtual void	Die() override;
};