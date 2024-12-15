#include "Game/TieFighter.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

TieFighter::TieFighter(Game* owner, Vec2 const& position)
	: Entity(owner, position)
{
	m_velocity = Vec2(TIE_FIGHTER_SPEED, TIE_FIGHTER_SPEED);
	m_physicsRadius = TIE_FIGHTER_PHYSICS_RADIUS;
	m_cosmeticRadius = TIE_FIGHTER_COSMETIC_RADIUS;
	m_health = 5;
	m_color = Rgba8(128, 128, 128, 255);

	m_gpuMesh = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU) * (NUM_FIGHTER_VERTS + NUM_OF_TIE_WEAPON_VERTICES));

	Initialize();
}

TieFighter::~TieFighter()
{
	DELETE_PTR(m_gpuMesh);
}

void TieFighter::Initialize()
{
	//BODY
	for (int index = 0; index < 20; index++)
	{
		float thetaDegrees = 360.0f / 20.0f;

		float radius = 1.0f;

		float theta1 = index * thetaDegrees;
		float theta2 = (index + 1) * thetaDegrees;

		Vec2 vert1 = Vec2(radius * CosDegrees(theta1), radius * SinDegrees(theta1));
		Vec2 vert2 = Vec2(radius * CosDegrees(theta2), radius * SinDegrees(theta2));

		m_bodyVertices[3 * index] = Vertex_PCU(0.0f, 0.0f, m_color.r, m_color.g, m_color.b, m_color.a);
		m_bodyVertices[3 * index + 1] = Vertex_PCU(vert1.x, vert1.y, m_color.r, m_color.g, m_color.b, m_color.a);
		m_bodyVertices[3 * index + 2] = Vertex_PCU(vert2.x, vert2.y, m_color.r, m_color.g, m_color.b, m_color.a);
	}

	m_bodyVertices[60] = Vertex_PCU(-0.25f, 0.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[61] = Vertex_PCU(0.25f, 0.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[62] = Vertex_PCU(0.25f, 2.0f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[63] = Vertex_PCU(0.25f, 2.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[64] = Vertex_PCU(-0.25f, 2.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[65] = Vertex_PCU(-0.25f, 0.0f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[66] = Vertex_PCU(0.25f, 0.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[67] = Vertex_PCU(-0.25f, 0.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[68] = Vertex_PCU(-0.25f, -2.0f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[69] = Vertex_PCU(-0.25f, -2.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[70] = Vertex_PCU(0.25f, -2.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[71] = Vertex_PCU(0.25f, 0.0f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[72] = Vertex_PCU(3.0f, 2.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[73] = Vertex_PCU(3.0f, 2.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[74] = Vertex_PCU(-3.0f, 2.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	
	m_bodyVertices[75] = Vertex_PCU(-3.0f, 2.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[76] = Vertex_PCU(-3.0f, 2.0f,  m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[77] = Vertex_PCU(3.0f, 2.0f,   m_color.r, m_color.g, m_color.b, m_color.a);
	
	m_bodyVertices[78] = Vertex_PCU(3.0f, -2.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[79] = Vertex_PCU(3.0f, -2.0f,  m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[80] = Vertex_PCU(-3.0f, -2.0f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[81] = Vertex_PCU(-3.0f, -2.0f,  m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[82] = Vertex_PCU(-3.0f, -2.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[83] = Vertex_PCU(3.0f, -2.25f,  m_color.r, m_color.g, m_color.b, m_color.a);

	for (int index = 0; index < 20; index++)
	{
		float thetaDegrees = 360.0f / 20.0f;

		float radius = 0.8f;

		float theta1 = index * thetaDegrees;
		float theta2 = (index + 1) * thetaDegrees;

		Vec2 vert1 = Vec2(radius * CosDegrees(theta1), radius * SinDegrees(theta1));
		Vec2 vert2 = Vec2(radius * CosDegrees(theta2), radius * SinDegrees(theta2));

		m_weaponVertices[3 * index] = Vertex_PCU(0.0f, 0.0f,           58, 58, 58, 255);
		m_weaponVertices[3 * index + 1] = Vertex_PCU(vert1.x, vert1.y, 58, 58, 58, 255);
		m_weaponVertices[3 * index + 2] = Vertex_PCU(vert2.x, vert2.y, 58, 58, 58, 255);
	}

	m_weaponVertices[60] = Vertex_PCU(0.25f, 0.25f, 58, 58, 58, 255);
	m_weaponVertices[61] = Vertex_PCU(1.75f, 0.25f, 58, 58, 58, 255);
	m_weaponVertices[62] = Vertex_PCU(1.75f, 0.5f,  58, 58, 58, 255);

	m_weaponVertices[63] = Vertex_PCU(1.75f, 0.5f,  58, 58, 58, 255);
	m_weaponVertices[64] = Vertex_PCU(0.25f, 0.5f,  58, 58, 58, 255);
	m_weaponVertices[65] = Vertex_PCU(0.25f, 0.25f, 58, 58, 58, 255);

	m_weaponVertices[66] = Vertex_PCU(0.25f, -0.25f, 58, 58, 58, 255);
	m_weaponVertices[67] = Vertex_PCU(0.25f, -0.5f,  58, 58, 58, 255);
	m_weaponVertices[68] = Vertex_PCU(1.75f, -0.5f,  58, 58, 58, 255);

	m_weaponVertices[69] = Vertex_PCU(1.75f, -0.5f,  58, 58, 58, 255);
	m_weaponVertices[70] = Vertex_PCU(1.75f, -0.25f, 58, 58, 58, 255);
	m_weaponVertices[71] = Vertex_PCU(0.25f, -0.25f, 58, 58, 58, 255);
}

void TieFighter::RenderBody() const
{

	//g_theRenderer->BindTexture();
	//g_theRenderer->DrawVertexArray(NUM_FIGHTER_VERTS, tempVerts);
}

void TieFighter::RenderWeapons() const
{

	//g_theRenderer->BindTexture();
	//g_theRenderer->DrawVertexArray(NUM_OF_TIE_WEAPON_VERTICES, tempVerts);
}

void TieFighter::Update(float deltaseconds)
{
	m_bulletDelay += deltaseconds;

	if (m_game->GetShip()->IsAlive())
	{
		float orientation = (m_game->GetShip()->GetPosition() - m_position).GetOrientationDegrees();
		float distance = GetDistance2D(m_game->GetShip()->GetPosition(), m_position);

		if (distance < 200.0f)
		{
			float shipConeThreshold = 20.0f;

			RandomNumberGenerator random = RandomNumberGenerator();

			float delay = random.RollRandomFloatInRange(1.0f, 2.0f);

			if (m_bulletDelay > delay)
			{
				m_bulletDelay = 0.0f;

				if (orientation > m_orientationDegrees - shipConeThreshold && orientation < m_orientationDegrees + shipConeThreshold)
				{

					SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Tie_Laser.mp3");
					g_theAudio->StartSound(testSound);

					m_game->SpawnBullet(m_position, m_orientationDegrees, Rgba8(0, 255, 0, 255));
				}
			}
		}
	}

	Vec2 forwardDirection;

	if (m_game->GetShip()->IsAlive())
	{
		m_lastShipPosition = m_game->GetShip()->GetPosition() - m_position;
	}

	forwardDirection = m_lastShipPosition;

	forwardDirection.Normalize();

	m_orientationDegrees = forwardDirection.GetOrientationDegrees();

	m_position += forwardDirection * m_velocity * deltaseconds;

	Vertex_PCU tempVerts[NUM_FIGHTER_VERTS * NUM_OF_TIE_WEAPON_VERTICES];

	for (int index = 0; index < NUM_FIGHTER_VERTS; index++)
	{
		tempVerts[index] = m_bodyVertices[index];
	}

	int weaponIndex = 0;

	for (int index = NUM_FIGHTER_VERTS; index < NUM_FIGHTER_VERTS + NUM_OF_TIE_WEAPON_VERTICES; index++)
	{
		tempVerts[index] = m_weaponVertices[weaponIndex];
		weaponIndex++;
	}

	TransformVertexArrayXY3D(NUM_FIGHTER_VERTS * NUM_OF_TIE_WEAPON_VERTICES, tempVerts, 1.0f * m_scale, m_orientationDegrees, m_position);

	g_theRenderer->CopyCPUToGPU(&tempVerts, sizeof(Vertex_PCU) * (NUM_FIGHTER_VERTS + NUM_OF_TIE_WEAPON_VERTICES), m_gpuMesh);
}

void TieFighter::Render() const
{
	//RenderBody();
	//RenderWeapons();

	g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetDepthMode(DepthMode::ENABLED);
	g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
	g_theRenderer->BindShader();
	g_theRenderer->BindTexture();
	g_theRenderer->DrawVertexBuffer(m_gpuMesh, NUM_FIGHTER_VERTS + NUM_OF_TIE_WEAPON_VERTICES, sizeof(Vertex_PCU));

	if (m_game->IsDeveloperModeOn() && m_game->GetShip())
	{
		DrawDebugLine(m_position, m_game->GetShip()->GetPosition(), 0.2f, Rgba8(50, 50, 50, 255));
		DrawDebugLine(m_position, Vec2((m_position.x + (m_cosmeticRadius * CosDegrees(m_orientationDegrees))), (m_position.y + (m_cosmeticRadius * SinDegrees(m_orientationDegrees)))), 0.2f, Rgba8(255, 0, 0, 255));
		DrawDebugLine(m_position, Vec2((m_position.x + (m_cosmeticRadius * CosDegrees(90.0f + m_orientationDegrees))), (m_position.y + (m_cosmeticRadius * SinDegrees(90.0f + m_orientationDegrees)))), 0.2f, Rgba8(0, 255, 0, 255));
		DrawDebugRing(m_position, m_cosmeticRadius, m_orientationDegrees, 0.2f, Rgba8(255, 0, 255, 255));
		DrawDebugRing(m_position, m_physicsRadius, m_orientationDegrees, 0.2f, Rgba8(0, 255, 255, 255));
		DrawDebugLine(m_position, Vec2((m_position.x + (5.0f * m_cosmeticRadius * CosDegrees(m_orientationDegrees))), (m_position.y + (5.0f * m_cosmeticRadius * SinDegrees(m_orientationDegrees)))), 0.2f, Rgba8(255, 255, 0, 255));
	}
}

void TieFighter::Die()
{
	SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Tie_Explosion.mp3");
	g_theAudio->StartSound(testSound);

	m_isDead = true;
	m_isGarbage = true;
	RandomNumberGenerator random = RandomNumberGenerator();
	Vec2 normal = m_game->GetShip()->GetPosition() - m_position;
	m_game->SpawnDebris(random.RollRandomIntInRange(3, 12), Vec2((m_position.x + (m_cosmeticRadius * CosDegrees(normal.GetOrientationDegrees()))), (m_position.y + (m_cosmeticRadius * SinDegrees(normal.GetOrientationDegrees())))), Vec2(0.0f, 0.0f), random.RollRandomFloatInRange(0.2f, 0.8f), m_color);
}
