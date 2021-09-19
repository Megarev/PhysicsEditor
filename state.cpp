#include "state.h"

EditState::EditState(olc::PixelGameEngine* pge) 
	: State(pge) {
	polygons.push_back(PolygonShape{ 5, { 100.0f, 100.0f }, { pge->ScreenWidth() / 2.0f, pge->ScreenHeight() / 2.0f }, olc::WHITE });
	edit_feature = EditFeature::NONE;
}

void EditState::Scale(const olc::vf2d& m_pos) {
	olc::vf2d offset = m_pos - prev_m_pos;

	float c = cosf(selected_shape->angle);
	if (m_pos.x < selected_shape->position.x) offset.x *= -1.0f;
	if (m_pos.y > selected_shape->position.y) offset.y *= -1.0f;

	selected_shape->scale = {
		std::fmaxf(0.1f, selected_shape->scale.x + offset.x),
		std::fmaxf(0.1f, selected_shape->scale.y + -offset.y)
	};
}


void EditState::Rotate(const olc::vf2d& m_pos) {
	const olc::vf2d& s = m_pos - prev_m_pos;
	const olc::vf2d& r = prev_m_pos - selected_shape->position;
	float d_theta_mag = std::sqrtf(s.mag2() / r.mag2());

	selected_shape->angle += (d_theta_mag * r.mag() - s.cross(r)) / r.mag2();
}


void EditState::Translate(const olc::vf2d& m_pos) {
	selected_shape->position += (m_pos - prev_m_pos);
}

void EditState::Input() {
	const olc::vf2d& m_pos = (olc::vf2d)pge->GetMousePos();
	
	// Editing functions
	if (!selected_shape) {
		for (auto& poly : polygons) {
			PolygonShape test_poly_s = poly; test_poly_s.scale *= 0.8f; test_poly_s.Update();
			PolygonShape test_poly_r = poly; test_poly_r.scale *= 1.2f; test_poly_r.Update();

			const bool& is_point_in_bounds = poly.IsPointInBounds(m_pos);
			uint8_t is_point_translate = (uint8_t)test_poly_s.IsPointInBounds(m_pos);
			uint8_t is_point_scale	   = (uint8_t)((!is_point_translate) & is_point_in_bounds);
			uint8_t is_point_rotate    = (uint8_t)(test_poly_r.IsPointInBounds(m_pos));
			
			uint8_t flags = is_point_translate << 0 | is_point_scale << 1 | is_point_rotate << 2;

			if (flags) {
				if (pge->GetMouse(0).bPressed) selected_shape = &poly;
				if (flags & 1) edit_feature = EditFeature::TRANSLATE;
				else if (flags & 2) edit_feature = EditFeature::SCALE;
				else if (flags & 4) edit_feature = EditFeature::ROTATE;
				break;
			}
			else { edit_feature = EditFeature::NONE; }
		}
	}

	if (selected_shape) {
		if (pge->GetMouse(0).bHeld) {
			switch (edit_feature) {
			case EditFeature::SCALE: Scale(m_pos); break;
			case EditFeature::ROTATE: Rotate(m_pos); break;
			case EditFeature::TRANSLATE: Translate(m_pos); break;
			}
		}
	}

	if (pge->GetMouse(0).bReleased) {
		selected_shape = nullptr;
		edit_feature = EditFeature::NONE;
	}

	// Adding functions
	if (pge->GetMouse(1).bPressed) {
		polygons.push_back(PolygonShape{ 3 + rand() % 4, { 25.0f, 25.0f }, m_pos, olc::Pixel(rand() % 256, rand() % 256, rand() % 256) });
	}

	prev_m_pos = m_pos;
}

void EditState::Update() {
	for (auto& p : polygons) p.Update();
}

void EditState::Draw() {
	for (auto& p : polygons) p.Draw(pge);
	if (selected_shape) {
		pge->FillCircle((olc::vi2d)selected_shape->position, 5, olc::RED);
	}

	switch (edit_feature) {
	case EditFeature::SCALE:
		pge->FillCircle(pge->GetMousePos(), 5, olc::BLUE);
		pge->DrawString({ pge->GetMousePos().x, pge->GetMousePos().y + 8 }, "Scale", olc::BLUE);
		break;
	case EditFeature::ROTATE:
		pge->FillCircle(pge->GetMousePos(), 5, olc::GREEN);
		pge->DrawString({ pge->GetMousePos().x, pge->GetMousePos().y + 8 }, "Rotate", olc::GREEN);
		break;
	case EditFeature::TRANSLATE:
		pge->FillCircle(pge->GetMousePos(), 5, olc::CYAN);
		pge->DrawString({ pge->GetMousePos().x, pge->GetMousePos().y + 8 }, "Translate", olc::CYAN);
		break;
	}
}

