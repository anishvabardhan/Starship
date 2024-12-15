#pragma once

#include "Game/Entity.hpp"

#include "Engine/Core/Vertex_PCU.hpp"

class Game;
class VertexBuffer;

constexpr int NUM_BOMBER_TRIANGLES = 36;
constexpr int NUM_BOMBER_VERTS = 3 * NUM_BOMBER_TRIANGLES;

constexpr int NUM_OF_TIE_B_WEAPON_TRIANGLES = 4 + 20;
constexpr int NUM_OF_TIE_B_WEAPON_VERTICES = 3 * NUM_OF_TIE_B_WEAPON_TRIANGLES;

class TieBomber : public Entity
{
	Vertex_PCU			m_bodyVertices[NUM_BOMBER_VERTS] = {};
	Vertex_PCU			m_weaponVertices[NUM_OF_TIE_B_WEAPON_VERTICES] = {};
	VertexBuffer*		m_gpuMesh = nullptr;
	Vec2				m_lastShipPosition;
public:
						TieBomber() = default;
	explicit			TieBomber(Game* owner, Vec2 const& position);
						~TieBomber();

	void				Initialize();

	void				RenderBody() const;
	void				RenderWeapons() const;

	virtual void		Update(float deltaseconds) override;
	virtual void		Render() const override;
	virtual void		Die() override;
};