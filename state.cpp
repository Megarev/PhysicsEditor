#include "state.h"

void EditState::IsRenderGUI(bool state) {
	box_panel.is_render = state;
	color_panel.is_render = state;
}

EditState::EditState(olc::PixelGameEngine* pge)
	: State(pge) {

	unit_size = 32;
	level_size = { (int)(pge->ScreenWidth() / unit_size) * (int)unit_size, (int)(pge->ScreenHeight() / unit_size) * (int)unit_size };

	//polygons.push_back(PolygonShape{ 5, { 100.0f, 100.0f }, (olc::vf2d)level_size / 2.0f, olc::WHITE, id_count++ });
	edit_feature = EditFeature::NONE;
	mode = Mode::POLYGON;

	icon_set.Load("images/iconset.png");

	uint32_t fg_layer = LayerManager::Get().GetLayer("fg");
	//uint32_t fg_layer = pge->CreateLayer();
	pge->EnableLayer(fg_layer, true);
	layers.insert({ "fg", { fg_layer, true, true } });
	
	uint32_t bg_layer = LayerManager::Get().GetLayer("bg");
	//uint32_t bg_layer = pge->CreateLayer();
	pge->EnableLayer(bg_layer, true);
	layers.insert({ "bg", { bg_layer, true, true } });

	// GUI
	const olc::vi2d& button_size = { 36, 36 };
	button_panel = gui::ButtonPanel{ { 1, 1 }, { pge->ScreenWidth() - 2, 36 }, olc::WHITE };
	button_panel.AddButton("Play", olc::GREEN, false, { 0, 0 }, button_size, button_size, icon_set.Decal());
	button_panel.AddButton("ToggleGrid", olc::MAGENTA, false, { button_size.x, 0 }, button_size, button_size, icon_set.Decal());
	button_panel.AddButton("ToggleSnapToGrid", olc::CYAN, true, { 2 * button_size.x, 0 }, button_size, button_size, icon_set.Decal());
	button_panel.AddButton("ToggleDrawMode", olc::YELLOW, true, { 3 * button_size.x, 0 }, button_size, button_size, icon_set.Decal());
	button_panel.AddButton("ToggleMassMode", olc::WHITE, true, { 4 * button_size.x, 0 }, button_size, button_size, icon_set.Decal());
	//button_panel.AddButton("ClearLevel", olc::RED, false, { 5 * button_size.x, 0 }, button_size, button_size, icon_set.Decal());
	button_panel.AddButton("AddConstraint", olc::YELLOW, true, { 6 * button_size.x, 0 }, button_size, button_size, icon_set.Decal());
	button_panel.AddButton("AddJointPair", olc::CYAN, true, { 7 * button_size.x, 0 }, button_size, button_size, icon_set.Decal());
	button_panel.AddButton("ShowHelpBox", olc::WHITE * 0.9f, false, { 8 * button_size.x, 0 }, button_size, button_size, icon_set.Decal());

	clear_button = gui::Button{ { pge->ScreenWidth() - button_size.x + 1, 1 }, button_size, olc::RED, false };
	clear_button.icon_data = { { 5 * button_size.x, 0 }, button_size, icon_set.Decal() };

	const olc::vi2d& box_size = { 150, 16 }, panel_size = { 220, 100 };
	box_panel = gui::DragBoxPanel{ { 32, 32 }, panel_size, olc::VERY_DARK_CYAN, "Properties" };
	box_panel.is_render = false;

	box_panel.AddDragBox("Mass", olc::BLUE, { 0.0f, INFINITY }, box_size, 1.0f);
	box_panel.AddDragBox("e", olc::BLUE, { 0.0f, 1.0f }, box_size, 0.1f);
	box_panel.AddDragBox("sf", olc::BLUE, { 0.0f, 1.0f }, box_size, 0.6f);
	box_panel.AddDragBox("df", olc::BLUE, { 0.0f, 1.0f }, box_size, 0.25f);
	box_panel.AddDragBox("w", olc::BLUE, { -5.0f, 5.0f }, box_size, 0.0f);

	constraints_panel = gui::DragBoxPanel{ { pge->ScreenWidth() - panel_size.x, pge->ScreenHeight() - panel_size.y }, { panel_size.x, panel_size.y - 3 * box_size.y }, olc::VERY_DARK_GREEN, "Settings" };
	constraints_panel.is_render = false;

	const olc::Pixel& ORANGE = olc::Pixel(256, 165, 0);
	constraints_panel.AddDragBox("k", ORANGE, { 0.1f, INFINITY }, box_size, 0.5f);
	constraints_panel.AddDragBox("b", ORANGE, { 0.0f, 1.0f }, box_size, 0.2f);
	constraints_panel.SetDragBoxSpeed("k", 0.05f);
	constraints_panel.SetDragBoxSpeed("b", 0.01f);

	color_panel = gui::ColorPanel({ 32, pge->ScreenHeight() - 200 }, { 170, 170 }, olc::VERY_DARK_YELLOW, "images/color_wheel.png");
	color_panel.is_render = false;

	poly_panel = gui::ListBox{ { 0, 0 }, { 100, 100 }, olc::VERY_DARK_RED, 10 };
	poly_panel.AddItem("Add Hexagon", olc::YELLOW);
	poly_panel.AddItem("Add Pentagon", olc::YELLOW);
	poly_panel.AddItem("Add Square", olc::YELLOW);
	poly_panel.AddItem("Add Triangle", olc::YELLOW);
	poly_panel.AddItem("Add PolyBall", olc::YELLOW);

	text_box = gui::TextBox{};
	help_box = gui::TextPanel{};
}

bool EditState::IsPointInLevel(const olc::vf2d& point) const {
	const olc::vf2d& top_left = { -(float)unit_size, -(float)unit_size };
	const olc::vf2d& bottom_right = olc::vf2d{ (float)level_size.x + unit_size, (float)level_size.y + unit_size };

	return point.x > top_left.x && point.x < bottom_right.x &&
		point.y > top_left.y && point.y < bottom_right.y;
}

/*void EditState::Scale(const olc::vf2d& m_pos) {
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
}*/

void EditState::Rotate(const olc::vf2d& m_pos) {
	const olc::vf2d& s = m_pos - prev_m_pos;
	const olc::vf2d& r = prev_m_pos - ToScreen(selected_shape->position);

	selected_shape->angle += (s.mag() - s.cross(r)) / r.mag2();
}

void EditState::Translate(const olc::vf2d& m_pos) {
	olc::vf2d move_step; 
	if (is_snap_to_grid) move_step = olc::vi2d((m_pos - press_m_pos) / (float)unit_size) * (float)unit_size;
	else move_step = (m_pos - press_m_pos);
	
	if (!IsPointInLevel(selected_shape->position + move_step)) return;

	if (move_step.mag2() > 0.0f) press_m_pos = m_pos;
	selected_shape->position += move_step;

	for (auto& c : constraint_mgr.constraints_data) {
		if (c.id == selected_shape->id) {
			c.positions.first += move_step;
		}
	}

	for (auto& j : joint_mgr.data) {
		if (j.ids.first == selected_shape->id) j.positions.first += move_step;
		else if (j.ids.second == selected_shape->id) j.positions.second += move_step;
	}
}

void EditState::MoveVertex(const olc::vf2d& m_pos) {
	olc::vf2d move_step;
	if (is_snap_to_grid) move_step = olc::vi2d((m_pos - press_m_pos) / (float)unit_size) * (float)unit_size;
	else move_step = (m_pos - press_m_pos);

	auto IsConcavePolygon = [&]() -> bool {
		for (int b = 0; b < selected_shape->n_vertices; b++) {
			int a = (b + selected_shape->n_vertices - 1) % selected_shape->n_vertices;
			int c = (b + 1) % selected_shape->n_vertices;

			const olc::vf2d& ba = selected_shape->GetVertex(a) - selected_shape->GetVertex(b);
			const olc::vf2d& bc = selected_shape->GetVertex(c) - selected_shape->GetVertex(b);
		
			if (ba.cross(bc) > 0.0f) return true;
		}
		return false;
	};

	if (!IsPointInLevel(*selected_vertex + move_step)) return;
	if (move_step.mag2() > 0.0f) press_m_pos = m_pos;
	
	*selected_vertex += move_step;
	if (IsConcavePolygon()) {
		*selected_vertex -= move_step;
	}
}

void EditState::Input() {
	bool is_gui_input = false;
	const olc::vf2d& m_pos = (olc::vf2d)pge->GetMousePos();
	const olc::vf2d& world_m_pos = ToWorld(m_pos);
	
	// GUI
	is_gui_input |= box_panel.Input(pge) | clear_button.Input(pge);
	is_gui_input |= constraints_panel.Input(pge);
	bool is_change_polygon_color = color_panel.Input(pge);
	if (is_change_polygon_color | is_gui_input) layers["fg"].is_update = true;
	
	is_gui_input |= button_panel.Input(pge); ButtonFunctions();
	is_gui_input |= is_change_polygon_color;
	
	// Adding functions
	if (pge->GetMouse(0).bPressed && add_polygon) { OnMousePressAdd(world_m_pos); }

	if (is_helper_box) return;

	if (mode == Mode::POLYGON) {
		is_gui_input |= poly_panel.Input(pge);
		ListBoxFunctions();
	}

	if (is_gui_input && !add_polygon) return;


	if (!add_polygon && pge->GetKey(olc::C).bHeld) {
		if (selected_shape) CopyPolygon(selected_shape->position);
	}

	if (mode == Mode::CONSTRAINTS) {
		if (!constraint_mgr.is_press) {
			if (pge->GetMouse(0).bPressed) {
				for (auto& poly : polygons) {
					if (poly.IsPointInBounds(world_m_pos)) {
						constraint_mgr.OnMousePress(&poly);
						break;
					}
				}
			}
		}
		else {
			if (pge->GetMouse(0).bReleased) {

				float k = constraints_panel("k")->value;
				float b = constraints_panel("b")->value;

				constraint_mgr.OnMouseRelease(world_m_pos, k, b);
				layers["fg"].is_update = true;
			}
		}

		//if (pge->GetMouse(1).bHeld) RemoveConstraint();
	}
	else if (mode == Mode::JOINTPAIR) {
		if (!joint_mgr.is_press) {
			if (pge->GetMouse(0).bPressed) {
				for (auto& poly : polygons) {
					if (poly.IsPointInBounds(world_m_pos)) {
						joint_mgr.OnMousePress(&poly);
						break;
					}
				}
			}
		}
		else {
			if (pge->GetMouse(0).bReleased) {
				bool is_join_polygon = false;
				for (auto& poly : polygons) {
					if (poly.IsPointInBounds(world_m_pos) && joint_mgr.poly != &poly) {

						float k = constraints_panel("k")->value;
						float b = constraints_panel("b")->value;

						joint_mgr.OnMouseRelease(&poly, k, b);
						layers["fg"].is_update = true;
						is_join_polygon = true;
						break;
					}
				}
				if (!is_join_polygon) { 
					joint_mgr.Reset();
				}
			}
		}
	}
	else {

		// Editing functions
		if (!add_polygon) {
			OnMousePressEdit(world_m_pos);
			if (pge->GetMouse(0).bHeld) { OnMouseHoldEdit(m_pos, world_m_pos); }
			if (pge->GetMouse(0).bReleased) { OnMouseReleaseEdit(); }
		}

		if (pge->GetKey(olc::R).bReleased) { RemovePolygon(); }
	}

	// Panning functions
	if (pge->GetMouse(2).bHeld) { PanLevel(m_pos); }

	prev_m_pos = m_pos;
}

void EditState::Update() {
	const olc::vi2d& m_pos = pge->GetMousePos();
	for (auto& p : polygons) p.Update();

	if (selected_shape) {
		selected_shape->properties.mass = box_panel("Mass")->value;
		selected_shape->properties.e = box_panel("e")->value;
		selected_shape->properties.sf = box_panel("sf")->value;
		selected_shape->properties.df = box_panel("df")->value;
		selected_shape->properties.angular_velocity = box_panel("w")->value;

		selected_shape->color = color_panel.color_picker.selected_color;
		selected_shape->init_color = color_panel.color_picker.selected_color;
	}

	if (add_polygon) {
		if (is_snap_to_grid) { add_polygon->position = ToGrid(ToWorld(m_pos)); }
		else { add_polygon->position = ToWorld(m_pos); }
		add_polygon->Update(true);
	}

	// GUI
	/*if (mode != Mode::POLYGON) {
		IsRenderGUI(false);
		constraints_panel.is_render = true;
	}
	else {
		constraints_panel.is_render = false;
		IsRenderGUI(true);
	}*/
}

void EditState::DrawBackground() {
	if (!layers["bg"].is_update) return;
	
	pge->SetDrawTarget(layers["bg"].id);
	pge->Clear(olc::CYAN * 0.1f);

	if (layers["bg"].state) {
		for (int i = 0; i < level_size.y / (int)unit_size + 1; i++) {
			pge->DrawLine((olc::vi2d)ToScreen({ 0.0f, (float)i * unit_size }),
				(olc::vi2d)ToScreen({ (float)level_size.x, (float)i * unit_size }), olc::DARK_CYAN);
		}
		for (int i = 0; i < level_size.x / (int)unit_size + 1; i++) {
			pge->DrawLine((olc::vi2d)ToScreen({ (float)i * unit_size, 0.0f }),
				(olc::vi2d)ToScreen({ (float)i * unit_size, (float)level_size.y }), olc::DARK_CYAN);
		}
	}
	else {
		pge->DrawRect((olc::vi2d)ToScreen({ 0.0f, 0.0f }), { pge->ScreenWidth(), pge->ScreenHeight() }, olc::WHITE);
	}

	pge->SetDrawTarget(nullptr);
	layers["bg"].is_update = false;
}

void EditState::Draw() {

	if (layers["fg"].is_update) {
		pge->SetDrawTarget(layers["fg"].id);
		pge->Clear(olc::BLANK);
		
		for (auto& p : polygons) {
			if (is_mass_mode) p.color = olc::WHITE * (p.properties.mass / mass_m);
			else p.color = p.init_color;
			p.Draw(pge, offset, is_polygon_fill);
		}
		if (selected_shape) {
			pge->FillCircle((olc::vi2d)ToScreen(selected_shape->position), 5, olc::RED);
			//DrawArrow(pge, selected_shape->position, { cosf(selected_shape->angle), sinf(selected_shape->angle) }, 100.0f, 5.0f, olc::BLUE);
			//pge->DrawString(0, 0, "Angle: " + std::to_string(selected_shape->angle));
		
			for (auto& v : selected_shape->GetVertices()) pge->FillCircle((olc::vi2d)ToScreen(v), 5, selected_shape->color * 0.8f);
			if (selected_vertex) pge->FillCircle((olc::vi2d)ToScreen(*selected_vertex), 5, olc::MAGENTA);
		}

		constraint_mgr.DrawConstraints(pge, offset);
		joint_mgr.DrawJointPairs(pge, offset);

		layers["fg"].is_update = false;
		pge->SetDrawTarget(nullptr);
	}


	help_box.is_render = false;
	if (selected_shape) {
		int y_offset = 32;
		help_box.SetPanel({ 0, pge->ScreenHeight() - y_offset }, { pge->ScreenWidth() / 4, y_offset }, olc::WHITE, {
			"C - Copy polygon",
			"R - Remove polygon"
		});
		help_box.SetTitle("");

		help_box.is_render = true;
	}

	constraint_mgr.Draw(pge, offset);
	joint_mgr.Draw(pge, offset);
	
	const olc::vi2d& m_pos = pge->GetMousePos();

	switch (edit_feature) {
	/*case EditFeature::SCALE:
		pge->FillCircle(m_pos, 5, olc::BLUE);
		pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Scale", olc::BLUE);
		break;*/
	case EditFeature::ROTATE:
		pge->FillCircle(m_pos, 5, olc::GREEN);
		pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Rotate", olc::GREEN);
		break;
	case EditFeature::TRANSLATE:
		pge->FillCircle(m_pos, 5, olc::CYAN);
		pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Translate", olc::CYAN);
		break;
	case EditFeature::VERTEX:
		pge->FillCircle(m_pos, 5, olc::MAGENTA);
		pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Move Vertex", olc::YELLOW);
		break;
	}

	auto DrawString = [&](const std::string& text, const olc::Pixel& color, const olc::vi2d& offset = { 0, 0 }) -> void {
		pge->DrawStringDecal({ (float)m_pos.x + offset.x /* - pge->GetTextSize(text).x / 2.0f */, (float)m_pos.y + offset.y + 15 }, text, color);
	};

	auto SetTextBox = [&](const std::string& text, const std::string& button_id, const olc::Pixel& color, const olc::vi2d& size = { 100, 20 }) -> void {
		text_box.SetBox(button_panel(button_id)->position + olc::vi2d{ 0, button_panel(button_id)->size.y + (int)unit_size / 2 }, size, color, text);
		text_box.is_render = true;
	};

	// GUI
	button_panel.DrawSprite(pge);
	clear_button.DrawSprite(pge);

	text_box.is_render = false;
	if (!is_helper_box) {
		if (button_panel("Play")->IsPointInBounds(m_pos)) {
			DrawString("Play", olc::GREEN);
			SetTextBox("Play simulation", "Play", olc::GREEN);
			//pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Play", olc::GREEN);
		}
		if (button_panel("ToggleGrid")->IsPointInBounds(m_pos)) {
			DrawString("Toggle Grid", olc::MAGENTA);
			SetTextBox("Turn grid on/off", "ToggleGrid", olc::Pixel(255, 192, 203), { 100, 32 }); // Pink color
			//pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Toggle Grid", olc::YELLOW);
		}
		else if (button_panel("ToggleDrawMode")->IsPointInBounds(m_pos)) {
			DrawString("Toggle DrawMode", olc::YELLOW);
			SetTextBox("Make polygons Outlined/Filled", "ToggleDrawMode", olc::YELLOW, { 105, 32 });
			//pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Toggle DrawMode", olc::CYAN);
		}
		else if (button_panel("ToggleSnapToGrid")->IsPointInBounds(m_pos)) {
			DrawString("Toggle SnapToGrid", olc::CYAN);
			SetTextBox("Grid-based movement", "ToggleSnapToGrid", olc::Pixel(135, 206, 236), { 100, 32 }); // Pink color
			//pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Snap to Grid", olc::MAGENTA);
		}
		else if (button_panel("ToggleMassMode")->IsPointInBounds(m_pos)) {
			DrawString("Toggle MassMode", olc::WHITE);
			SetTextBox("Brightness indicates the polygon's heaviness", "ToggleMassMode", olc::WHITE, { 100, 56 });
			//pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Toggle mass view", olc::WHITE);
		}
		//else if (button_panel("ClearLevel")->IsPointInBounds(m_pos)) {
		//	DrawString("Clear Scene", olc::RED);
		//	SetTextBox("Clears the contents of the scene", "ClearLevel", olc::Pixel(255, 193, 143), { 105, 45 }); // Red violet color
		//	//pge->DrawString({ m_pos.x, m_pos.y + 8 }, "Toggle mass view", olc::WHITE);
		//}
		else if (button_panel("AddConstraint")->IsPointInBounds(m_pos)) {
			DrawString("Contraints Mode", olc::WHITE);
			SetTextBox("Click and drag a polygon to make rope", "AddConstraint", olc::YELLOW, { 105, 45 });
		}
		else if (button_panel("AddJointPair")->IsPointInBounds(m_pos)) {
			DrawString("PolygonPair mode", olc::YELLOW);
			SetTextBox("Click and drag between two polygons to connect them", "AddJointPair", olc::CYAN, { 100, 56 });
		}
		else if (button_panel("ShowHelpBox")->IsPointInBounds(m_pos)) {
			DrawString("Help", olc::WHITE);
		}
		else if (clear_button.IsPointInBounds(m_pos)) {
			const std::string& text = "Clear Scene";
			DrawString(text, olc::MAGENTA, { -(pge->GetTextSizeProp(text).x + 8), 0 });
			text_box.SetBox(clear_button.position + olc::vi2d{ -clear_button.size.x * 2, clear_button.size.y + (int)unit_size / 2 }, { 105, 45 },
				olc::Pixel(255, 193, 143), "Clears the contents of the scene"); // Red violet color
			text_box.is_render = true;
			//SetTextBox("Clears the contents of the scene", "ClearLevel", olc::Pixel(255, 193, 143), { 105, 45 }); // Red violet color
		}
	}

	text_box.Draw(pge);

	box_panel.Draw(pge);
	constraints_panel.Draw(pge);

	color_panel.Draw(pge);
	poly_panel.Draw(pge);

	// Adding functions
	if (add_polygon) add_polygon->Draw(pge, offset, is_polygon_fill);

	//const olc::vi2d& offset = { 40, 40 };
	//help_box.SetPanel(offset, { pge->ScreenWidth() - 2 * offset.x, pge->ScreenHeight() - 2 * offset.y }, olc::YELLOW, {
	//	"Right mouse - Open up the polygons panel",
	//	"Middle mouse - To pan the scene",
	//	//"R - remove the selected polygon",
	//	//"Ctrl + C - copy a polygon"
	//}, 2);
	//help_box.SetTitle("Instructions");

	if (is_helper_box) {
		SetHelpBox(0);
		help_box.is_render = true;
	}
	help_box.Draw(pge);
}

void EditState::Initialize() {
	uint32_t id_pos = 0;
	for (auto& poly : polygons) { id_pos = std::fmaxf(id_pos, poly.id); }
	id_count = id_pos + 1;
}

void EditState::PanLevel(const olc::vf2d& m_pos) {
	offset += -(m_pos - prev_m_pos);
	layers["bg"].is_update = true;
	layers["fg"].is_update = true;
}

void EditState::ButtonFunctions() {
	if (!button_panel.buttons.size()) return;

	if (button_panel("ShowHelpBox")->is_pressed) {
		is_helper_box = !is_helper_box;
	}

	if (button_panel("Play")->is_pressed) {
		ChangeState(States::PLAY);
		color_panel.Clear();
		box_panel.Clear();
		button_panel.Clear();
		is_mass_mode = false;

		for (auto& poly : polygons) poly.color = poly.init_color;

		for (auto& layer : layers) {
			pge->EnableLayer(layer.second.id, false);
		}
	}
	else if (button_panel("ToggleGrid")->is_pressed) {
		LayerData& data = layers["bg"];
		data.state = !data.state;
		data.is_update = true;
	}
	else if (button_panel("ToggleDrawMode")->is_pressed) {
		is_polygon_fill = !is_polygon_fill;
	}
	else if (button_panel("ToggleSnapToGrid")->is_pressed) {
		is_snap_to_grid = !is_snap_to_grid;

		if (is_snap_to_grid) {
			for (auto& poly : polygons) {
				poly.position = ToGrid(poly.position);
			}
		}
	}
	else if (button_panel("ToggleMassMode")->is_pressed) {
		is_mass_mode = !is_mass_mode;
	}
	/*else if (button_panel("ClearLevel")->is_pressed) {
		offset = { 0.0f, 0.0f };
		polygons.clear();
		constraint_mgr.ClearConstraints();
		joint_mgr.ClearJointPairs();

		selected_shape = nullptr;
		selected_vertex = nullptr;
		IsRenderGUI(false);
	}*/
	else if (button_panel("AddConstraint")->is_pressed) {
		mode = mode == Mode::CONSTRAINTS ? Mode::POLYGON : Mode::CONSTRAINTS;
		constraints_panel.is_render = (mode == Mode::CONSTRAINTS);
		IsRenderGUI(false);
		//is_add_constraints = !is_add_constraints;
		button_panel("AddJointPair")->is_toggle_state = false;
		//is_add_joint_pair = false;
	}
	else if (button_panel("AddJointPair")->is_pressed) {
		mode = mode == Mode::JOINTPAIR ? Mode::POLYGON : Mode::JOINTPAIR;
		constraints_panel.is_render = (mode == Mode::JOINTPAIR);
		IsRenderGUI(false);
		button_panel("AddConstraint")->is_toggle_state = false;
		//is_add_constraints = false;
	}
	else if (clear_button.is_pressed) {
		offset = { 0.0f, 0.0f };
		polygons.clear();
		constraint_mgr.ClearConstraints();
		joint_mgr.ClearJointPairs();

		selected_shape = nullptr;
		selected_vertex = nullptr;
		IsRenderGUI(false);
		layers["fg"].is_update = true;
	}
}

void EditState::ListBoxFunctions() {
	
	if (!poly_panel.is_render) return;

	const olc::vi2d& m_pos = pge->GetMousePos();

	if (poly_panel("Add Triangle")->is_pressed) {
		if (add_polygon) delete add_polygon;
		add_polygon = new PolygonShape{ 3, { (float)unit_size, (float)unit_size }, m_pos, olc::WHITE * 0.8f, id_count++ };
		add_polygon->Update(true);
		poly_panel.is_render = false;
	}
	else if (poly_panel("Add Square")->is_pressed) {
		if (add_polygon) delete add_polygon;
		add_polygon = new PolygonShape{ 4, { (float)unit_size, (float)unit_size }, m_pos, olc::WHITE * 0.8f, id_count++ };
		add_polygon->Update(true);
		poly_panel.is_render = false;
	} 
	else if (poly_panel("Add Pentagon")->is_pressed) {
		if (add_polygon) delete add_polygon;
		add_polygon = new PolygonShape{ 5, { (float)unit_size, (float)unit_size }, m_pos, olc::WHITE * 0.8f, id_count++ };
		add_polygon->Update(true);
		poly_panel.is_render = false;
	}
	else if (poly_panel("Add Hexagon")->is_pressed) {
		if (add_polygon) delete add_polygon;
		add_polygon = new PolygonShape{ 6, { (float)unit_size, (float)unit_size }, m_pos, olc::WHITE * 0.8f, id_count++ };
		add_polygon->Update(true);
		poly_panel.is_render = false;
	}
	else if (poly_panel("Add PolyBall")->is_pressed) {
		if (add_polygon) delete add_polygon;
		add_polygon = new PolygonShape{ 10, { (float)unit_size, (float)unit_size }, m_pos, olc::WHITE * 0.8f, id_count++ };
		add_polygon->Update(true);
		poly_panel.is_render = false;
	}
}

void EditState::OnMousePressEdit(const olc::vf2d& world_m_pos) {
	bool is_bounds = false;

	auto ScalePolygon = [](float scaling_factor, const PolygonShape& ref_poly) -> PolygonShape {
		PolygonShape poly = ref_poly;
		for (int i = 0; i < poly.n_vertices; i++) {
			poly.GetVertex(i) += (poly.GetVertex(i) - poly.position) * scaling_factor;
		}
		return poly;
	};

	for (auto& poly : polygons) {
		if (poly.IsPointInBounds(world_m_pos) && pge->GetMouse(0).bPressed) {
			selected_shape = &poly;

			selected_vertex = nullptr;

			box_panel("Mass")->value = selected_shape->properties.mass;
			box_panel("e")->value = selected_shape->properties.e;
			box_panel("sf")->value = selected_shape->properties.sf;
			box_panel("df")->value = selected_shape->properties.df;
			box_panel("w")->value = selected_shape->properties.angular_velocity;

			color_panel.color_picker.selected_color = selected_shape->color;

			box_panel.is_render = true;
			color_panel.is_render = true;

			break;
		}
	}

	if (!is_feature) {
		if (selected_shape) {
			const PolygonShape& test_poly_s = ScalePolygon(-0.2f, *selected_shape);
			const PolygonShape& test_poly_r = ScalePolygon(+0.2f, *selected_shape);

			const bool& is_point_in_bounds = selected_shape->IsPointInBounds(world_m_pos);
			uint8_t is_point_translate = (uint8_t)test_poly_s.IsPointInBounds(world_m_pos);
			uint8_t is_point_scale = (uint8_t)((!is_point_translate) & is_point_in_bounds);
			uint8_t is_point_rotate = (uint8_t)(test_poly_r.IsPointInBounds(world_m_pos));

			uint8_t flags = is_point_translate << 0 | is_point_scale << 1 | is_point_rotate << 2;

			if (flags) {
				is_bounds = true;
				
				if (pge->GetMouse(0).bHeld) {
					is_feature = true;
					press_m_pos = world_m_pos;
				}

				if (flags & 1) edit_feature = EditFeature::TRANSLATE;
				else if (flags & 2) edit_feature = EditFeature::SCALE;
				else if (flags & 4) edit_feature = EditFeature::ROTATE;

				if (selected_shape) {
					olc::vf2d* vertex = selected_shape->GetVertexInBounds(world_m_pos, 5.0f);
					if (vertex) {
						selected_vertex = vertex;
						edit_feature = EditFeature::VERTEX;
					}
				}
			}
			else { edit_feature = EditFeature::NONE; }
		}
		if (!is_bounds && pge->GetMouse(0).bPressed) {
			selected_shape = nullptr;
		}
	}
}

void EditState::OnMouseHoldEdit(const olc::vf2d& m_pos, const olc::vf2d& world_m_pos) {

	auto ToGridPolygon = [&](PolygonShape& poly) -> void { poly.position = ToGrid(poly.position); };

	layers["fg"].is_update = true;

	if (selected_shape) {
		selected_shape->is_update_shape = true;
		switch (edit_feature) {
		//case EditFeature::SCALE: Scale(m_pos); break;
		case EditFeature::ROTATE: Rotate(m_pos); break;
		case EditFeature::TRANSLATE: Translate(world_m_pos); break;
		case EditFeature::VERTEX: MoveVertex(world_m_pos); break;
		}
		if (is_snap_to_grid) ToGridPolygon(*selected_shape);
	}
	else { 
		IsRenderGUI(false);
	}
}

void EditState::OnMouseReleaseEdit() {
	edit_feature = EditFeature::NONE;
	is_feature = false;
}

void EditState::OnMousePressAdd(const olc::vf2d& world_m_pos) {
	polygons.push_back(PolygonShape{ add_polygon->n_vertices, add_polygon->scale, add_polygon->position, add_polygon->color, add_polygon->id });
	for (size_t i = 0; i < (size_t)add_polygon->n_vertices; i++) polygons.back().GetVertex(i) = add_polygon->GetVertex(i);
	polygons.back().properties = add_polygon->properties;
	
	layers["fg"].is_update = true;
	polygons.back().Update(true);
	
	delete add_polygon;
	add_polygon = nullptr;

	poly_panel.is_pressed = false;
}

void EditState::RemovePolygon() {
	if (!selected_shape) return;

	for (int i = polygons.size() - 1; i >= 0; i--) {
		if (polygons[i].id == selected_shape->id) {
			selected_shape = nullptr;

			IsRenderGUI(false);
			layers["fg"].is_update = true;

			auto& mgr_data = constraint_mgr.constraints_data;

			for (int j = (int)mgr_data.size() - 1; j >= 0; j--) {
				if (mgr_data[j].id == polygons[i].id) {
					mgr_data.erase(mgr_data.begin() + j);
				}
			}

			for (int j = (int)joint_mgr.data.size() - 1; j >= 0; j--) {
				if (joint_mgr.data[j].ids.first == polygons[i].id || joint_mgr.data[j].ids.second == polygons[i].id) {
					joint_mgr.data.erase(joint_mgr.data.begin() + j);
				}
			}

			edit_feature = EditFeature::NONE;

			polygons.erase(polygons.begin() + i);
			break;
		}
	}
}

void EditState::RemoveConstraint() {

	auto& mgr_data = constraint_mgr.constraints_data;

	for (int i = (int)mgr_data.size() - 1; i >= 0; i--) {
		const olc::vf2d& p1 = mgr_data[i].positions.first;
		const olc::vf2d& p2 = mgr_data[i].positions.second;

		const olc::vf2d& p3 = prev_m_pos;
		const olc::vf2d& p4 = (olc::vf2d)pge->GetMousePos();

		float d = (p1 - p2).cross(p3 - p4);
		if (d) {
			float t = (p1 - p3).cross(p3 - p4) / d;
			float u = (p1 - p3).cross(p1 - p2) / d;

			if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
				mgr_data.erase(mgr_data.begin() + i);
			}
		}
	}
}

void EditState::CopyPolygon(const olc::vf2d& pos) {
	if (add_polygon) delete add_polygon;
	add_polygon = new PolygonShape(selected_shape->n_vertices, selected_shape->scale, is_snap_to_grid ? ToGrid(pos) : (olc::vi2d)pos, selected_shape->color, id_count++);
	add_polygon->properties = selected_shape->properties;
	for (int i = 0; i < add_polygon->n_vertices; i++) {
		add_polygon->GetVertex(i) = selected_shape->GetVertex(i);
	}
	add_polygon->Update(true);
}

void EditState::SetHelpBox(int n_slide) {
	switch (n_slide) {
	case 0:
		olc::vi2d offset = { 40, 40 };
		
		help_box.SetPanel(offset, { pge->ScreenWidth() - 2 * offset.x, pge->ScreenHeight() - 2 * offset.y }, olc::CYAN, {
			"Mass: Heaviness of polygons",
			"e: Restitution/bounciness",
			"sf: Static friction",
			"df: Dynamic friction",
			"w: Angular velocity",
			"k: Spring constant/stiffness (rope)",
			"b: Damping constant/drag (rope)"
		}, 2);
		help_box.SetTitle("Physics Info");
		break;
	}
}


void EditState::OnWindowUpdate() {
	layers["bg"].is_update = true;
	layers["fg"].is_update = true;
}

PlayState::PlayState(olc::PixelGameEngine* pge)
	: State(pge) {

	icon_set.Load("images/pause_button.png");

	const olc::vi2d& button_size = { 36, 36 };

	edit_button = gui::Button{ { 1, 1 }, button_size, olc::WHITE };
	edit_button.icon_data.spritesheet_ptr = icon_set.Decal();
	edit_button.icon_data.source_pos = { 0, 0 };
	edit_button.icon_data.source_size = button_size;

	scene.Initialize(olc::vf2d{ (float)pge->ScreenWidth(), (float)pge->ScreenHeight() });
}

void PlayState::Initialize() {

	std::unordered_map<int, int> polygon_id;

	for (auto& p : polygons) {
		RigidBody rb(p.position, p.GetVertices(), p.angle, p.properties.angular_velocity, p.properties.mass, p.properties.e, p.properties.sf, p.properties.df, p.id);
		rb.SetColor(p.color);
		
		int id = scene.AddShape(rb);
	
		polygon_id.insert({ p.id, id });
	}

	int n_rope_segments = 5;
	for (auto& data : constraint_mgr.constraints_data) {
		Constraint c(data.positions.second, (data.positions.second - data.positions.first).mag(), data.k, data.b, n_rope_segments);
		c.Attach(polygon_id[data.id]);
		scene.AddConstraint(c);
	}

	for (auto& data : joint_mgr.data) {
		JointPair j(std::make_pair(polygon_id[data.ids.first], polygon_id[data.ids.second]), 
			(data.positions.second - data.positions.first).mag(), data.k, data.b, n_rope_segments, false);
		scene.AddJointPair(j);
	}
}

void PlayState::Input() {
	edit_button.Input(pge);
}

void PlayState::Update() {
	if (edit_button.is_pressed) {
		ChangeState(States::EDIT);
		return;
	}

	const olc::vf2d& m_pos = (olc::vf2d)pge->GetMousePos();

	int n_iter = 5;
	float p = 0.2f;
	for (int i = 0; i < n_iter; i++) scene.Update(pge->GetElapsedTime() / (p * n_iter));
	if (pge->GetMouse(2).bHeld) {
		offset += -(m_pos - prev_m_pos);
	}

	prev_m_pos = m_pos;
}


void PlayState::Draw() {

	scene.Draw(pge, offset /* + olc::vf2d{pge->ScreenWidth() * 0.5f, pge->ScreenHeight() * 0.5f} */, is_polygon_fill);

	// GUI
	edit_button.DrawSprite(pge);
}

void PlayState::DrawBackground() {
	pge->Clear(olc::Pixel(20, 20, 20));
}
