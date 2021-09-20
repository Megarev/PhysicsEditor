#include "gui.h"

gui::Button::Button(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color)
	: position(_position), size(_size), init_color(_color), color(_color) {}

bool gui::Button::IsPointInBounds(const olc::vf2d& point) const {
	return point.x > position.x && point.x < position.x + size.x &&
		point.y > position.y && point.y < position.y + size.y;
}

void gui::Button::Input(olc::PixelGameEngine* pge) {

	const olc::vf2d& m_pos = pge->GetMousePos() * 1.0f;

	color = init_color * 0.25f;
	is_pressed = false;
	
	if (IsPointInBounds(m_pos)) {
		color = init_color * 0.5f;
		if (pge->GetMouse(0).bPressed) {
			color = init_color * 0.9f;
			is_pressed = true;
		}
	}

	//if (pge->GetMouse(0).bReleased) is_pressed = false;
}

void gui::Button::Draw(olc::PixelGameEngine* pge) const {
	pge->DrawRect(position, size, olc::WHITE);
	pge->FillRect(position + 0.1f * size, 0.8f * size, color);
}



gui::ButtonPanel::ButtonPanel(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color) 
	: position(_position), size(_size), color(_color) {}

void gui::ButtonPanel::AddButton(const std::string& name, const olc::Pixel& button_color, const olc::vi2d& button_size) {
	
	Button button{ position + (int)buttons.size() * button_size, button_size, button_color };
	buttons.insert({ name, button });
}

bool gui::ButtonPanel::IsPointInBounds(const olc::vf2d& point) const {
	return point.x > position.x && point.x < position.x + size.x &&
		point.y > position.y && point.y < position.y + size.y;
}

void gui::ButtonPanel::Input(olc::PixelGameEngine* pge) {
	for (auto& button : buttons) button.second.Input(pge);
}

void gui::ButtonPanel::Draw(olc::PixelGameEngine* pge) const {
	pge->FillRect(position, size, color);
	for (auto& button : buttons) button.second.Draw(pge);
}

gui::Button* gui::ButtonPanel::operator()(const std::string& name) {

	auto button = buttons.find(name);
	if (button != buttons.end()) return &button->second;

	return nullptr;
}
