/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/system.h>
#include <base/math.h>
#include <base/vmath.h>

#include <math.h>
#include <engine/map.h>
#include <engine/kernel.h>

#include <game/mapitems.h>
#include <game/layers.h>
#include <game/collision.h>
#include <stdio.h>

CCollision::CCollision()
{
	m_pTiles = 0;
	m_Width = 0;
	m_Height = 0;
	m_pLayers = 0;
}

void CCollision::Init(class CLayers *pLayers)
{
	m_pLayers = pLayers;
	m_Width = m_pLayers->GameLayer()->m_Width;
	m_Height = m_pLayers->GameLayer()->m_Height;
	m_pTiles = static_cast<CTile *>(m_pLayers->Map()->GetData(m_pLayers->GameLayer()->m_Data));

	for(int i = 0; i < m_Width*m_Height; i++)
	{
		int Index = m_pTiles[i].m_Index;

		if(Index > 128)
			continue;

		switch(Index)
		{
		case TILE_DEATH:
			m_pTiles[i].m_Index = COLFLAG_DEATH;
			break;
		case TILE_SOLID:
			m_pTiles[i].m_Index = COLFLAG_SOLID|COLFLAG_BALL_SOLID;
			break;
		case TILE_NOHOOK:
			m_pTiles[i].m_Index = COLFLAG_SOLID|COLFLAG_NOHOOK|COLFLAG_BALL_SOLID;
			break;
		case TILE_GOAL_TEAM_0:
			m_pTiles[i].m_Index = SFLAG_GOAL_TEAM_0;
			break;
		case TILE_GOAL_TEAM_1:
			m_pTiles[i].m_Index = SFLAG_GOAL_TEAM_1;
			break;
		case TILE_LIMIT_TEAM_0:
			m_pTiles[i].m_Index = SFLAG_LIMIT_TEAM_0;
			break;
		case TILE_LIMIT_TEAM_1:
			m_pTiles[i].m_Index = SFLAG_LIMIT_TEAM_1;
			break;
		case TILE_GOALIE_LIMIT_0:
			m_pTiles[i].m_Index = SFLAG_GOALIE_LIMIT_0;
			break;
		case TILE_GOALIE_LIMIT_1:
			m_pTiles[i].m_Index = SFLAG_GOALIE_LIMIT_1;
			break;
		case TILE_BALL_SOLID:
			m_pTiles[i].m_Index = COLFLAG_BALL_SOLID;
			break;
		case TILE_DEATH_BALL_SOLID:
			m_pTiles[i].m_Index = COLFLAG_BALL_SOLID | COLFLAG_DEATH;
			break;
		default:
			m_pTiles[i].m_Index = 0;
		}
	}
}

int CCollision::GetTile(int x, int y)
{
	int Nx = clamp(x/32, 0, m_Width-1);
	int Ny = clamp(y/32, 0, m_Height-1);

	return m_pTiles[Ny*m_Width+Nx].m_Index > 128 ? 0 : m_pTiles[Ny*m_Width+Nx].m_Index;
}

bool CCollision::IsTileSolid(int x, int y)
{
	return GetTile(x, y)&COLFLAG_SOLID;
}

// TODO: rewrite this smarter!
bool CCollision::IsTileBallEvent(int x, int y)
{
	switch (GetTile(x, y)) {
	case SFLAG_GOAL_TEAM_0:
	case SFLAG_GOAL_TEAM_1:
		return true;
	default:
		return GetTile(x, y)&COLFLAG_BALL_SOLID;
	}
}

int CCollision::IntersectLine(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision, bool ball)
{
	vec2 Cur = Pos0;
	vec2 Dist = Pos1 - Pos0;
	vec2 Last = Pos0;
	int Dir_x;
	int Dir_y;

	if (Dist.x > 0) {
		Dir_x = 1;
	} else {
		Dir_x = -1;
	}
	if (Dist.y > 0) {
		Dir_y = 1;
	} else {
		Dir_y = -1;
	}

	// Move along the line Pos0 -> Pos1 by jumping between tile intersection points
	while ((Pos1.x - Cur.x) * Dir_x >= 0 && (Pos1.y - Cur.y) * Dir_y >= 0) {
		if((ball && BallCheckPoint(Cur.x, Cur.y)) || (!ball && CheckPoint(Cur.x, Cur.y)))
		{
			if(pOutCollision)
				*pOutCollision = Cur;
			if(pOutBeforeCollision)
				*pOutBeforeCollision = Last;
			return GetCollisionAt(Cur.x, Cur.y);
		}


		Last = Cur;
		vec2 next;
		if (Dir_x > 0) {
			next.x = ((int)Cur.x/32 + 1) * 32;
		} else {
			next.x = ((int)Cur.x/32) * 32 - 0.5001;
		}
		if (Dir_y > 0) {
			next.y = ((int)Cur.y/32 + 1) * 32;
		} else {
			next.y = ((int)Cur.y/32) * 32 - 0.5001;
		}
		vec2 rem_dist;
		rem_dist.x = (next.x - Cur.x) / Dist.x;
		rem_dist.y = (next.y - Cur.y) / Dist.y;

		if (rem_dist.x < rem_dist.y) {
			Cur.x += rem_dist.x * Dist.x;
			Cur.y += rem_dist.x * Dist.y;
		} else {
			Cur.x += rem_dist.y * Dist.x;
			Cur.y += rem_dist.y * Dist.y;
		}
	}
	if((ball && BallCheckPoint(Pos1.x, Pos1.y)) || (!ball && CheckPoint(Pos1.x, Pos1.y)))
	{
		if(pOutCollision)
			*pOutCollision = Pos1;
		if(pOutBeforeCollision)
			*pOutBeforeCollision = Last;
		return GetCollisionAt(Pos1.x, Pos1.y);
	}
	
	if(pOutCollision)
		*pOutCollision = Pos1;
	if(pOutBeforeCollision)
		*pOutBeforeCollision = Pos1;
	return 0;
}

// TODO: OPT: rewrite this smarter!
void CCollision::MovePoint(vec2 *pInoutPos, vec2 *pInoutVel, float Elasticity, int *pBounces)
{
	if(pBounces)
		*pBounces = 0;

	vec2 Pos = *pInoutPos;
	vec2 Vel = *pInoutVel;
	if(CheckPoint(Pos + Vel))
	{
		int Affected = 0;
		if(CheckPoint(Pos.x + Vel.x, Pos.y))
		{
			pInoutVel->x *= -Elasticity;
			if(pBounces)
				(*pBounces)++;
			Affected++;
		}

		if(CheckPoint(Pos.x, Pos.y + Vel.y))
		{
			pInoutVel->y *= -Elasticity;
			if(pBounces)
				(*pBounces)++;
			Affected++;
		}

		if(Affected == 0)
		{
			pInoutVel->x *= -Elasticity;
			pInoutVel->y *= -Elasticity;
		}
	}
	else
	{
		*pInoutPos = Pos + Vel;
	}
}

bool CCollision::TestBox(vec2 Pos, vec2 Size)
{
	Size *= 0.5f;
	if(CheckPoint(Pos.x-Size.x, Pos.y-Size.y))
		return true;
	if(CheckPoint(Pos.x+Size.x, Pos.y-Size.y))
		return true;
	if(CheckPoint(Pos.x-Size.x, Pos.y+Size.y))
		return true;
	if(CheckPoint(Pos.x+Size.x, Pos.y+Size.y))
		return true;
	return false;
}

void CCollision::MoveBox(vec2 *pInoutPos, vec2 *pInoutVel, vec2 Size, float Elasticity)
{
	// do the move
	vec2 Pos = *pInoutPos;
	vec2 Vel = *pInoutVel;

	float Distance = length(Vel);
	int Max = (int)Distance;

	if(Distance > 0.00001f)
	{
		//vec2 old_pos = pos;
		float Fraction = 1.0f/(float)(Max+1);
		for(int i = 0; i <= Max; i++)
		{
			//float amount = i/(float)max;
			//if(max == 0)
				//amount = 0;

			vec2 NewPos = Pos + Vel*Fraction; // TODO: this row is not nice

			if(TestBox(vec2(NewPos.x, NewPos.y), Size))
			{
				int Hits = 0;

				if(TestBox(vec2(Pos.x, NewPos.y), Size))
				{
					NewPos.y = Pos.y;
					Vel.y *= -Elasticity;
					Hits++;
				}

				if(TestBox(vec2(NewPos.x, Pos.y), Size))
				{
					NewPos.x = Pos.x;
					Vel.x *= -Elasticity;
					Hits++;
				}

				// neither of the tests got a collision.
				// this is a real _corner case_!
				if(Hits == 0)
				{
					NewPos.y = Pos.y;
					Vel.y *= -Elasticity;
					NewPos.x = Pos.x;
					Vel.x *= -Elasticity;
				}
			}

			Pos = NewPos;
		}
	}

	*pInoutPos = Pos;
	*pInoutVel = Vel;
}
