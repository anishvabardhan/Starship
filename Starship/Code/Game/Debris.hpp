#pragma once

#include "Game/Entity.hpp"

#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

constexpr int NUM_DEBRIS_TRIANGLES = 5;
constexpr int NUM_DEBRIS_VERTS = 3 * NUM_DEBRIS_TRIANGLES;

class VertexBuffer;

class Debris : public Entity
{
	Vertex_PCU			m_vertices[NUM_DEBRIS_VERTS] = {};
	VertexBuffer*		m_gpuMesh = nullptr;
	float				m_scale = 1.0f;
	float				m_speed = 0.0f;
	unsigned char		m_age = 4;
public:
						Debris() = default;
						Debris(Game* owner, Vec2 const& startPos, Vec2 const& velocity, float scale, Rgba8 const& color);
						~Debris();

	void				Initialize();

	virtual void		Update(float deltaseconds) override;
	virtual void		Render() const override;
	virtual void		Die() override;
};