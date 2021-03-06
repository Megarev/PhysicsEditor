#pragma once
#include "olcPixelGameEngine.h"
#include <unordered_map>

class UtilityFunctions {
public:
	template <typename T>
	static void SafeDeletePointer(T*& ptr) {
		delete ptr;
		ptr = nullptr;
	}
};

namespace gui {
	class BoxUIBase {
	public:
		olc::vi2d position, size;
		olc::Pixel init_color, color;

		bool is_pressed = false;
	public:
		BoxUIBase() {}
		BoxUIBase(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color)
			: position(_position), size(_size), init_color(_color), color(_color) {}

		virtual bool IsPointInBounds(const olc::vf2d& point) const;

		virtual bool Input(olc::PixelGameEngine* pge) = 0;
		virtual void Draw(olc::PixelGameEngine* pge) = 0;

		virtual void Clear() {}
	};

	struct IconData {
		olc::vi2d source_pos, source_size;
		olc::Decal* spritesheet_ptr = nullptr;
	};

	class Button : public BoxUIBase {
	private:
		bool is_button_toggleable = false;
	public:
		bool is_toggle_state = false;
		IconData icon_data;
	public:
		Button() {}
		Button(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, bool is_toggleable = false);

		bool Input(olc::PixelGameEngine* pge) override;
		void Draw(olc::PixelGameEngine* pge) override;
		void DrawSprite(olc::PixelGameEngine* pge);
	};

	class ButtonPanel : public BoxUIBase {
	public:
		std::unordered_map<std::string, Button> buttons;
	public:
		ButtonPanel() {}
		ButtonPanel(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color);

		void AddButton(const std::string& name, const olc::Pixel& button_color, 
			bool is_toggleable = false, const olc::vi2d& source_pos = { 0, 0 }, 
			const olc::vi2d& source_size = { 0, 0 }, const olc::vi2d & button_size = { 32, 32 }, olc::Decal* decal = nullptr);

		bool Input(olc::PixelGameEngine* pge) override;
		void Draw(olc::PixelGameEngine* pge) override;
		void DrawSprite(olc::PixelGameEngine* pge);

		Button* operator()(const std::string& name);
	};


	class DragBox : public BoxUIBase {
	public:
		float value = 0.0f;
		olc::vi2d prev_m_pos;
		Button reset_button;

		float speed = 0.1f;

		std::pair<float, float> value_constraints = { -INFINITY, INFINITY };
	public:
		DragBox() {}
		DragBox(const olc::vi2d& _position, const olc::vf2d& _size, const olc::Pixel& _color, float start_value);

		bool Input(olc::PixelGameEngine* pge) override;
		void Draw(olc::PixelGameEngine* pge) override;
	};


	class DragBoxPanel : public BoxUIBase {
	public:
		std::string title;
		std::unordered_map<std::string, DragBox> drag_boxes;
		bool is_render = true;
		olc::vi2d prev_m_pos;

		int title_scale = 2;
	public:
		DragBoxPanel() {}
		DragBoxPanel(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, const std::string& _title);

		void AddDragBox(const std::string& name, const olc::Pixel& box_color, const std::pair<float, float>& _value_constraints, const olc::vi2d& box_size = { 32, 32 }, float start_value = 0.0f);
		void SetDragBoxSpeed(const std::string& name, float speed);

		bool IsPointInBounds(const olc::vf2d& point) const;

		bool Input(olc::PixelGameEngine* pge) override;
		void Draw(olc::PixelGameEngine* pge) override;

		DragBox* operator()(const std::string& name);
	};


	class ColorPicker {
	public:
		olc::Sprite* color_circle = nullptr;
		olc::vi2d position;
		float radius = 0.0f;
		olc::Pixel selected_color = olc::WHITE;
	public:
		ColorPicker() {}
		ColorPicker(const olc::vi2d& _position, const std::string& filename);

		bool IsPointInBounds(const olc::vf2d& point) const;

		bool Input(olc::PixelGameEngine* pge);
		void Draw(olc::PixelGameEngine* pge) const;

		void ClearMemory();
	};


	class ColorPanel : public BoxUIBase {
	public:
		ColorPicker color_picker;
		olc::vi2d prev_m_pos;
		bool is_render = false;
	public:
		ColorPanel() {}
		ColorPanel(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, const std::string& filename);

		bool Input(olc::PixelGameEngine* pge) override;
		void Draw(olc::PixelGameEngine* pge) override;

		void Clear() override;
	};
	

	class ListBox : public BoxUIBase {
	public:
		std::unordered_map<std::string, Button> items;
		bool is_render = false;
		int y_offset = 0;
	public:
		ListBox() {}
		ListBox(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, int _y_offset);

		void AddItem(const std::string& item_name, const olc::Pixel& item_color);

		bool Input(olc::PixelGameEngine* pge) override;
		void Draw(olc::PixelGameEngine* pge) override;

		Button* operator()(const std::string& name);
	};
	
	class TexturePanel : public BoxUIBase {
	private:
		olc::Decal* textures = nullptr;
	public:
		TexturePanel() {}
		TexturePanel(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, olc::Decal* _textures);

		bool Input(olc::PixelGameEngine* pge) override;
		void Draw(olc::PixelGameEngine* pge) override;
	};

	class TextBox : public BoxUIBase {
	private:
		std::string text;
	public:
		bool is_render = false;
		TextBox() {}
		TextBox(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, const std::string& _text);
		/*void SetText(const std::string& _text) { text = _text; }
		void SetPosition(const olc::vi2d& _position) { position = _position; }
		void SetSize(const olc::vi2d& _size) { size = _size; }
		void SetColor(const olc::Pixel& _color) { color = _color; }*/
		void SetBox(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, const std::string& _text);

		bool Input(olc::PixelGameEngine* pge) override;
		void Draw(olc::PixelGameEngine* pge) override;
	};

	class TextPanel : public BoxUIBase {
	private:
		std::vector<std::string> texts;
		std::string title;
	public:
		int scale = 1;
		bool is_render = true;
		TextPanel() {}
		TextPanel(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, const std::vector<std::string>& _texts);
		void SetPanel(const olc::vi2d& _position, const olc::vi2d& _size, const olc::Pixel& _color, const std::vector<std::string>& _texts, int _scale = 1);
		void SetTitle(const std::string& _title) { title = _title; }

		bool Input(olc::PixelGameEngine* pge) override;
		void Draw(olc::PixelGameEngine* pge) override;
	};
};