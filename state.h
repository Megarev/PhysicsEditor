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
	bool is_polygon_fill = false, is_update_render = false;
	bool is_snap_to_grid = true;

	olc::vf2d offset; // Panning
	olc::vi2d level_size;
	uint32_t id_count = 0;

	bool IsPointInLevel(const olc::vf2d& point) const;

	olc::vf2d ToWorld(const olc::vf2d& point) const { return point + offset; }
	olc::vf2d ToScreen(const olc::vf2d& point) const { return point - offset; }
	olc::vi2d ToGrid(const olc::vf2d& point) const { return olc::vi2d(point / unit_size) * unit_size; }

	void PanLevel(const olc::vf2d& m_pos);
private: // Editing functions
	uint32_t unit_size = 0;
	olc::vf2d press_m_pos, prev_m_pos;
	PolygonShape* selected_shape = nullptr;
	olc::vf2d* selected_vertex = nullptr;

	void OnMousePressEdit(const olc::vf2d& world_m_pos);
	void OnMouseHoldEdit(const olc::vf2d& m_pos, const olc::vf2d& world_m_pos);
	void OnMouseReleaseEdit();

	void OnMousePressAdd(const olc::vf2d& world_m_pos);
	void RemovePolygon();

	bool is_feature = false; // Is edit feature being used
	
	enum class EditFeature { NONE = 0, TRANSLATE, SCALE, ROTATE, VERTEX } edit_feature;
	void Scale(const olc::vf2d& m_pos);
	void Rotate(const olc::vf2d& m_pos);
	void Translate(const olc::vf2d& m_pos);
	void MoveVertex(const olc::vf2d& m_pos);
private: // Layers
	struct LayerData { 
		uint32_t id = 0; 
		bool state = false, is_update = false;
	};
	std::unordered_map<std::string, LayerData> layers;
private: // GUI
	gui::ButtonPanel button_panel;
	gui::DragBoxPanel box_panel;
	gui::ColorPanel color_panel;
	gui::ListBox poly_panel;

	void ButtonFunctions();
	void ListBoxFunctions();

	PolygonShape* add_polygon = nullptr;

	void IsRenderGUI(bool state);
public:
	EditState(olc::PixelGameEngine* pge);

	void Input() override;
	void Update() override;
	void Draw() override;
	void DrawBackground() override;
};
