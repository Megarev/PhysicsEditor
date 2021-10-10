#include "constraints.h"

void ConstraintManager::OnMousePress(PolygonShape* _poly) {
	poly = _poly;
	is_press = true;
}

void ConstraintManager::OnMouseRelease(olc::PixelGameEngine* pge) {
	const olc::vf2d& m_pos = (olc::vf2d)pge->GetMousePos();
	constraints_data.push_back({ poly->id, { poly->position, m_pos } });
	is_press = false;
	poly = nullptr;
}

void ConstraintManager::Draw(olc::PixelGameEngine* pge) {
	if (poly) {
		pge->DrawLine((olc::vi2d)poly->position, pge->GetMousePos(), olc::WHITE);
	}
}

void ConstraintManager::DrawConstraints(olc::PixelGameEngine* pge) {
	for (auto& constraint_pair : constraints_data) {
		pge->DrawLine(constraint_pair.second.first, constraint_pair.second.second, olc::WHITE);
	}
}
