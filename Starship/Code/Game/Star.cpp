#include "Game/Star.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"

#include <vector>

Star::Star(Game* owner, Vec2 const& startPos, Rgba8 const& color, float const& distanceFromPlayer)
	: Entity(owner, startPos)
{
	m_physicsRadius = STAR_PHYSICS_RADIUS;
	m_cosmeticRadius = STAR_COSMETIC_RADIUS;
	m_color = color;
	m_distanceFromScreen = distanceFromPlayer;

	m_gpuMesh = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU) * NUM_OF_STAR_VERTICES);
	m_gpuHyperSpaceMesh = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU) * 4);

	Initialize();
}

Star::~Star()
{
	DELETE_PTR(m_gpuMesh);
	DELETE_PTR(m_gpuHyperSpaceMesh);
}

void Star::Initialize()
{
	float thetaDegrees = 360.0f / (float)NUM_OF_STAR_TRIANGLES;

	for (int index = 0; index < NUM_OF_STAR_TRIANGLES; index++)
	{
		float radius = 1.0f;

		float theta1 = index * thetaDegrees;
		float theta2 = (index + 1) * thetaDegrees;

		Vec2 vert1 = Vec2(radius * CosDegrees(theta1), radius * SinDegrees(theta1));
		Vec2 vert2 = Vec2(radius * CosDegrees(theta2), radius * SinDegrees(theta2));

		m_vertices[3 * index] = Vertex_PCU(0.0f, 0.0f, m_color.r, m_color.g, m_color.b, m_color.a);
		m_vertices[3 * index + 1] = Vertex_PCU(vert1.x, vert1.y, m_color.r, m_color.g, m_color.b, m_color.a);
		m_vertices[3 * index + 2] = Vertex_PCU(vert2.x, vert2.y, m_color.r, m_color.g, m_color.b, m_color.a);
	}
}

void Star::Update(float deltaseconds)
{

	if (m_position.x > WORLD_SIZE_X + m_cosmeticRadius)
		m_position.x = -m_cosmeticRadius;

	if (m_position.x < -m_cosmeticRadius)
		m_position.x = WORLD_SIZE_X + m_cosmeticRadius;

	if (m_position.y > WORLD_SIZE_Y + m_cosmeticRadius)
		m_position.y = -m_cosmeticRadius;

	if (m_position.y < -m_cosmeticRadius)
		m_position.y = WORLD_SIZE_Y + m_cosmeticRadius;

	if (m_game->m_isHyperSpace)
	{
		if (m_game->m_hyperSpaceTimer > 2.0f && m_game->m_hyperSpaceTimer <= 4.0f)
		{
			m_spaceStretch += 20.0f * deltaseconds;
			m_stretchSpeed = 100.0f;
		}
		else if (m_game->m_hyperSpaceTimer > 4.0f && m_game->m_hyperSpaceTimer <= 6.0f)
		{
			m_spaceStretch -= 20.0f * deltaseconds;
			m_stretchSpeed = 100.0f;
		}
		else if(m_game->m_hyperSpaceTimer > 0.0f && m_game->m_hyperSpaceTimer <= 2.0f)
		{
			m_spaceStretch = 0.1f;
			m_stretchSpeed = 10.0f;
		}
		m_velocity = -1.0f * m_distanceFromScreen * STAR_SPEED * m_stretchSpeed * m_game->GetShip()->GetForwardNormal();
		m_position += m_velocity * deltaseconds;

		std::vector<Vertex_PCU> verts;

		AddVertsForLineSegment2D(verts, m_position, Vec2(m_position.x - (m_spaceStretch * m_game->GetShip()->GetForwardNormal().x), m_position.y - (m_spaceStretch * m_game->GetShip()->GetForwardNormal().y)), 0.25f, m_color);

		g_theRenderer->CopyCPUToGPU(verts.data(), sizeof(Vertex_PCU) * 4, m_gpuHyperSpaceMesh);
	}
	else
	{
		m_velocity = -1.0f * m_distanceFromScreen * STAR_SPEED * m_game->GetShip()->GetForwardNormal();
		m_position += m_velocity * deltaseconds;
		m_spaceStretch = 0.0f;

		Vertex_PCU tempVerts[NUM_OF_STAR_VERTICES];

		for (int index = 0; index < NUM_OF_STAR_VERTICES; index++)
		{
			tempVerts[index] = m_vertices[index];
		}

		TransformVertexArrayXY3D(NUM_OF_STAR_VERTICES, tempVerts, 0.125f, m_orientationDegrees, m_position);

		g_theRenderer->CopyCPUToGPU(&tempVerts, sizeof(Vertex_PCU) * NUM_OF_STAR_VERTICES, m_gpuMesh);
	}
}

void Star::Render() const
{
	if (!m_game->IsHyperSpace())
	{
		g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE);
		g_theRenderer->SetBlendMode(BlendMode::ALPHA);
		g_theRenderer->SetDepthMode(DepthMode::ENABLED);
		g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
		g_theRenderer->BindShader();
		g_theRenderer->BindTexture();
		g_theRenderer->DrawVertexBuffer(m_gpuMesh, NUM_OF_STAR_VERTICES, sizeof(Vertex_PCU));
	}
	else
	{
		g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE);
		g_theRenderer->SetBlendMode(BlendMode::ALPHA);
		g_theRenderer->SetDepthMode(DepthMode::ENABLED);
		g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
		g_theRenderer->BindShader();
		g_theRenderer->BindTexture();
		g_theRenderer->DrawVertexBuffer(m_gpuHyperSpaceMesh, 4, sizeof(Vertex_PCU));

		//DrawDebugLine(m_position, Vec2(m_position.x - (m_spaceStretch * m_game->GetShip()->GetForwardNormal().x), m_position.y - (m_spaceStretch * m_game->GetShip()->GetForwardNormal().y)), 0.25f, m_color);
	}
}

void Star::Die()
{
	m_isDead = true;
	m_isGarbage = true;
}