#pragma once
#include "physics.h"
#include "polygon.h"

class ConstraintManager {
public:
	bool is_press = false;
	std::vector<std::pair<int, olc::vf2d>> constraints_data;
	PolygonShape* poly = nullptr;

	void OnMousePress(PolygonShape* _poly);
	void OnMouseRelease(olc::PixelGameEngine* pge);

	void Draw(olc::PixelGameEngine* pge);
};