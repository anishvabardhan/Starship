#pragma once

#include "Game/Entity.hpp"

#include "Engine/Core/Vertex_PCU.hpp"

class Game;
class VertexBuffer;

constexpr int NUM_FIGHTER_TRIANGLES = 28;
constexpr int NUM_FIGHTER_VERTS = 3 * NUM_FIGHTER_TRIANGLES;

constexpr int NUM_OF_TIE_WEAPON_TRIANGLES = 4 + 20;
constexpr int NUM_OF_TIE_WEAPON_VERTICES = 3 * NUM_OF_TIE_WEAPON_TRIANGLES;

class TieFighter : public Entity
{
	VertexBuffer*		m_gpuMesh = nullptr;
	Vertex_PCU			m_bodyVertices[NUM_FIGHTER_VERTS] = {};
	Vertex_PCU			m_weaponVertices[NUM_OF_TIE_WEAPON_VERTICES] = {};
	Vec2				m_lastShipPosition;
	float				m_bulletDelay = 2.0f;
public:
						TieFighter() = default;
	explicit			TieFighter(Game* owner, Vec2 const& position);
						~TieFighter();

	void				Initialize();

	void				RenderBody() const;
	void				RenderWeapons() const;

	virtual void		Update(float deltaseconds) override;
	virtual void		Render() const override;
	virtual void		Die() override;
};