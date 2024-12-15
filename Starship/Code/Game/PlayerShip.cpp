#include "Game/PlayerShip.hpp"

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

#include <vector>

PlayerShip::PlayerShip(Game* owner, Vec2 const& startPos)
	: Entity(owner, startPos)
{
	m_cosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
	m_physicsRadius = PLAYER_SHIP_PHYSICS_RADIUS;
	m_angularVelocity = PLAYER_SHIP_TURN_SPEED;
	m_health = 100;
	m_color = Rgba8(189, 195, 199, 255);

	m_gpuBodyMesh = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU) * (NUM_OF_MAIN_BODY_VERTICES + NUM_OF_COCKPIT_VERTICES + NUM_OF_FRONT_VERTICES + NUM_OF_SHIP_WEAPON_VERTICES));
	m_gpuThrusterMesh = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU) * 300);

	Initialize();
}

PlayerShip::~PlayerShip()
{
	DELETE_PTR(m_gpuBodyMesh);
	DELETE_PTR(m_gpuThrusterMesh);
}

void PlayerShip::Initialize()
{
	//BODY
	for (int index = 0; index < NUM_OF_MAIN_BODY_TRIANGLES; index++)
	{
		float thetaDegrees = 360.0f / static_cast<float>(NUM_OF_MAIN_BODY_TRIANGLES);

		float radius = 1.0f;

		float theta1 = index * thetaDegrees;
		float theta2 = (index + 1) * thetaDegrees;

		Vec2 vert1 = Vec2(radius * CosDegrees(theta1), radius * SinDegrees(theta1));
		Vec2 vert2 = Vec2(radius * CosDegrees(theta2), radius * SinDegrees(theta2));

		m_bodyVertices[3 * index] = Vertex_PCU(0.0f, 0.0f,           m_color.r, m_color.g, m_color.b, m_color.a);
		m_bodyVertices[3 * index + 1] = Vertex_PCU(vert1.x, vert1.y, m_color.r, m_color.g, m_color.b, m_color.a);
		m_bodyVertices[3 * index + 2] = Vertex_PCU(vert2.x, vert2.y, m_color.r, m_color.g, m_color.b, m_color.a);
	}

	m_bodyVertices[60] = Vertex_PCU(0.25f, 0.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[61] = Vertex_PCU(1.55f, 0.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[62] = Vertex_PCU(1.55f, 0.5f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[63] = Vertex_PCU(1.55f, 0.5f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[64] = Vertex_PCU(0.25f, 0.95f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[65] = Vertex_PCU(0.25f, 0.25f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[66] = Vertex_PCU(0.25f, -0.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[67] = Vertex_PCU(0.25f, -0.95f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[68] = Vertex_PCU(1.55f, -0.5f, m_color.r, m_color.g, m_color.b, m_color.a);

	m_bodyVertices[69] = Vertex_PCU(1.55f, -0.5f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[70] = Vertex_PCU(1.55f, -0.25f, m_color.r, m_color.g, m_color.b, m_color.a);
	m_bodyVertices[71] = Vertex_PCU(0.25f, -0.25f, m_color.r, m_color.g, m_color.b, m_color.a);


	//WEAPONS
	for (int index = 0; index < 6; index++)
	{
		float thetaDegrees = 360.0f / static_cast<float>(6);

		float radius = 1.0f;

		float theta1 = index * thetaDegrees;
		float theta2 = (index + 1) * thetaDegrees;

		Vec2 vert1 = Vec2(radius * CosDegrees(theta1), radius * SinDegrees(theta1));
		Vec2 vert2 = Vec2(radius * CosDegrees(theta2), radius * SinDegrees(theta2));

		m_weaponVertices[3 * index] = Vertex_PCU(0.0f, 0.0f,           90, 90, 90, 255);
		m_weaponVertices[3 * index + 1] = Vertex_PCU(vert1.x, vert1.y, 90, 90, 90, 255);
		m_weaponVertices[3 * index + 2] = Vertex_PCU(vert2.x, vert2.y, 90, 90, 90, 255);
	}

	m_weaponVertices[18] = Vertex_PCU(0.25f, 0.25f, 90, 90, 90, 255);
	m_weaponVertices[19] = Vertex_PCU(1.75f, 0.25f, 90, 90, 90, 255);
	m_weaponVertices[20] = Vertex_PCU(1.75f, 0.5f,  90, 90, 90, 255);

	m_weaponVertices[21] = Vertex_PCU(1.75f, 0.5f, 90, 90, 90, 255);
	m_weaponVertices[22] = Vertex_PCU(0.25f, 0.5f, 90, 90, 90, 255);
	m_weaponVertices[23] = Vertex_PCU(0.25f, 0.25f, 90, 90, 90, 255);

	m_weaponVertices[24] = Vertex_PCU(0.25f, -0.25f, 90, 90, 90, 255);
	m_weaponVertices[25] = Vertex_PCU(0.25f, -0.5f, 90, 90, 90, 255);
	m_weaponVertices[26] = Vertex_PCU(1.75f, -0.5f, 90, 90, 90, 255);

	m_weaponVertices[27] = Vertex_PCU(1.75f, -0.5f, 90, 90, 90, 255);
	m_weaponVertices[28] = Vertex_PCU(1.75f, -0.25f, 90, 90, 90, 255);
	m_weaponVertices[29] = Vertex_PCU(0.25f, -0.25f, 90, 90, 90, 255);

	// COCKPIT
	m_cockpitVertices[0] = Vertex_PCU(-1.0f, 0.0f, 128, 128, 128, 255);
	m_cockpitVertices[1] = Vertex_PCU(2.5f, -3.5f, 128, 128, 128, 255);
	m_cockpitVertices[2] = Vertex_PCU(2.5f, -2.5f, 128, 128, 128, 255);

	m_cockpitVertices[3] = Vertex_PCU(2.5f, -2.5f, 128, 128, 128, 255);
	m_cockpitVertices[4] = Vertex_PCU(0.0f, 0.0f,  128, 128, 128, 255);
	m_cockpitVertices[5] = Vertex_PCU(-1.0f, 0.0f, 128, 128, 128, 255);

	m_cockpitVertices[6] = Vertex_PCU(2.5f, -3.5f, 128, 128, 128, 255);
	m_cockpitVertices[7] = Vertex_PCU(3.0f, -3.25f, 128, 128, 128, 255);
	m_cockpitVertices[8] = Vertex_PCU(3.0f, -2.75f, 128, 128, 128, 255);

	m_cockpitVertices[9] = Vertex_PCU(3.0f, -2.75f, 128, 128, 128, 255);
	m_cockpitVertices[10] = Vertex_PCU(2.5f, -2.5f, 128, 128, 128, 255);
	m_cockpitVertices[11] = Vertex_PCU(2.5f, -3.5f, 128, 128, 128, 255);
}

void PlayerShip::BounceOffWalls()
{
	if ((m_position.x + m_physicsRadius > WORLD_SIZE_X) || (m_position.x - m_physicsRadius < 0.0f))
		m_velocity.x = -m_velocity.x;

	if ((m_position.y + m_physicsRadius > WORLD_SIZE_Y) || (m_position.y - m_physicsRadius < 0.0f))
		m_velocity.y = -m_velocity.y;
}

void PlayerShip::HandleKeyboardInput()
{
	if (!m_game->m_isAttractMode && !g_theConsole->IsOpen())
	{
		if (g_theInputSystem->WasKeyJustPressed(KEYCODE_SPACE))
		{
			SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Player_Laser.wav");
			g_theAudio->StartSound(testSound);

			m_game->SpawnBullet(m_position, m_orientationDegrees, Rgba8(255, 0, 0, 255));
		}
	}

	if (g_theInputSystem->WasKeyJustPressed('S'))
	{
		m_isTurningLeft = true;
	}

	if (g_theInputSystem->WasKeyJustReleased('S'))
	{
		m_isTurningLeft = false;
	}

	if (g_theInputSystem->WasKeyJustPressed('F'))
	{
		m_isTurningRight = true;
	}

	if (g_theInputSystem->WasKeyJustReleased('F'))
	{
		m_isTurningRight = false;
	}

	if (g_theInputSystem->WasKeyJustPressed('E'))
	{
		m_isShipThrusting = true;
		m_thrustPower = 0.8f;
	}

	if (g_theInputSystem->WasKeyJustReleased('E'))
	{
		m_isShipThrusting = false;
		m_thrustPower = 0.0f;
	}
}

void PlayerShip::UpdateThruster(float deltaseconds)
{
	if (m_game->IsHyperSpace())
	{
		if (m_game->m_hyperSpaceTimer > 4.0f && m_game->m_hyperSpaceTimer <= 6.0f)
		{
			m_thrustPower -= 10.0f * deltaseconds;
		}
		else if (m_game->m_hyperSpaceTimer > 0.0f && m_game->m_hyperSpaceTimer <= 2.0f)
		{
			m_thrustPower += 10.0f * deltaseconds;
		}
	}
	else
	{
		m_thrustPower = 2.0f;
	}
}

void PlayerShip::UpdateFromController(float deltaseconds)
{
	UNUSED(deltaseconds);

	XboxController const& controller = g_theInputSystem->GetController(0);

	float leftStickMag = controller.GetLeftStick().GetMagnitude();

	if (leftStickMag > 0.0f)
	{
		m_isShipThrusting = true;
		m_thrustFraction = leftStickMag;
		m_thrustPower = GetClamped(10.0f * m_thrustFraction, 0.0f, 5.0f);
		m_orientationDegrees = controller.GetLeftStick().GetOrientationDegrees();
	}
	else if(leftStickMag <= 0.0f && !g_theInputSystem->IsKeyDown('E'))
	{
		m_isShipThrusting = false;
	}

	if (controller.WasButtonJustPressed(XboxButtonID::BUTTON_A))
	{
		SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Player_Laser.wav");
		g_theAudio->StartSound(testSound);

		m_game->SpawnBullet(m_position, m_orientationDegrees, Rgba8(255, 0, 0, 255));
	}
}

void PlayerShip::Update(float deltaseconds)
{
	UpdateThruster(deltaseconds);

	if (!m_game->IsHyperSpace())
	{
		HandleKeyboardInput();
		BounceOffWalls();
		UpdateFromController(deltaseconds);

		if (m_isShipThrusting)
		{
			m_velocity.x += GetForwardNormal().x * PLAYER_SHIP_ACCELERATION * m_thrustFraction * deltaseconds;
			m_velocity.y += GetForwardNormal().y * PLAYER_SHIP_ACCELERATION * m_thrustFraction * deltaseconds;
		}

		if (m_isTurningLeft)
			m_orientationDegrees += PLAYER_SHIP_TURN_SPEED * deltaseconds;

		if (m_isTurningRight)
			m_orientationDegrees -= PLAYER_SHIP_TURN_SPEED * deltaseconds;

		m_position += m_velocity * deltaseconds;

		std::vector<Vertex_PCU> verts;

		Vertex_PCU shipVerts[NUM_OF_VERTICES];

		for (int index = 0; index < NUM_OF_VERTICES; index++)
		{
			shipVerts[index] = m_bodyVertices[index];
		}

		TransformVertexArrayXY3D(NUM_OF_VERTICES, shipVerts, 3.0f * m_scale, m_orientationDegrees, m_position);

		for (int index = 0; index < NUM_OF_VERTICES; index++)
		{
			verts.push_back(shipVerts[index]);
		}

		Vertex_PCU cockpitVerts[NUM_OF_COCKPIT_VERTICES];

		for (int index = 0; index < NUM_OF_COCKPIT_VERTICES; index++)
		{
			cockpitVerts[index] = m_cockpitVertices[index];
		}

		TransformVertexArrayXY3D(NUM_OF_COCKPIT_VERTICES, cockpitVerts, 1.0f * m_scale, m_orientationDegrees, m_position);

		for (int index = 0; index < NUM_OF_COCKPIT_VERTICES; index++)
		{
			verts.push_back(cockpitVerts[index]);
		}

		Vertex_PCU weaponVerts[NUM_OF_SHIP_WEAPON_VERTICES];

		for (int index = 0; index < NUM_OF_SHIP_WEAPON_VERTICES; index++)
		{
			weaponVerts[index] = m_weaponVertices[index];
		}

		TransformVertexArrayXY3D(NUM_OF_SHIP_WEAPON_VERTICES, weaponVerts, 1.0f * m_scale, m_orientationDegrees, m_position);

		for (int index = 0; index < NUM_OF_SHIP_WEAPON_VERTICES; index++)
		{
			verts.push_back(weaponVerts[index]);
		}

		g_theRenderer->CopyCPUToGPU(verts.data(), sizeof(Vertex_PCU) * (NUM_OF_MAIN_BODY_VERTICES + NUM_OF_COCKPIT_VERTICES + NUM_OF_FRONT_VERTICES + NUM_OF_SHIP_WEAPON_VERTICES), m_gpuBodyMesh);

		if (m_isShipThrusting)
		{
			float thetaDegrees = 360.0f / 50.0f;

			Vertex_PCU vertices[300];

			for (int index = 0; index < 12; index++)
			{
				float theta1 = index * thetaDegrees;
				float theta2 = (index + 1) * thetaDegrees;

				Vec2 vert1 = Vec2(2.0f * CosDegrees(theta1) + 0.2f, 2.0f * SinDegrees(theta1) + 0.2f);
				Vec2 vert2 = Vec2(2.0f * CosDegrees(theta2) + 0.2f, 2.0f * SinDegrees(theta2) + 0.2f);
				Vec2 vert3 = Vec2((2.0f + m_thrustPower) * CosDegrees(theta1) + 0.2f, (2.0f + m_thrustPower) * SinDegrees(theta1) + 0.2f);
				Vec2 vert4 = Vec2((2.0f + m_thrustPower) * CosDegrees(theta2) + 0.2f, (2.0f + m_thrustPower) * SinDegrees(theta2) + 0.2f);

				vertices[6 * index] = Vertex_PCU(vert1.x, vert1.y,     0, 255, 255, 127);
				vertices[6 * index + 1] = Vertex_PCU(vert3.x, vert3.y, 0, 255, 255, 0);
				vertices[6 * index + 2] = Vertex_PCU(vert4.x, vert4.y, 0, 255, 255, 0);
				vertices[6 * index + 3] = Vertex_PCU(vert4.x, vert4.y, 0, 255, 255, 0);
				vertices[6 * index + 4] = Vertex_PCU(vert2.x, vert2.y, 0, 255, 255, 127);
				vertices[6 * index + 5] = Vertex_PCU(vert1.x, vert1.y, 0, 255, 255, 127);
			}

			TransformVertexArrayXY3D(300, vertices, 1.0f, m_orientationDegrees + 135.0f, m_position);

			g_theRenderer->CopyCPUToGPU(&vertices, sizeof(Vertex_PCU) * 300, m_gpuThrusterMesh);
		}
	}
	else
	{
		float thetaDegrees = 360.0f / 50.0f;

		Vertex_PCU vertices[300];

		for (int index = 0; index < 12; index++)
		{
			float theta1 = index * thetaDegrees;
			float theta2 = (index + 1) * thetaDegrees;

			Vec2 vert1 = Vec2(2.0f * CosDegrees(theta1) + 0.2f, 2.0f * SinDegrees(theta1) + 0.2f);
			Vec2 vert2 = Vec2(2.0f * CosDegrees(theta2) + 0.2f, 2.0f * SinDegrees(theta2) + 0.2f);
			Vec2 vert3 = Vec2((2.0f + m_thrustPower) * CosDegrees(theta1) + 0.2f, (2.0f + m_thrustPower) * SinDegrees(theta1) + 0.2f);
			Vec2 vert4 = Vec2((2.0f + m_thrustPower) * CosDegrees(theta2) + 0.2f, (2.0f + m_thrustPower) * SinDegrees(theta2) + 0.2f);

			vertices[6 * index] = Vertex_PCU(vert1.x, vert1.y,     0, 255, 255, 127);
			vertices[6 * index + 1] = Vertex_PCU(vert3.x, vert3.y, 0, 255, 255, 0);
			vertices[6 * index + 2] = Vertex_PCU(vert4.x, vert4.y, 0, 255, 255, 0);
			vertices[6 * index + 3] = Vertex_PCU(vert4.x, vert4.y, 0, 255, 255, 0);
			vertices[6 * index + 4] = Vertex_PCU(vert2.x, vert2.y, 0, 255, 255, 127);
			vertices[6 * index + 5] = Vertex_PCU(vert1.x, vert1.y, 0, 255, 255, 127);
		}

		TransformVertexArrayXY3D(300, vertices, 1.0f, m_orientationDegrees + 135.0f, m_position);

		g_theRenderer->CopyCPUToGPU(&vertices, sizeof(Vertex_PCU) * 300, m_gpuThrusterMesh);
	}
}

void PlayerShip::RenderBody() const
{/*
	g_theRenderer->BindTexture();
	g_theRenderer->DrawVertexArray(NUM_OF_VERTICES, shipVerts);*/
}

void PlayerShip::RenderShield() const
{
	DrawDebugRing(m_position, PLAYER_SHIP_PHYSICS_RADIUS, m_orientationDegrees, 0.2f, Rgba8(47, 249, 35, 255));
}

void PlayerShip::RenderWeapons() const
{

	//g_theRenderer->BindTexture();
	//g_theRenderer->DrawVertexArray(NUM_OF_SHIP_WEAPON_VERTICES, shipVerts);
}

void PlayerShip::RenderCockpit() const
{

	//g_theRenderer->BindTexture();
	//g_theRenderer->DrawVertexArray(NUM_OF_COCKPIT_VERTICES, shipVerts);
}

void PlayerShip::Render() const
{
	/*if (m_game->m_isPlayerShieldActive)
	{
		RenderShield();
	}*/

	//RenderBody();
	//RenderCockpit();
	//RenderWeapons();

	g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetDepthMode(DepthMode::ENABLED);
	g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
	g_theRenderer->BindShader();
	g_theRenderer->BindTexture();
	g_theRenderer->DrawVertexBuffer(m_gpuBodyMesh, (NUM_OF_MAIN_BODY_VERTICES + NUM_OF_COCKPIT_VERTICES + NUM_OF_FRONT_VERTICES + NUM_OF_SHIP_WEAPON_VERTICES), sizeof(Vertex_PCU));

	if (m_isShipThrusting || m_game->IsHyperSpace())
	{
		g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE);
		g_theRenderer->SetBlendMode(BlendMode::ALPHA);
		g_theRenderer->SetDepthMode(DepthMode::ENABLED);
		g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
		g_theRenderer->BindShader();
		g_theRenderer->BindTexture();
		g_theRenderer->DrawVertexBuffer(m_gpuThrusterMesh, 300, sizeof(Vertex_PCU));
	}

	if (m_game->IsDeveloperModeOn())
	{
		DrawDebugLine(m_position, Vec2((m_position.x + (m_cosmeticRadius * CosDegrees(m_orientationDegrees))), (m_position.y + (m_cosmeticRadius * SinDegrees(m_orientationDegrees)))), 0.2f, Rgba8(255, 0, 0, 255));
		DrawDebugLine(m_position, Vec2((m_position.x + (m_cosmeticRadius * CosDegrees(90.0f + m_orientationDegrees))), (m_position.y + (m_cosmeticRadius * SinDegrees(90.0f + m_orientationDegrees)))), 0.2f, Rgba8(0, 255, 0, 255));
		DrawDebugRing(m_position, m_cosmeticRadius, m_orientationDegrees, 0.2f, Rgba8(255, 0, 255, 255));
		DrawDebugRing(m_position, m_physicsRadius, m_orientationDegrees, 0.2f, Rgba8(0, 255, 255, 255));
		DrawDebugLine(m_position, Vec2(m_position.x + m_velocity.x, m_position.y + m_velocity.y), 0.2f, Rgba8(255, 255, 0, 255));
	}
}

void PlayerShip::RenderThrusts() const
{

	//g_theRenderer->BindTexture();
	//g_theRenderer->DrawVertexArray(300, vertices);
}

void PlayerShip::Die()
{
	SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Player_Explosion.mp3");
	g_theAudio->StartSound(testSound);

	m_isDead = true;
	m_isGarbage = false;
	RandomNumberGenerator random = RandomNumberGenerator();
	m_game->SpawnDebris(random.RollRandomIntInRange(5, 30), m_position, m_velocity * 0.1f, random.RollRandomFloatInRange(0.2f, 0.8f), m_color);
}
