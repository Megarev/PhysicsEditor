#include "gui.h"

bool gui::BoxUIBase::IsPointInBounds(const olc::vf2d& point) const {
	return point.x > position.x && point.x < position.x + size.x &&
		point.y > position.y && point.y < position.y + size.y;
}



gui::Button::Button(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, bool is_toggleable) 
	: BoxUIBase(_position, _size, _color), is_button_toggleable(is_toggleable) {}

bool gui::Button::Input(olc::PixelGameEngine* pge) {

	const olc::vf2d& m_pos = (olc::vf2d)pge->GetMousePos();

	color = init_color * 0.25f;
	if (is_toggle_state && is_button_toggleable) color = init_color * 0.1f;
	is_pressed = false;
	
	if (IsPointInBounds(m_pos)) {
		if (!is_toggle_state && !is_button_toggleable) color = init_color * 0.5f;
		if (pge->GetMouse(0).bPressed) {
			if (!is_toggle_state && !is_button_toggleable) color = init_color * 0.9f;
			is_pressed = true;
			if (is_button_toggleable) is_toggle_state = !is_toggle_state;
		}
	}

	return is_pressed;
}

void gui::Button::Draw(olc::PixelGameEngine* pge) {
	pge->FillRect(position, size, color);
	pge->DrawRect(position, size, olc::WHITE);
}

void gui::Button::DrawSprite(olc::PixelGameEngine* pge) {
	
	Draw(pge);
	//pge->DrawPartialSprite(position, icon_data.spritesheet_ptr, icon_data.source_pos, icon_data.source_size);
	if (icon_data.spritesheet_ptr) pge->DrawPartialDecal(position, size, icon_data.spritesheet_ptr, icon_data.source_pos, icon_data.source_size, color);
}



gui::ButtonPanel::ButtonPanel(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color)
	: BoxUIBase(_position, _size, _color) {}

void gui::ButtonPanel::AddButton(const std::string& name, const olc::Pixel& button_color,
	bool is_toggleable, const olc::vi2d& source_pos,
	const olc::vi2d& source_size, const olc::vi2d& button_size, olc::Decal* decal) {
	
	Button button{ position + (int)buttons.size() * olc::vi2d{ button_size.x, 0 }, button_size, button_color, is_toggleable };
	button.icon_data.source_pos = source_pos;
	button.icon_data.source_size = source_size;
	button.icon_data.spritesheet_ptr = decal;
	buttons.insert({ name, button });
}

bool gui::ButtonPanel::Input(olc::PixelGameEngine* pge) {
	bool is_press = false;
	for (auto& button : buttons) is_press |= button.second.Input(pge);
	return is_press;
}

void gui::ButtonPanel::Draw(olc::PixelGameEngine* pge) {
	pge->FillRect(position, size, olc::VERY_DARK_CYAN);
	pge->DrawRect(position, size, color);
	for (auto& button : buttons) button.second.Draw(pge);
}

void gui::ButtonPanel::DrawSprite(olc::PixelGameEngine* pge) {
	for (auto& button : buttons) button.second.DrawSprite(pge);
}

gui::Button* gui::ButtonPanel::operator()(const std::string& name) {

	auto button = buttons.find(name);
	if (button != buttons.end()) return &button->second;

	return nullptr;
}



gui::DragBox::DragBox(const olc::vi2d& _position, const olc::vf2d& _size, const olc::Pixel& _color, float start_value)
	: BoxUIBase(_position, _size, _color), value(start_value), reset_button(_position + olc::vi2d{ (int)size.x + 2, 0 }, { (int)_size.y, (int)_size.y }, color) {}

bool gui::DragBox::Input(olc::PixelGameEngine* pge) {
	const olc::vi2d& m_pos = pge->GetMousePos();
	int dir = m_pos.x > position.x + size.x / 2 ? 1 : -1;

	reset_button.Input(pge);
	//reset_button.position = position + olc::vi2d{ size.x, 0 };

	color = init_color * 0.25f;
	if (IsPointInBounds(m_pos)) {
		color = init_color * 0.75f;
		
		if (pge->GetMouse(0).bPressed) is_pressed = true;
	}

	if (pge->GetMouse(0).bHeld && is_pressed) {
		color = init_color;
		value = std::fmaxf(value_constraints.first, std::fminf(value_constraints.second, value + speed * dir));
	}

	if (pge->GetMouse(0).bReleased) is_pressed = false;

	prev_m_pos = m_pos;

	if (!is_pressed && reset_button.is_pressed) value = 0.0f;

	return is_pressed;

}

void gui::DragBox::Draw(olc::PixelGameEngine* pge) {
	pge->FillRect(position, size, color);
	pge->DrawRect(position, size, olc::WHITE);

	const std::string& value_str = std::to_string(value);
	const olc::vi2d& text_size = pge->GetTextSizeProp(value_str);
	int text_scale = size.y / (2 * text_size.y);

	pge->DrawStringProp(position + olc::vi2d{ (int)((size.x - text_size.x) / 2), (int)(3 * size.y / text_size.y) }, value_str, olc::WHITE, text_scale > 2 ? text_scale : 1);

	reset_button.Draw(pge);
	pge->DrawStringProp(reset_button.position + olc::vi2d{ (int)((reset_button.size.x - pge->GetTextSizeProp("0").x) / 2), (int)(3 * size.y / text_size.y)}, "0", olc::WHITE, text_scale > 2 ? text_scale : 1);
}



gui::DragBoxPanel::DragBoxPanel(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, const std::string& _title) 
	: BoxUIBase(_position, _size, _color), title(_title) {}

void gui::DragBoxPanel::AddDragBox(const std::string& name, const olc::Pixel& box_color, const std::pair<float, float>& _value_constraints, const olc::vi2d& box_size, float start_value) {
	
	int offset = 50;
	
	DragBox box{ { position.x + offset, position.y + (int)drag_boxes.size() * box_size.y + 20 }, box_size, box_color, start_value };
	box.value_constraints = _value_constraints;
	drag_boxes.insert({ name, box });
}

bool gui::DragBoxPanel::IsPointInBounds(const olc::vf2d& point) const {
	return point.x > position.x && point.x < position.x + size.x &&
		point.y > position.y && point.y < position.y + size.y;
}

bool gui::DragBoxPanel::Input(olc::PixelGameEngine* pge) {
	for (auto& box : drag_boxes) box.second.Input(pge);
	const olc::vi2d& m_pos = pge->GetMousePos();

	if (pge->GetMouse(0).bPressed) {
		if (IsPointInBounds((olc::vf2d)m_pos)) {
			is_pressed = true;
			is_render = true;
		}
	}

	if (pge->GetMouse(0).bHeld && is_pressed) {
		position += (m_pos - prev_m_pos);
		for (auto& box : drag_boxes) {
			box.second.position += (m_pos - prev_m_pos);
			box.second.reset_button.position += (m_pos - prev_m_pos);
		}
	}

	if (pge->GetMouse(0).bReleased) {
		is_pressed = false;
	}

	prev_m_pos = m_pos;
	return is_pressed;
}

void gui::DragBoxPanel::Draw(olc::PixelGameEngine* pge) {
	if (is_render) {

		pge->FillRect(position, size, olc::VERY_DARK_BLUE);
		pge->DrawRect(position, size, olc::WHITE);

		const olc::vi2d& title_size = pge->GetTextSizeProp(title);

		pge->DrawStringProp({ position.x + (size.x - title_scale * title_size.x) / 2, position.y }, title, olc::WHITE, title_scale);
		pge->DrawLine({ position.x, position.y + title_scale * title_size.y }, { position.x + size.x, position.y + title_scale * title_size.y }, olc::WHITE);

		for (auto& box : drag_boxes) {
			const olc::vi2d& text_size = pge->GetTextSizeProp(box.first);
			
			pge->DrawStringProp({ box.second.position.x - text_size.x, box.second.position.y + (3 * box.second.size.y / text_size.y) }, box.first, olc::WHITE);
			box.second.Draw(pge);
		}
	}
}

gui::DragBox* gui::DragBoxPanel::operator()(const std::string& name) {

	auto box = drag_boxes.find(name);
	if (box != drag_boxes.end()) return &box->second;

	return nullptr;
}



gui::ColorPicker::ColorPicker(const olc::vi2d& _position, const std::string& filename) 
	: position(_position) {
	color_circle = new olc::Sprite(filename);
	if (color_circle) {
		radius = (float)color_circle->width / 2.0f;
	}
}

bool gui::ColorPicker::IsPointInBounds(const olc::vf2d& point) const {
	const olc::vf2d& color_circle_pos = position + olc::vf2d{ color_circle->width / 2.0f, color_circle->height / 2.0f };
	return (point.x - color_circle_pos.x) * (point.x - color_circle_pos.x) + (point.y - color_circle_pos.y) * (point.y - color_circle_pos.y) <= radius * radius;
}

bool gui::ColorPicker::Input(olc::PixelGameEngine* pge) {
	const olc::vf2d& m_pos = (olc::vf2d)pge->GetMousePos();

	if (pge->GetMouse(0).bHeld) {
		if (IsPointInBounds(m_pos)) {
			const olc::vi2d& point = m_pos - position;
			selected_color = color_circle->GetPixel(point);
			return true;
		}
	}

	return false;
}

void gui::ColorPicker::Draw(olc::PixelGameEngine* pge) const {
	if (!color_circle) return;
	pge->SetPixelMode(olc::Pixel::MASK);
	pge->DrawSprite(position, color_circle);
	pge->SetPixelMode(olc::Pixel::NORMAL);
}

void gui::ColorPicker::ClearMemory() {
	UtilityFunctions::SafeDeletePointer(color_circle);
}

gui::ColorPanel::ColorPanel(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, const std::string& filename) 
	: BoxUIBase(_position, _size, _color) {
	int offset = 1;
	color_picker = ColorPicker{ { position.x + offset, position.y + offset }, filename };
}

bool gui::ColorPanel::Input(olc::PixelGameEngine* pge) {

	const olc::vf2d& m_pos = (olc::vf2d)pge->GetMousePos();
	bool is_pos_in_bounds = false;

	if (pge->GetMouse(0).bHeld) {
		if (!is_pressed && IsPointInBounds(m_pos)) {
			bool is_color_picker_input = color_picker.Input(pge);
			if (!(is_pressed || is_color_picker_input)) {
				is_pressed = true;
			}
			else if (is_color_picker_input) { is_pos_in_bounds = true; }
		}
		
		if (is_pressed) {
			position += (m_pos - prev_m_pos);
			color_picker.position += (m_pos - prev_m_pos);
		}
	}

	if (pge->GetMouse(0).bReleased) is_pressed = false;
	prev_m_pos = m_pos;

	return is_pos_in_bounds | is_pressed;
}

void gui::ColorPanel::Draw(olc::PixelGameEngine* pge) {
	if (!is_render) return;

	pge->FillRect(position, size, color);
	pge->DrawRect(position, size, olc::WHITE);
	color_picker.Draw(pge);
}

void gui::ColorPanel::Clear() {
	color_picker.ClearMemory();
}

gui::ListBox::ListBox(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, int _y_offset)
	: BoxUIBase(_position, _size, _color), y_offset(_y_offset) {}

void gui::ListBox::AddItem(const std::string& item_name, const olc::Pixel& item_color) {
	int offset = 0;

	Button item_button{ { position.x + offset, position.y + offset + (int)items.size() * y_offset }, { size.x - offset, y_offset }, item_color, false };
	items.insert({ item_name, item_button });
}

bool gui::ListBox::Input(olc::PixelGameEngine* pge) {

	const olc::vi2d& m_pos = pge->GetMousePos();
	
	if (pge->GetMouse(1).bPressed && !is_render) {
		is_render = true;

		position = m_pos;

		int offset = 0, n = 0;
		for (auto& item : items) {
			item.second.position = { position.x + offset, position.y + offset + n * y_offset };
			n++;
		}
	}
	if (is_render) {
		for (auto& item : items) is_pressed |= item.second.Input(pge);
	}

	if (pge->GetMouse(0).bPressed && !IsPointInBounds(m_pos)) {
		is_pressed = false;
		is_render = false;
	}
	return is_pressed;
}

void gui::ListBox::Draw(olc::PixelGameEngine* pge) {
	if (!is_render) return;

	pge->FillRect(position, size, color);
	pge->DrawRect(position, size, olc::WHITE);

	for (auto& item : items) {
		item.second.Draw(pge);

		const olc::vi2d& text_size = pge->GetTextSizeProp(item.first);
		pge->DrawStringProp({ item.second.position.x + (item.second.size.x - text_size.x) / 2, item.second.position.y }, item.first);
	}
}

gui::Button* gui::ListBox::operator()(const std::string& name)
{
	auto item = items.find(name);
	if (item != items.end()) return &item->second;

	return nullptr;
}
