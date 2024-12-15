#pragma once

#include "Game/Entity.hpp"

#include "Engine/Core/Vertex_PCU.hpp"

constexpr int NUM_OF_STAR_TRIANGLES = 10;
constexpr int NUM_OF_STAR_VERTICES = 3 * NUM_OF_STAR_TRIANGLES;

class VertexBuffer;

class Star : public Entity
{
	Vertex_PCU		m_vertices[NUM_OF_STAR_VERTICES]	= {};
	VertexBuffer*	m_gpuMesh							= nullptr;
	VertexBuffer*	m_gpuHyperSpaceMesh					= nullptr;
	float			m_distanceFromScreen				= 0.0f;
	float			m_spaceStretch						= 0.0f;
	float			m_stretchSpeed						= 100.0f;
public:
	Star(Game* owner, Vec2 const& startPos, Rgba8 const& color, float const& distanceFromPlayer);
	~Star();

	void			Initialize();

	virtual void	Update(float deltaseconds) override;
	virtual void	Render() const override;
	virtual void	Die() override;
};