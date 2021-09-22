#include "state.h"

EditState::EditState(olc::PixelGameEngine* pge) 
	: State(pge) {

	level_size = { pge->ScreenWidth(), pge->ScreenHeight() };

	polygons.push_back(PolygonShape{ 5, { 100.0f, 100.0f }, level_size / 2.0f, olc::WHITE });
	edit_feature = EditFeature::NONE;

	unit_size = 32;
	uint32_t bg_layer = pge->CreateLayer();
	pge->EnableLayer(bg_layer, true);
	layers.insert({ "bg", { bg_layer, true } });

	// GUI
	button_panel = gui::ButtonPanel{ { 1, 1 }, { pge->ScreenWidth() - 2, 36 }, olc::VERY_DARK_BLUE };
	button_panel.AddButton("ToggleGrid", olc::MAGENTA);
	button_panel.AddButton("ToggleDrawMode", olc::CYAN);

	const olc::vi2d& box_size = { 150, 16 }, panel_size = { 220, 256 };
	box_panel = gui::DragBoxPanel({ 32, 32 }, panel_size, olc::DARK_YELLOW);

	box_panel.AddDragBox("Mass", olc::BLUE, { 0.0f, INFINITY }, box_size, 1.0f);
	box_panel.AddDragBox("e", olc::BLUE, { 0.0f, 1.0f }, box_size, 0.1f);
	box_panel.AddDragBox("sf", olc::BLUE, { 0.0f, 1.0f }, box_size, 0.6f);
	box_panel.AddDragBox("df", olc::BLUE, { 0.0f, 1.0f }, box_size, 0.25f);
}

bool EditState::IsPointInLevel(const olc::vf2d& point) const {
	const olc::vf2d& top_left = { -(float)unit_size, -(float)unit_size };
	const olc::vf2d& bottom_right = olc::vf2d{ (float)level_size.x + unit_size, (float)level_size.y + unit_size };

	return point.x > top_left.x && point.x < bottom_right.x &&
		point.y > top_left.y && point.y < bottom_right.y;
}

void EditState::Scale(const olc::vf2d& m_pos) {
	olc::vf2d offset = m_pos - prev_m_pos;

	const olc::vf2d& screen_pos = ToScreen(selected_shape->position);

	if (press_m_pos.x < screen_pos.x) offset.x *= -1.0f;
	if (press_m_pos.y > screen_pos.y) offset.y *= -1.0f;

	float rotation = std::fmodf(selected_shape->angle, 2.0f * PI);
	if ((rotation > 0.5f * PI && rotation < PI) || (rotation > 1.5f * PI && rotation < 2.0f * PI)) { std::swap(offset.x, offset.y); }

	selected_shape->scale = {
		std::fmaxf(5.0f, selected_shape->scale.x + offset.x),
		std::fmaxf(5.0f, selected_shape->scale.y + -offset.y)
	};
}

void EditState::Rotate(const olc::vf2d& m_pos) {
	const olc::vf2d& s = m_pos - prev_m_pos;
	const olc::vf2d& r = prev_m_pos - ToScreen(selected_shape->position);
	float d_theta_mag = std::sqrtf(s.mag2() / r.mag2());

	selected_shape->angle += (d_theta_mag * r.mag() - s.cross(r)) / r.mag2();
}

void EditState::Translate(const olc::vf2d& m_pos) {

	const olc::vf2d& move_step = olc::vi2d((m_pos - press_m_pos) / unit_size) * (float)unit_size;
	//auto Clamp = [](float value, float a, float b) { return std::fmaxf(a, std::fminf(value, b)); };
	
	if (!IsPointInLevel(selected_shape->position + move_step)) return;
	
	/*olc::vf2d new_pos = selected_shape->position + move_step;

	const olc::vf2d& world_top_left = ToScreen({ 0.0f, 0.0f });
	const olc::vf2d& world_bottom_right = ToScreen((olc::vf2d)level_size);

	new_pos.x = Clamp(new_pos.x, world_top_left.x, world_bottom_right.x);
	new_pos.y = Clamp(new_pos.y, world_top_left.y, world_bottom_right.y);*/

	if (move_step.mag2() > 0.0f) press_m_pos = m_pos;
	selected_shape->position += move_step;
}

void EditState::Input() {
	
	bool is_gui_input = false;

	// GUI
	is_gui_input |= button_panel.Input(pge);
	ButtonFunctions();

	is_gui_input |= box_panel.Input(pge);
	if (is_gui_input) return;
	
	const olc::vf2d& m_pos = (olc::vf2d)pge->GetMousePos();

	auto ToGrid = [&](PolygonShape& poly) -> void {
		poly.position = olc::vi2d(poly.position / unit_size) * (float)unit_size;
	};
	
	// Editing functions
	const olc::vf2d& world_m_pos = ToWorld(m_pos);

	if (pge->GetMouse(0).bPressed) {
		bool is_bounds = false;
		for (auto& poly : polygons) {
			PolygonShape test_poly_s = poly; test_poly_s.scale *= 0.8f; test_poly_s.Update(true);
			PolygonShape test_poly_r = poly; test_poly_r.scale *= 1.2f; test_poly_r.Update(true);

			const bool& is_point_in_bounds = poly.IsPointInBounds(world_m_pos);
			uint8_t is_point_translate = (uint8_t)test_poly_s.IsPointInBounds(world_m_pos);
			uint8_t is_point_scale	   = (uint8_t)((!is_point_translate) & is_point_in_bounds);
			uint8_t is_point_rotate    = (uint8_t)(test_poly_r.IsPointInBounds(world_m_pos));
			
			uint8_t flags = is_point_translate << 0 | is_point_scale << 1 | is_point_rotate << 2;

			if (flags) {
				is_bounds = true;
				if (pge->GetMouse(0).bPressed) {
					selected_shape = &poly;
					press_m_pos = world_m_pos;

					box_panel.is_render = true;
				}
				if (flags & 1) edit_feature = EditFeature::TRANSLATE;
				else if (flags & 2) edit_feature = EditFeature::SCALE;
				else if (flags & 4) edit_feature = EditFeature::ROTATE;
				break;
			}
			else { 
				edit_feature = EditFeature::NONE; 
			}
		}
		if (!is_bounds) selected_shape = nullptr;
	}

	if (selected_shape) {
		selected_shape->is_update_shape = true;
		if (pge->GetMouse(0).bHeld) {
			switch (edit_feature) {
			case EditFeature::SCALE: Scale(m_pos); break;
			case EditFeature::ROTATE: Rotate(m_pos); break;
			case EditFeature::TRANSLATE: Translate(world_m_pos); break;
			}
			ToGrid(*selected_shape);
		}
	}
	else {
		box_panel.is_render = false;
	}

	if (pge->GetMouse(0).bReleased) {
		//selected_shape = nullptr;
		edit_feature = EditFeature::NONE;
	}

	// Adding functions
	if (pge->GetMouse(1).bPressed) {
		PolygonShape poly = PolygonShape{ 3 + rand() % 4, { 25.0f, 25.0f }, world_m_pos, olc::Pixel(rand() % 256, rand() % 256, rand() % 256) };
		polygons.push_back(poly);
	}

	// Panning functions
	if (pge->GetMouse(2).bHeld) {
		offset += -(m_pos - prev_m_pos);
		is_update_layers = true;
	}


	prev_m_pos = m_pos;
}

void EditState::Update() {
	for (auto& p : polygons) p.Update();
}

void EditState::DrawBackground() {
	if (!is_update_layers) return;
	
	pge->SetDrawTarget(layers["bg"].id);
	pge->Clear(olc::BLACK);

	if (layers["bg"].state) {
		for (int i = 0; i < level_size.y / unit_size + 1; i++) {
			pge->DrawLine((olc::vi2d)ToScreen({ 0.0f, (float)i * unit_size }),
				(olc::vi2d)ToScreen({ (float)level_size.x, (float)i * unit_size }), olc::DARK_CYAN);
		}
		for (int i = 0; i < level_size.x / unit_size + 1; i++) {
			pge->DrawLine((olc::vi2d)ToScreen({ (float)i * unit_size, 0.0f }),
				(olc::vi2d)ToScreen({ (float)i * unit_size, (float)level_size.y }), olc::DARK_CYAN);
		}
	}
	else {
		pge->DrawRect((olc::vi2d)ToScreen({ 0.0f, 0.0f }), { pge->ScreenWidth(), pge->ScreenHeight() }, olc::WHITE);
	}

	pge->SetDrawTarget(nullptr);
	is_update_layers = false;
}

void EditState::Draw() {
	for (auto& p : polygons) p.Draw(pge, offset, is_polygon_fill);
	if (selected_shape) {
		pge->FillCircle((olc::vi2d)ToScreen(selected_shape->position), 5, olc::RED);
		//DrawArrow(pge, selected_shape->position, { cosf(selected_shape->angle), sinf(selected_shape->angle) }, 100.0f, 5.0f, olc::BLUE);
		//pge->DrawString(0, 0, "Angle: " + std::to_string(selected_shape->angle));
	}

	const olc::vi2d& m_pos = pge->GetMousePos();

	switch (edit_feature) {
	case EditFeature::SCALE:
		pge->FillCircle(m_pos, 5, olc::BLUE);
		pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Scale", olc::BLUE);
		break;
	case EditFeature::ROTATE:
		pge->FillCircle(m_pos, 5, olc::GREEN);
		pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Rotate", olc::GREEN);
		break;
	case EditFeature::TRANSLATE:
		pge->FillCircle(m_pos, 5, olc::CYAN);
		pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Translate", olc::CYAN);
		break;
	}


	// GUI
	button_panel.Draw(pge);
	if (button_panel("ToggleGrid")->IsPointInBounds(m_pos)) {
		pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Toggle Grid", olc::YELLOW);
	}
	else if (button_panel("ToggleDrawMode")->IsPointInBounds(m_pos)) {
		pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Toggle DrawMode", olc::CYAN);
	}

	box_panel.Draw(pge);
}

void EditState::ButtonFunctions() {
	if (!button_panel.buttons.size()) return;

	if (button_panel("ToggleGrid")->is_pressed) {
		LayerData& data = layers["bg"];
		data.state = !data.state;
		//pge->EnableLayer(data.id, data.state);
		is_update_layers = true;
	}
	else if (button_panel("ToggleDrawMode")->is_pressed) {
		is_polygon_fill = !is_polygon_fill;
	}
}

