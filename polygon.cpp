#include "polygon.h"

PolygonShape::PolygonShape(int _n_vertices, const olc::vf2d& size, const olc::vf2d& pos, const olc::Pixel& col, uint32_t _id)
	: n_vertices(_n_vertices), init_color(col), color(col), scale(size), prev_position(pos), position(pos), id(_id) {
	vertices.resize(n_vertices);

	for (int i = 0; i < n_vertices; i++) {
		model.push_back({ cosf(2.0f * PI / n_vertices * i + PI / 4.0f), sinf(2.0f * PI / n_vertices * i + PI / 4.0f) });
	}

	Initialize();
}

bool PolygonShape::IsPointInBounds(const olc::vf2d& point) const {
	// Iterate over all triangles
	for (size_t i = 0; (int)i < (int)(vertices.size() - 2); i++) {
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

olc::vf2d* PolygonShape::GetVertexInBounds(const olc::vf2d& point, float radius) {
	
	for (auto& v : vertices) {
		if ((v.x - point.x) * (v.x - point.x) + (v.y - point.y) * (v.y - point.y) <= radius * radius) return &v;
	}
	
	return nullptr;
}

void PolygonShape::Initialize() {
	float c = cosf(angle), s = sinf(angle);

	for (size_t i = 0; i < model.size(); i++) {

		const olc::vf2d& model_ref = (model[i] * scale);
		// Rotation + Scaling
		vertices[i] = {
			model_ref.x * c - model_ref.y * s,
			model_ref.x * s + model_ref.y * c,
		};

		// Translation
		vertices[i] += position;
	}
}

void PolygonShape::Update(bool force_update) {

	if (!(is_update_shape || force_update)) return;

	float c = cosf(angle - prev_angle), s = sinf(angle - prev_angle);
	for (size_t i = 0; i < vertices.size(); i++) {
		olc::vf2d vertex = {
			(vertices[i].x - position.x) * c - (vertices[i].y - position.y) * s,
			(vertices[i].x - position.x) * s + (vertices[i].y - position.y) * c
		};

		vertices[i] = vertex + position + (position - prev_position);
		//if (scale.mag2() != prev_scale.mag2()) vertices[i] *= (scale - prev_scale);
	}
	prev_angle = angle;
	prev_position = position;
	
	//for (size_t i = 0; i < model.size(); i++) {
	//	
	//	const olc::vf2d& model_ref = (model[i] * scale);
	//	// Rotation + Scaling
	//	vertices[i] = {
	//		model_ref.x * c - model_ref.y * s,
	//		model_ref.x * s + model_ref.y * c,
	//	};

	//	// Translation
	//	vertices[i] += position;
	//}

	is_update_shape = false;
}

void PolygonShape::Draw(olc::PixelGameEngine* pge, const olc::vf2d& offset, bool is_fill) const {
	if (is_fill) {
		for (size_t i = 0; i < vertices.size() - 2; i++) {
			size_t j = (i + 1) % vertices.size();
			size_t k = (i + 2) % vertices.size();

			pge->FillTriangle((olc::vi2d)(vertices[0] - offset), (olc::vi2d)(vertices[j] - offset), (olc::vi2d)(vertices[k] - offset), color);
		}
	}
	else {
		for (size_t i = 0; i < vertices.size() - 1; i++) {
			pge->DrawLine(vertices[i] - offset, vertices[i + 1] - offset, color);
		}
		pge->DrawLine(vertices.back() - offset, vertices[0] - offset, color);
	}
}