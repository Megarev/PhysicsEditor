#pragma once
#include "olcPixelGameEngine.h"
#include <unordered_map>

namespace gui {
	class Button {
	public:
		olc::vi2d position, size;
		olc::Pixel init_color, color;

		bool is_pressed = false;
	public:
		Button() {}
		Button(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color);

		bool IsPointInBounds(const olc::vf2d& point) const;

		bool Input(olc::PixelGameEngine* pge);
		void Draw(olc::PixelGameEngine* pge) const;
	};


	class ButtonPanel {
	public:
		olc::vi2d position, size;
		olc::Pixel color;

		std::unordered_map<std::string, Button> buttons;
	public:
		ButtonPanel() {}
		ButtonPanel(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color);

		void AddButton(const std::string& name, const olc::Pixel& button_color, const olc::vi2d& button_size = { 32, 32 });

		bool IsPointInBounds(const olc::vf2d& point) const;

		bool Input(olc::PixelGameEngine* pge);
		void Draw(olc::PixelGameEngine* pge) const;

		Button* operator()(const std::string& name);
	};


	class DragBox {
	public:
		olc::vi2d position, size;
		olc::Pixel init_color, color;
		float value = 0.0f;
		olc::vi2d prev_m_pos;

		bool is_press = false;
		float speed = 0.01f;

		std::pair<float, float> value_constraints = { -INFINITY, INFINITY };
	public:
		DragBox() {}
		DragBox(const olc::vi2d& _position, const olc::vf2d& _size, const olc::Pixel& _color, float start_value);

		bool IsPointInBounds(const olc::vf2d& point) const;

		bool Input(olc::PixelGameEngine* pge);
		void Draw(olc::PixelGameEngine* pge) const;
	};


	class DragBoxPanel {
	public:
		olc::vi2d position, size;
		olc::Pixel color;

		std::string title;
		std::unordered_map<std::string, DragBox> drag_boxes;
		bool is_press = false, is_render = true;
		olc::vi2d prev_m_pos;

		int title_scale = 2;
	public:
		DragBoxPanel() {}
		DragBoxPanel(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, const std::string& _title);

		void AddDragBox(const std::string& name, const olc::Pixel& box_color, const std::pair<float, float>& _value_constraints, const olc::vi2d& box_size = { 32, 32 }, float start_value = 0.0f);
		
		bool IsPointInBounds(const olc::vf2d& point) const;

		bool Input(olc::PixelGameEngine* pge);
		void Draw(olc::PixelGameEngine* pge) const;

		DragBox* operator()(const std::string& name);

	};

	class ColorPicker {
	public:
		olc::Sprite* color_circle = nullptr;
		olc::vi2d position;
		float radius = 0.0f;
		olc::Pixel selected_color;
	public:
		ColorPicker() {}
		ColorPicker(const olc::vi2d& _position, const std::string& filename);

		bool IsPointInBounds(const olc::vf2d& point) const;

		bool Input(olc::PixelGameEngine* pge);
		void Draw(olc::PixelGameEngine* pge) const;
	};

	class ColorPanel {
	public:
		ColorPicker color_picker;
		olc::vi2d position, size;
		olc::Pixel bg_color;

		olc::vi2d prev_m_pos;
		bool is_pressed = false, is_render = false;
	public:
		ColorPanel() {}
		ColorPanel(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, const std::string& filename);

		bool IsPointInBounds(const olc::vf2d& point) const;

		bool Input(olc::PixelGameEngine* pge);
		void Draw(olc::PixelGameEngine* pge) const;
	};
};