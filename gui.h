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

		void Input(olc::PixelGameEngine* pge);
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

		void Input(olc::PixelGameEngine* pge);
		void Draw(olc::PixelGameEngine* pge) const;

		Button* operator()(const std::string& name);
	};
};

