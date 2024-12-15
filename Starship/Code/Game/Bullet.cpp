#include "Game/Bullet.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"

Bullet::Bullet(Game* owner, Vec2 const& startPos, float const& orientation, Rgba8 color)
	: Entity(owner, startPos)
{
	m_velocity = Vec2(BULLET_SPEED, BULLET_SPEED);
	m_physicsRadius = BULLET_PHYSICS_RADIUS;
	m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
	m_orientationDegrees = orientation;
	m_health = 1;
	m_color = color;

	m_gpuMesh = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU) * (NUM_OF_BULLET_VERTICES + NUM_OF_GLOW_VERTICES));

	Initialize();
}

Bullet::~Bullet()
{
	DELETE_PTR(m_gpuMesh);
}

void Bullet::Initialize()
{
	m_vertices[0] = Vertex_PCU(-1.0f, -0.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_vertices[1] = Vertex_PCU(1.0f, -0.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_vertices[2] = Vertex_PCU(1.0f, 0.25f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_vertices[3] = Vertex_PCU(1.0f, 0.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_vertices[4] = Vertex_PCU(-1.0f, 0.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_vertices[5] = Vertex_PCU(-1.0f, -0.25f, m_color.r, m_color.g, m_color.b, m_color.a);

	float thetaDegrees = 360.0f / static_cast<float>(NUM_OF_GLOW_TRIANGLES);

	for (int index = 0; index < NUM_OF_GLOW_TRIANGLES; index++)
	{
		float radius = 2.25f;

		float theta1 = index * thetaDegrees;
		float theta2 = (index + 1) * thetaDegrees;

		Vec2 vert1 = Vec2(radius * CosDegrees(theta1), radius * SinDegrees(theta1));
		Vec2 vert2 = Vec2(radius * CosDegrees(theta2), radius * SinDegrees(theta2));

		m_glowVertices[3 * index] = Vertex_PCU(0.0f, 0.0f, m_color.r, m_color.g, m_color.b, 127);
		m_glowVertices[3 * index + 1] = Vertex_PCU(vert1.x, vert1.y, m_color.r, m_color.g, m_color.b, 0);
		m_glowVertices[3 * index + 2] = Vertex_PCU(vert2.x, vert2.y, m_color.r, m_color.g, m_color.b, 0);
	}
}

void Bullet::RenderGlow() const
{
	Vertex_PCU tempVerts[NUM_OF_GLOW_VERTICES];

	for (int index = 0; index < NUM_OF_GLOW_VERTICES; index++)
	{
		tempVerts[index] = m_glowVertices[index];
	}

	TransformVertexArrayXY3D(NUM_OF_GLOW_VERTICES, tempVerts, 1.0f, m_orientationDegrees, m_position);

	g_theRenderer->BindTexture();
	g_theRenderer->DrawVertexArray(NUM_OF_GLOW_VERTICES, tempVerts);
}

void Bullet::Update(float deltaseconds)
{
	m_aliveTime += deltaseconds;

	m_position.x += GetForwardNormal().x * m_velocity.x * deltaseconds;
	m_position.y += GetForwardNormal().y * m_velocity.y * deltaseconds;

	Vertex_PCU tempVerts[NUM_OF_GLOW_VERTICES * NUM_OF_BULLET_VERTICES];

	for (int index = 0; index < NUM_OF_BULLET_VERTICES; index++)
	{
		tempVerts[index] = m_vertices[index];
	}

	int weaponIndex = 0;

	for (int index = NUM_OF_BULLET_VERTICES; index < NUM_OF_GLOW_VERTICES + NUM_OF_BULLET_VERTICES; index++)
	{
		tempVerts[index] = m_glowVertices[weaponIndex];
		weaponIndex++;
	}

	TransformVertexArrayXY3D(NUM_OF_GLOW_VERTICES * NUM_OF_BULLET_VERTICES, tempVerts, 1.0f * m_scale, m_orientationDegrees, m_position);

	g_theRenderer->CopyCPUToGPU(&tempVerts, sizeof(Vertex_PCU) * (NUM_OF_GLOW_VERTICES * NUM_OF_BULLET_VERTICES), m_gpuMesh);
}

void Bullet::Render() const
{
	//Vertex_PCU tempVerts[NUM_OF_BULLET_VERTICES];
	//
	//for (int index = 0; index < NUM_OF_BULLET_VERTICES; index++)
	//{
	//	tempVerts[index] = m_vertices[index];
	//}
	//
	//TransformVertexArrayXY3D(NUM_OF_BULLET_VERTICES, tempVerts, 1.0f, m_orientationDegrees, m_position);
	//
	//g_theRenderer->BindTexture();
	//g_theRenderer->DrawVertexArray(NUM_OF_BULLET_VERTICES, tempVerts);
	//
	//RenderGlow();

	g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetDepthMode(DepthMode::ENABLED);
	g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
	g_theRenderer->BindShader();
	g_theRenderer->BindTexture();
	g_theRenderer->DrawVertexBuffer(m_gpuMesh, NUM_OF_GLOW_VERTICES + NUM_OF_BULLET_VERTICES, sizeof(Vertex_PCU));

	if (m_game->IsDeveloperModeOn() && m_game->GetShip())
	{
		DrawDebugLine(m_position, m_game->GetShip()->GetPosition(), 0.2f, Rgba8(50, 50, 50, 255));
		DrawDebugLine(m_position, Vec2((m_position.x + (m_cosmeticRadius * CosDegrees(m_orientationDegrees))), (m_position.y + (m_cosmeticRadius * SinDegrees(m_orientationDegrees)))), 0.2f, Rgba8(255, 0, 0, 255));
		DrawDebugLine(m_position, Vec2((m_position.x + (m_cosmeticRadius * CosDegrees(90.0f + m_orientationDegrees))), (m_position.y + (m_cosmeticRadius * SinDegrees(90.0f + m_orientationDegrees)))), 0.2f, Rgba8(0, 255, 0, 255));
		DrawDebugRing(m_position, m_cosmeticRadius, m_orientationDegrees, 0.2f, Rgba8(255, 0, 255, 255));
		DrawDebugRing(m_position, m_physicsRadius, m_orientationDegrees, 0.2f, Rgba8(0, 255, 255, 255));
		DrawDebugLine(m_position, Vec2((m_position.x + (10.0f * m_cosmeticRadius * CosDegrees(m_orientationDegrees))), (m_position.y + (10.0f * m_cosmeticRadius * SinDegrees(m_orientationDegrees)))), 0.2f, Rgba8(255, 255, 0, 255));
	}
}

void Bullet::Die()
{
	m_isDead = true;
	m_isGarbage = true;
}
