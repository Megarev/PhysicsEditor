#pragma once
#include "olcPixelGameEngine.h"
#include "polygon.h"
#include "gui.h"

class State {
protected:
	olc::PixelGameEngine* pge = nullptr;
	const float PI = 3.1415926f;
public:
	State() {}
	State(olc::PixelGameEngine* p)
		: pge(p) {}

	virtual void Input() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawBackground() = 0;
};

class EditState : public State {
private: // Main editor
	std::vector<PolygonShape> polygons;
private: // Editing functions
	uint32_t unit_size = 0;
	olc::vf2d press_m_pos, prev_m_pos;
	PolygonShape* selected_shape = nullptr;
	
	enum class EditFeature { NONE = 0, TRANSLATE, SCALE, ROTATE } edit_feature;
	void Scale(const olc::vf2d& m_pos);
	void Rotate(const olc::vf2d& m_pos);
	void Translate(const olc::vf2d& m_pos);
private: // Layers
	struct LayerData { 
		uint32_t id = 0; 
		bool state = false; 
	};
	std::unordered_map<std::string, LayerData> layers;
		 //uint32_t bg_layer;
private: // GUI
	gui::ButtonPanel panel;

	void ButtonFunctions();
public:
	EditState(olc::PixelGameEngine* pge);

	void Input() override;
	void Update() override;
	void Draw() override;
	void DrawBackground() override;
};
