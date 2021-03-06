#pragma once
#include "olcPixelGameEngine.h"

static void DrawArrow(olc::PixelGameEngine* pge, const olc::vf2d& point, const olc::vf2d& dir,
	float len, float arrow_len, const olc::Pixel& color = olc::WHITE) {
	const olc::vf2d& p2 = point + len * dir;

	const olc::vf2d& dir_1 =  dir.perp() - dir;
	const olc::vf2d& dir_2 = -dir.perp() - dir;

	pge->DrawLine((olc::vi2d)point, (olc::vi2d)p2, color);
	pge->DrawLine((olc::vi2d)p2, (olc::vi2d)(p2 + dir_1 * arrow_len), color);
	pge->DrawLine((olc::vi2d)p2, (olc::vi2d)(p2 + dir_2 * arrow_len), color);
}

struct PhysicsData {
	float mass = 1.0f;
	float e = 0.2f; // Coefficient of restitution
	float sf = 0.5f, df = 0.3f; // Static and dynamic friction
	float angular_velocity = 0.0f;
};

class PolygonShape {
private:
	std::vector<olc::vf2d> model, vertices;
	float PI = 3.1415926f;
public:
	olc::vf2d position, prev_position, scale;
	olc::Pixel init_color, color;
	float angle = 0.0f, prev_angle = 0.0f;
	int n_vertices = 0;

	PhysicsData properties;
	bool is_update_shape = true;
	uint32_t id = 0;
public:
	PolygonShape() {}
	PolygonShape(int _n_vertices, const olc::vf2d& size, const olc::vf2d& pos, const olc::Pixel& col, uint32_t _id);

	bool IsPointInBounds(const olc::vf2d& point) const;
	olc::vf2d* GetVertexInBounds(const olc::vf2d& point, float radius);

	void Initialize();

	void Update(bool force_update = false);
	void Draw(olc::PixelGameEngine* pge, const olc::vf2d& offset, float scale_zoom, bool is_fill = false) const;

	std::vector<olc::vf2d> GetVertices() const { return vertices; }
	olc::vf2d& GetVertex(size_t index) { return vertices[index]; }
};