#include "constraints.h"

void ConstraintManager::OnMousePress(PolygonShape* _poly) {
	poly = _poly;
	is_press = true;
}

void ConstraintManager::OnMouseRelease(const olc::vf2d& world_m_pos) {
	constraints_data.push_back({ poly->id, { poly->position, world_m_pos } });
	is_press = false;
	poly = nullptr;
}

void ConstraintManager::Draw(olc::PixelGameEngine* pge, const olc::vf2d& offset) {
	if (poly) {
		pge->DrawLine((olc::vi2d)(poly->position - offset), (olc::vi2d)(pge->GetMousePos()), olc::WHITE);
	}
}

void ConstraintManager::DrawConstraints(olc::PixelGameEngine* pge, const olc::vf2d& offset) {
	for (auto& constraint_pair : constraints_data) {
		pge->DrawLine((olc::vi2d)(constraint_pair.second.first - offset), (olc::vi2d)(constraint_pair.second.second - offset), olc::WHITE);
	}
}
