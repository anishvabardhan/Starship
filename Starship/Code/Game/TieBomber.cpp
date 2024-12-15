#include "Game/TieBomber.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerShip.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

TieBomber::TieBomber(Game* owner, Vec2 const& position)
	: Entity(owner, position)
{
	m_physicsRadius = TIE_BOMBER_PHYSICS_RADIUS;
	m_cosmeticRadius = TIE_BOMBER_COSMETIC_RADIUS;
	m_health = 10;
	m_color = Rgba8(128, 128, 128, 255);

	m_gpuMesh = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU) * (NUM_BOMBER_VERTS + NUM_OF_TIE_B_WEAPON_VERTICES));

	Initialize();
}

TieBomber::~TieBomber()
{
	DELETE_PTR(m_gpuMesh);
}

void TieBomber::Initialize()
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

	m_bodyVertices[72] = Vertex_PCU(1.0f, 2.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[73] = Vertex_PCU(1.0f, 2.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[74] = Vertex_PCU(-1.0f, 2.25f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[75] = Vertex_PCU(-1.0f, 2.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[76] = Vertex_PCU(-1.0f, 2.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[77] = Vertex_PCU(1.0f, 2.0f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[78] = Vertex_PCU(1.0f, -2.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[79] = Vertex_PCU(1.0f, -2.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[80] = Vertex_PCU(-1.0f, -2.0f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[81] = Vertex_PCU(-1.0f, -2.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[82] = Vertex_PCU(-1.0f, -2.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[83] = Vertex_PCU(1.0f, -2.25f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[84] = Vertex_PCU(1.0f, 2.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[85] = Vertex_PCU(1.5f, 1.5f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[86] = Vertex_PCU(1.75f, 1.5f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[87] = Vertex_PCU(1.75f, 1.5f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[88] = Vertex_PCU(1.0f, 2.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[89] = Vertex_PCU(1.0f, 2.0f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[90] = Vertex_PCU(-1.0f, 2.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[91] = Vertex_PCU(-1.0f, 2.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[92] = Vertex_PCU(-1.75f, 1.5f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[93] = Vertex_PCU(-1.75f, -1.5f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[94] = Vertex_PCU(-1.0f, -1.5f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[95] = Vertex_PCU(-1.0f, -2.0f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[96] = Vertex_PCU(1.0f, -2.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[97] = Vertex_PCU(1.5f, -1.5f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[98] = Vertex_PCU(1.75f, -1.5f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[99] = Vertex_PCU(1.75f, -1.5f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[100] = Vertex_PCU(1.0f, -2.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[101] = Vertex_PCU(1.0f, -2.0f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[102] = Vertex_PCU(-1.0f, -2.0f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[103] = Vertex_PCU(-1.0f, -2.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[104] = Vertex_PCU(-1.75f, -1.5f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[105] = Vertex_PCU(-1.75f, -1.5f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[106] = Vertex_PCU(-1.0f, -1.5f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[107] = Vertex_PCU(-1.0f, -2.0f, m_color.r, m_color.g, m_color.b, m_color.a);

	//WEAPON
	for (int index = 0; index < 20; index++)
	{
		float thetaDegrees = 360.0f / 20.0f;

		float radius = 0.8f;

		float theta1 = index * thetaDegrees;
		float theta2 = (index + 1) * thetaDegrees;

		Vec2 vert1 = Vec2(radius * CosDegrees(theta1), radius * SinDegrees(theta1));
		Vec2 vert2 = Vec2(radius * CosDegrees(theta2), radius * SinDegrees(theta2));

		m_weaponVertices[3 * index] = Vertex_PCU(0.0f, 0.0f, 0, 0, 0, 255);
		m_weaponVertices[3 * index + 1] = Vertex_PCU(vert1.x, vert1.y, 0, 0, 0, 255);
		m_weaponVertices[3 * index + 2] = Vertex_PCU(vert2.x, vert2.y, 0, 0, 0, 255);
	}

	m_weaponVertices[60] = Vertex_PCU(0.25f, 0.25f, 255, 0, 0, 255);
	m_weaponVertices[61] = Vertex_PCU(1.75f, 0.25f, 255, 0, 0, 255);
	m_weaponVertices[62] = Vertex_PCU(1.75f, 0.5f,  255, 0, 0, 255);

	m_weaponVertices[63] = Vertex_PCU(1.75f, 0.5f, 255, 0, 0, 255);
	m_weaponVertices[64] = Vertex_PCU(0.25f, 0.5f, 255, 0, 0, 255);
	m_weaponVertices[65] = Vertex_PCU(0.25f, 0.25f, 255, 0, 0, 255);

	m_weaponVertices[66] = Vertex_PCU(0.25f, -0.25f, 255, 0, 0, 255);
	m_weaponVertices[67] = Vertex_PCU(0.25f, -0.5f, 255, 0, 0, 255);
	m_weaponVertices[68] = Vertex_PCU(1.75f, -0.5f, 255, 0, 0, 255);

	m_weaponVertices[69] = Vertex_PCU(1.75f, -0.5f, 255, 0, 0, 255);
	m_weaponVertices[70] = Vertex_PCU(1.75f, -0.25f, 255, 0, 0, 255);
	m_weaponVertices[71] = Vertex_PCU(0.25f, -0.25f, 255, 0, 0, 255);
}

void TieBomber::RenderBody() const
{
	Vertex_PCU tempVerts[NUM_BOMBER_VERTS];

	for (int index = 0; index < NUM_BOMBER_VERTS; index++)
	{
		tempVerts[index] = m_bodyVertices[index];
	}

	TransformVertexArrayXY3D(NUM_BOMBER_VERTS, tempVerts, 1.0f * m_scale, m_orientationDegrees, m_position);

	g_theRenderer->BindTexture();
	g_theRenderer->DrawVertexArray(NUM_BOMBER_VERTS, tempVerts);
}

void TieBomber::RenderWeapons() const
{
	Vertex_PCU tempVerts[NUM_OF_TIE_B_WEAPON_VERTICES];

	for (int index = 0; index < NUM_OF_TIE_B_WEAPON_VERTICES; index++)
	{
		tempVerts[index] = m_weaponVertices[index];
	}

	TransformVertexArrayXY3D(NUM_OF_TIE_B_WEAPON_VERTICES, tempVerts, 1.0f * m_scale, m_orientationDegrees, m_position);

	g_theRenderer->BindTexture();
	g_theRenderer->DrawVertexArray(NUM_OF_TIE_B_WEAPON_VERTICES, tempVerts);
}

void TieBomber::Update(float deltaseconds)
{
	Vec2 forwardDirection;

	if (m_game->GetShip()->IsAlive())
	{
		m_lastShipPosition = m_game->GetShip()->GetPosition() - m_position;
	}

	forwardDirection = m_lastShipPosition;

	forwardDirection.Normalize();

	m_orientationDegrees = forwardDirection.GetOrientationDegrees();

	m_velocity.x += TIE_BOMBER_ACCELERATION * deltaseconds;
	m_velocity.y += TIE_BOMBER_ACCELERATION * deltaseconds;

	m_velocity.x = GetClamped(m_velocity.x, 0.0f, 20.0f);
	m_velocity.y = GetClamped(m_velocity.y, 0.0f, 20.0f);

	m_position += forwardDirection * m_velocity * deltaseconds;

	Vertex_PCU tempVerts[NUM_BOMBER_VERTS * NUM_OF_TIE_B_WEAPON_VERTICES];

	for (int index = 0; index < NUM_BOMBER_VERTS; index++)
	{
		tempVerts[index] = m_bodyVertices[index];
	}

	int weaponIndex = 0;

	for (int index = NUM_BOMBER_VERTS; index < NUM_BOMBER_VERTS + NUM_OF_TIE_B_WEAPON_VERTICES; index++)
	{
		tempVerts[index] = m_weaponVertices[weaponIndex];
		weaponIndex++;
	}

	TransformVertexArrayXY3D(NUM_BOMBER_VERTS * NUM_OF_TIE_B_WEAPON_VERTICES, tempVerts, 1.0f * m_scale, m_orientationDegrees, m_position);

	g_theRenderer->CopyCPUToGPU(&tempVerts, sizeof(Vertex_PCU) * (NUM_BOMBER_VERTS + NUM_OF_TIE_B_WEAPON_VERTICES), m_gpuMesh);
}

void TieBomber::Render() const
{
	//RenderBody();
	//RenderWeapons();

	g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetDepthMode(DepthMode::ENABLED);
	g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
	g_theRenderer->BindShader();
	g_theRenderer->BindTexture();
	g_theRenderer->DrawVertexBuffer(m_gpuMesh, NUM_BOMBER_VERTS + NUM_OF_TIE_B_WEAPON_VERTICES, sizeof(Vertex_PCU));

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

void TieBomber::Die()
{
	SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Tie_Explosion.mp3");
	g_theAudio->StartSound(testSound);

	m_isDead = true;
	m_isGarbage = true;
	RandomNumberGenerator random = RandomNumberGenerator();
	Vec2 normal = m_game->GetShip()->GetPosition() - m_position;
	m_game->SpawnDebris(random.RollRandomIntInRange(3, 12), Vec2((m_position.x + (m_cosmeticRadius * CosDegrees(normal.GetOrientationDegrees()))), (m_position.y + (m_cosmeticRadius * SinDegrees(normal.GetOrientationDegrees())))), Vec2(0.0f, 0.0f), random.RollRandomFloatInRange(0.2f, 0.8f), m_color);
}
