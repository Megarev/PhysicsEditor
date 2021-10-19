#pragma once
#include "physics.h"
#include "polygon.h"


class ConstraintManager {
public:
	bool is_press = false;

	struct Data {
		uint32_t id;
		std::pair<olc::vf2d, olc::vf2d> positions;
		float k = 0.5f, b = 0.2f;
	};

	std::vector<Data> constraints_data;
	PolygonShape* poly = nullptr;

	void OnMousePress(PolygonShape* _poly);
	void OnMouseRelease(const olc::vf2d& world_m_pos, float _k, float _b);

	void Draw(olc::PixelGameEngine* pge, const olc::vf2d& offset, float scale_zoom);
	void DrawConstraints(olc::PixelGameEngine* pge, const olc::vf2d& offset, float scale_zoom);

	void ClearConstraints();
};


class JointPairManager {
public:
	struct DataPair {
		std::pair<uint32_t, uint32_t> ids;
		std::pair<olc::vf2d, olc::vf2d> positions;
		float k = 0.5f, b = 0.2f;
	};

	bool is_press = false;
	std::vector<DataPair> data;
	PolygonShape* poly = nullptr;

	void Reset() { is_press = false; poly = nullptr; }

	void OnMousePress(PolygonShape* _polyA);
	void OnMouseRelease(PolygonShape* _polyB, float _k, float _b);

	void Draw(olc::PixelGameEngine* pge, const olc::vf2d& offset, float scale_zoom);
	void DrawJointPairs(olc::PixelGameEngine* pge, const olc::vf2d& offset, float scale_zoom);

	void ClearJointPairs();
};