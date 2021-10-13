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
		pge->DrawLine((olc::vi2d)(poly->position - offset), (olc::vi2d)(pge->GetMousePos()), olc::DARK_YELLOW);
	}
}

void ConstraintManager::DrawConstraints(olc::PixelGameEngine* pge, const olc::vf2d& offset) {
	for (auto& constraint_pair : constraints_data) {
		pge->DrawLine((olc::vi2d)(constraint_pair.second.first - offset), (olc::vi2d)(constraint_pair.second.second - offset), olc::YELLOW);
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


void JointPairManager::OnMouseRelease(PolygonShape* _polyB) {
	data.push_back({ std::make_pair(poly->id, _polyB->id), std::make_pair(poly->position, _polyB->position) });
	is_press = false;
	poly = nullptr;
}


void JointPairManager::Draw(olc::PixelGameEngine* pge, const olc::vf2d& offset) {
	if (poly) {
		pge->DrawLine((olc::vi2d)(poly->position - offset), (olc::vi2d)(pge->GetMousePos()), olc::DARK_CYAN);
	}
}


void JointPairManager::DrawJointPairs(olc::PixelGameEngine* pge, const olc::vf2d& offset) {
	for (auto& pair : data) {
		pge->DrawLine((olc::vi2d)(pair.positions.first - offset), (olc::vi2d)(pair.positions.second - offset), olc::CYAN);
	}
}

void JointPairManager::ClearJointPairs() {
	data.clear();
	is_press = false;
	poly = nullptr;
}