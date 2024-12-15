#include "Asteroid.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#include <math.h>

Asteroid::Asteroid(Game* owner, Vec2 const& startPos)
	: Entity(owner, startPos)
{
	m_physicsRadius = ASTEROID_PHYSICS_RADIUS;
	m_cosmeticRadius = ASTEROID_COSMETIC_RADIUS;

	m_Rand = RandomNumberGenerator();
	m_orientationDegrees = m_Rand.RollRandomFloatInRange(-50.0f, 50.0f);
	m_angularVelocity = m_Rand.RollRandomFloatInRange(-200.0f, 200.0f);
	
	Vec2 direction = m_game->GetShip()->GetPosition() - m_position;
	
	direction.Normalize();

	m_velocity = Vec2(ASTEROID_SPEED * direction.x, ASTEROID_SPEED * direction.y);
	m_health = 3;
	m_color = Rgba8(100, 100, 100, 255);

	m_gpuMesh = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU) * NUM_ASTEROID_VERTS);

	Initialize();
}

Asteroid::~Asteroid()
{
	DELETE_PTR(m_gpuMesh);
}

void Asteroid::Initialize()
{
	float thetaDegrees = 360.0f / (float)NUM_ASTEROID_TRIANGLES;

	Vec2 previousVertex;

	for (int index = 0; index < NUM_ASTEROID_TRIANGLES; index++)
	{
		float radius = m_Rand.RollRandomFloatInRange(ASTEROID_PHYSICS_RADIUS, ASTEROID_COSMETIC_RADIUS);

		float theta1 = index * thetaDegrees;
		float theta2 = (index + 1) * thetaDegrees;

		Vec2 vert1 = Vec2(radius * CosDegrees(theta1), radius * SinDegrees(theta1));
		Vec2 vert2 = Vec2(radius * CosDegrees(theta2), radius * SinDegrees(theta2));

		if (previousVertex.x == 0.0f && previousVertex.y == 0.0f)
		{
			previousVertex = vert1;
		}

		m_vertices[3 * index]     = Vertex_PCU(0.0f, 0.0f, m_color.r, m_color.g, m_color.b, m_color.a);
		m_vertices[3 * index + 1] = Vertex_PCU(previousVertex.x, previousVertex.y, m_color.r, m_color.g, m_color.b, m_color.a);
		m_vertices[3 * index + 2] = Vertex_PCU(vert2.x, vert2.y, m_color.r, m_color.g, m_color.b, m_color.a);

		previousVertex = vert2;
	}
}

void Asteroid::Update(float deltaseconds)
{
	if(m_position.x > WORLD_SIZE_X + m_cosmeticRadius)
		m_position.x = -m_cosmeticRadius;

	if (m_position.x < -m_cosmeticRadius)
		m_position.x = WORLD_SIZE_X + m_cosmeticRadius;

	if (m_position.y > WORLD_SIZE_Y + m_cosmeticRadius)
		m_position.y = -m_cosmeticRadius;

	if (m_position.y < -m_cosmeticRadius)
		m_position.y = WORLD_SIZE_Y + m_cosmeticRadius;

	m_orientationDegrees += m_angularVelocity * deltaseconds;
	m_position += m_velocity * deltaseconds;

	Vertex_PCU tempVerts[NUM_ASTEROID_VERTS];

	for (int index = 0; index < NUM_ASTEROID_VERTS; index++)
	{
		tempVerts[index] = m_vertices[index];
	}

	TransformVertexArrayXY3D(NUM_ASTEROID_VERTS, tempVerts, 1.0f, m_orientationDegrees, m_position);

	g_theRenderer->CopyCPUToGPU(&tempVerts, sizeof(Vertex_PCU) * NUM_ASTEROID_VERTS, m_gpuMesh);
}

void Asteroid::Render() const
{
	g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetDepthMode(DepthMode::ENABLED);
	g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
	g_theRenderer->BindShader();
	g_theRenderer->BindTexture();
	g_theRenderer->DrawVertexBuffer(m_gpuMesh, NUM_ASTEROID_VERTS, sizeof(Vertex_PCU));

	if (m_game->IsDeveloperModeOn() && m_game->GetShip())
	{
		DrawDebugLine(m_position, m_game->GetShip()->GetPosition(), 0.2f, Rgba8(50, 50, 50, 255));
		DrawDebugLine(m_position, Vec2((m_position.x + (ASTEROID_COSMETIC_RADIUS * CosDegrees(m_orientationDegrees))), (m_position.y + (ASTEROID_COSMETIC_RADIUS * SinDegrees(m_orientationDegrees)))), 0.2f, Rgba8(255, 0, 0, 255));
		DrawDebugLine(m_position, Vec2((m_position.x + (ASTEROID_COSMETIC_RADIUS * CosDegrees(90.0f + m_orientationDegrees))), (m_position.y + (ASTEROID_COSMETIC_RADIUS * SinDegrees(90.0f + m_orientationDegrees)))), 0.2f, Rgba8(0, 255, 0, 255));
		DrawDebugRing(m_position, ASTEROID_COSMETIC_RADIUS, m_orientationDegrees, 0.2f, Rgba8(255, 0, 255, 255));
		DrawDebugRing(m_position, ASTEROID_PHYSICS_RADIUS, m_orientationDegrees, 0.2f, Rgba8(0, 255, 255, 255));
		DrawDebugLine(m_position, Vec2(m_position.x + (m_velocity.x), m_position.y + (m_velocity.y)), 0.2f, Rgba8(255, 255, 0, 255));
	}
}

void Asteroid::Die()
{
	SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Asteroid_Explosion.wav");
	g_theAudio->StartSound(testSound);

	m_isDead = true;
	m_isGarbage = true;
	RandomNumberGenerator random = RandomNumberGenerator();
	Vec2 normal = m_game->GetShip()->GetPosition() - m_position;
	m_game->SpawnDebris(random.RollRandomIntInRange(3, 12), Vec2((m_position.x + (m_cosmeticRadius * CosDegrees(normal.GetOrientationDegrees()))), (m_position.y + (m_cosmeticRadius * SinDegrees(normal.GetOrientationDegrees())))), Vec2(0.0f, 0.0f),random.RollRandomFloatInRange(0.2f, 0.8f), m_color);
}
