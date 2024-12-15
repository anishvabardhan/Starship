#include "Debris.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#include <math.h>

extern Renderer* g_theRenderer;

Debris::Debris(Game* owner, Vec2 const& startPos, Vec2 const& velocity, float scale, Rgba8 const& color)
	: Entity(owner, startPos)
{
	m_physicsRadius = DEBRIS_PHYSICS_RADIUS;
	m_cosmeticRadius = DEBRIS_COSMETIC_RADIUS;

	RandomNumberGenerator rand = RandomNumberGenerator();
	m_orientationDegrees = rand.RollRandomFloatInRange(-50.0f, 50.0f);
	m_angularVelocity = rand.RollRandomFloatInRange(-200.0f, 200.0f);
	m_velocity = Vec2(rand.RollRandomFloatInRange(-1.5f, 1.5f), rand.RollRandomFloatInRange(-1.5f, 1.5f));
	m_speed = rand.RollRandomFloatInRange(0.0f, DEBRIS_SPEED);
	m_velocity += velocity;
	m_scale = scale;
	m_color = color;

	Initialize();
}

Debris::~Debris()
{
	DELETE_PTR(m_gpuMesh);
}

void Debris::Initialize()
{
	float thetaDegrees = 360.0f / static_cast<float>(NUM_DEBRIS_TRIANGLES);

	Vec2 previousVertex;

	for (int index = 0; index < NUM_DEBRIS_TRIANGLES; index++)
	{
		RandomNumberGenerator rand = RandomNumberGenerator();

		float radius = rand.RollRandomFloatInRange(DEBRIS_PHYSICS_RADIUS, DEBRIS_COSMETIC_RADIUS);

		float theta1 = index * thetaDegrees;
		float theta2 = (index + 1) * thetaDegrees;

		Vec2 vert1 = Vec2(radius * CosDegrees(theta1), radius * SinDegrees(theta1));
		Vec2 vert2 = Vec2(radius * CosDegrees(theta2), radius * SinDegrees(theta2));

		if (previousVertex.x == 0.0f && previousVertex.y == 0.0f)
		{
			previousVertex = vert1;
		}

		m_vertices[3 * index] = Vertex_PCU(0.0f, 0.0f, m_color.r, m_color.g, m_color.b, m_color.a);
		m_vertices[3 * index + 1] = Vertex_PCU(previousVertex.x, previousVertex.y, m_color.r, m_color.g, m_color.b, m_color.a);
		m_vertices[3 * index + 2] = Vertex_PCU(vert2.x, vert2.y, m_color.r, m_color.g, m_color.b, m_color.a);

		previousVertex = vert2;
	}

	m_gpuMesh = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU) * NUM_DEBRIS_VERTS);
}

void Debris::Update(float deltaseconds)
{
	m_aliveTime += deltaseconds;

	m_orientationDegrees += m_angularVelocity * deltaseconds;
	m_position += m_velocity * m_speed * deltaseconds;

	if (m_aliveTime < 1.0f)
	{
		m_color.a -= m_age;
	}

	Vertex_PCU tempVerts[NUM_DEBRIS_VERTS];

	for (int index = 0; index < NUM_DEBRIS_VERTS; index++)
	{
		tempVerts[index].m_position = m_vertices[index].m_position;
		tempVerts[index].m_color = m_color;
		tempVerts[index].m_uvTexCoords = m_vertices[index].m_uvTexCoords;
	}

	TransformVertexArrayXY3D(NUM_DEBRIS_VERTS, tempVerts, m_scale, m_orientationDegrees, m_position);

	g_theRenderer->CopyCPUToGPU(&tempVerts, sizeof(Vertex_PCU) * NUM_DEBRIS_VERTS, m_gpuMesh);
}

void Debris::Render() const
{
	g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetDepthMode(DepthMode::ENABLED);
	g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
	g_theRenderer->BindShader();
	g_theRenderer->BindTexture();
	g_theRenderer->DrawVertexBuffer(m_gpuMesh, NUM_DEBRIS_VERTS, sizeof(Vertex_PCU));

	if (m_game->IsDeveloperModeOn() && m_game->GetShip())
	{
		DrawDebugLine(m_position, m_game->GetShip()->GetPosition(), 0.2f, Rgba8(50, 50, 50, 255));
		DrawDebugLine(m_position, Vec2((m_position.x + (m_cosmeticRadius * CosDegrees(m_orientationDegrees))), (m_position.y + (m_cosmeticRadius * SinDegrees(m_orientationDegrees)))), 0.2f, Rgba8(255, 0, 0, 255));
		DrawDebugLine(m_position, Vec2((m_position.x + (m_cosmeticRadius * CosDegrees(90.0f + m_orientationDegrees))), (m_position.y + (m_cosmeticRadius * SinDegrees(90.0f + m_orientationDegrees)))), 0.2f, Rgba8(0, 255, 0, 255));
		DrawDebugRing(m_position, m_cosmeticRadius, m_orientationDegrees, 0.2f, Rgba8(255, 0, 255, 255));
		DrawDebugRing(m_position, m_physicsRadius, m_orientationDegrees, 0.2f, Rgba8(0, 255, 255, 255));
		DrawDebugLine(m_position, Vec2(m_position.x + (10.0f * m_velocity.x), m_position.y + (10.0f * m_velocity.y)), 0.2f, Rgba8(255, 255, 0, 255));
	}
}

void Debris::Die()
{
	m_isDead = true;
	m_isGarbage = true;
}