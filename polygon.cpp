#include "polygon.h"

PolygonShape::PolygonShape(int _n_vertices, const olc::vf2d& size, const olc::vf2d& pos, const olc::Pixel& col)
	: n_vertices(_n_vertices), color(col), scale(size), position(pos) {
	vertices.resize(n_vertices);
	for (int i = 0; i < n_vertices; i++) {
		model.push_back({ cosf(2.0f * 3.1415926f / n_vertices * i), sinf(2.0f * 3.1415926f / n_vertices * i) });
	}
}

bool PolygonShape::IsPointInBounds(const olc::vf2d& point) const {
	// Iterate over all triangles
	for (size_t i = 0; i < vertices.size() - 2; i++) {
		size_t j = (i + 1) % vertices.size();
		size_t k = (i + 2) % vertices.size();

		const olc::vf2d& ab = vertices[j] - vertices[0];
		const olc::vf2d& bc = vertices[k] - vertices[j];
		const olc::vf2d& ca = vertices[0] - vertices[k];
	
		const olc::vf2d& pa = point - vertices[0];
		const olc::vf2d& pb = point - vertices[j];
		const olc::vf2d& pc = point - vertices[k];

		float t1 = ab.cross(-pa), t2 = bc.cross(-pb), t3 = ca.cross(-pc);
		if (t1 < 0.0f && t2 < 0.0f && t3 < 0.0f) return true;

	}
	return false;
}

void PolygonShape::Update() {

	for (size_t i = 0; i < model.size(); i++) {
		
		const olc::vf2d& model_ref = (model[i] * scale);
		
		// Rotation + Scaling
		vertices[i] = { 
			model_ref.x * cosf(angle) - model_ref.y * sinf(angle),
			model_ref.x * sinf(angle) + model_ref.y * cosf(angle),
		};

		// Translation
		vertices[i] += position;
	}
}

void PolygonShape::Draw(olc::PixelGameEngine* pge, bool is_fill) const {
	if (is_fill) {
		for (size_t i = 0; i < vertices.size() - 2; i++) {
			size_t j = (i + 1) % vertices.size();
			size_t k = (i + 2) % vertices.size();

			pge->FillTriangle((olc::vi2d)vertices[0], (olc::vi2d)vertices[j], (olc::vi2d)vertices[k], color);
		}
	}
	else {
		for (size_t i = 0; i < vertices.size() - 1; i++) {
			pge->DrawLine(vertices[i], vertices[i + 1], color);
		}
		pge->DrawLine(vertices.back(), vertices[0], color);
	}
}