#pragma once
#include "olcPixelGameEngine.h"

class PolygonShape {
private:
	std::vector<olc::vf2d> model, vertices;
public:
	olc::vf2d position, scale;
	olc::Pixel color;
	float angle = 0.0f;
	int n_vertices = 0;
public:
	PolygonShape() {}
	PolygonShape(int n_vertices, const olc::vf2d& size, const olc::vf2d& pos, const olc::Pixel& col);

	bool IsPointInBounds(const olc::vf2d& point) const;

	void Update();
	void Draw(olc::PixelGameEngine* pge) const;
};