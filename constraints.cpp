#include "constraints.h"

void ConstraintManager::OnMousePress(PolygonShape* _poly) {
	poly = _poly;
	is_press = true;
}

void ConstraintManager::OnMouseRelease(const olc::vf2d& world_m_pos, float _k, float _b) {

	constraints_data.push_back({ poly->id, { poly->position, world_m_pos }, _k, _b });
	//constraints_data.push_back({ poly->id, { poly->position, world_m_pos } });
	is_press = false;
	poly = nullptr;
}

void ConstraintManager::Draw(olc::PixelGameEngine* pge, const olc::vf2d& offset, float scale_zoom) {
	if (poly) {
		pge->DrawLine((olc::vi2d)(poly->position / scale_zoom - offset), (olc::vi2d)(pge->GetMousePos()), olc::DARK_YELLOW);
	}
}

void ConstraintManager::DrawConstraints(olc::PixelGameEngine* pge, const olc::vf2d& offset, float scale_zoom) {
	for (auto& constraint_pair : constraints_data) {
		pge->DrawLine((olc::vi2d)(constraint_pair.positions.first / scale_zoom - offset), (olc::vi2d)(constraint_pair.positions.second / scale_zoom - offset), olc::YELLOW);
	}
}

void ConstraintManager::ClearConstraints() {
	constraints_data.clear();
	is_press = false;
	poly = nullptr;
}



void JointPairManager::OnMousePress(PolygonShape* _polyA) {
	poly = _polyA;
	is_press = true;
}


void JointPairManager::OnMouseRelease(PolygonShape* _polyB, float _k, float _b) {
	if (!_polyB) return;

	data.push_back({ std::make_pair(poly->id, _polyB->id), std::make_pair(poly->position, _polyB->position), _k, _b });
	is_press = false;
	poly = nullptr;
}


void JointPairManager::Draw(olc::PixelGameEngine* pge, const olc::vf2d& offset, float scale_zoom) {
	if (poly) {
		pge->DrawLine((olc::vi2d)(poly->position / scale_zoom - offset), (olc::vi2d)(pge->GetMousePos()), olc::DARK_CYAN);
	}
}


void JointPairManager::DrawJointPairs(olc::PixelGameEngine* pge, const olc::vf2d& offset, float scale_zoom) {
	for (auto& pair : data) {
		pge->DrawLine((olc::vi2d)(pair.positions.first / scale_zoom - offset), (olc::vi2d)(pair.positions.second / scale_zoom - offset), olc::CYAN);
	}
}

void JointPairManager::ClearJointPairs() {
	data.clear();
	is_press = false;
	poly = nullptr;
}