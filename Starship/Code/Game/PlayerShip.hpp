#pragma once

#include "Game/Entity.hpp"

#include "Engine/Core/Vertex_PCU.hpp"

constexpr int NUM_OF_MAIN_BODY_TRIANGLES = 20;
constexpr int NUM_OF_MAIN_BODY_VERTICES = 3 * NUM_OF_MAIN_BODY_TRIANGLES;

constexpr int NUM_OF_COCKPIT_TRIANGLES = 5;
constexpr int NUM_OF_COCKPIT_VERTICES = 3 * NUM_OF_COCKPIT_TRIANGLES;

constexpr int NUM_OF_FRONT_TRIANGLES = 4;
constexpr int NUM_OF_FRONT_VERTICES = 3 * NUM_OF_FRONT_TRIANGLES;

constexpr int NUM_OF_SHIP_WEAPON_TRIANGLES = 4 + 6;
constexpr int NUM_OF_SHIP_WEAPON_VERTICES = 3 * NUM_OF_SHIP_WEAPON_TRIANGLES;

constexpr int NUM_OF_VERTICES = NUM_OF_MAIN_BODY_VERTICES + NUM_OF_FRONT_VERTICES;

class VertexBuffer;

class PlayerShip : public Entity
{
	Vertex_PCU			m_bodyVertices[NUM_OF_VERTICES]						= {};
	Vertex_PCU			m_cockpitVertices[NUM_OF_COCKPIT_VERTICES]			= {};
	Vertex_PCU			m_weaponVertices[NUM_OF_SHIP_WEAPON_VERTICES]		= {};
	VertexBuffer*		m_gpuBodyMesh										= nullptr;
	VertexBuffer*		m_gpuThrusterMesh									= nullptr;
	bool				m_isShipThrusting									= false;
	bool				m_isTurningLeft										= false;
	bool				m_isTurningRight									= false;
	float				m_thrustFraction									= 1.0f;
	float				m_thrustPower										= 2.0f;
public:
						PlayerShip(Game* owner, Vec2 const& startPos);
						~PlayerShip();

	void				Initialize();
	void				BounceOffWalls();
	void				HandleKeyboardInput();
	void				UpdateThruster(float deltaseconds);
	void				UpdateFromController(float deltaseconds);

	void				RenderBody() const;
	void				RenderShield() const;
	void				RenderWeapons() const;
	void				RenderCockpit() const;
	void				RenderThrusts() const;

	virtual void		Update(float deltaseconds) override;
	virtual void		Render() const override;
	virtual void		Die() override;
};