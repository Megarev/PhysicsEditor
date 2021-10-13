#pragma once
#include "physics.h"
#include "polygon.h"

class ConstraintManager {
public:
	bool is_press = false;
	std::vector<std::pair<uint32_t, std::pair<olc::vf2d, olc::vf2d>>> constraints_data;
	PolygonShape* poly = nullptr;

	void OnMousePress(PolygonShape* _poly);
	void OnMouseRelease(const olc::vf2d& world_m_pos);

	void Draw(olc::PixelGameEngine* pge, const olc::vf2d& offset);
	void DrawConstraints(olc::PixelGameEngine* pge, const olc::vf2d& offset);

	void ClearConstraints();
};


class JointPairManager {
public:
	struct DataPair {
		std::pair<uint32_t, uint32_t> ids;
		std::pair<olc::vf2d, olc::vf2d> positions;
	};

	bool is_press = false;
	std::vector<DataPair> data;
	PolygonShape* poly = nullptr;

	void OnMousePress(PolygonShape* _polyA);
	void OnMouseRelease(PolygonShape* _polyB);

	void Draw(olc::PixelGameEngine* pge, const olc::vf2d& offset);
	void DrawJointPairs(olc::PixelGameEngine* pge, const olc::vf2d& offset);

	void ClearJointPairs();
};