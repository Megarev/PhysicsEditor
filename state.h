#pragma once
#include "olcPixelGameEngine.h"
#include "polygon.h"
#include "gui.h"
#include "physics.h"
#include "constraints.h"
#include "Levels.h"

class Data {
private:
	Data() {}
public:
	static Data& Get() {
		static Data info;
		return info;
	}
public:
	bool is_edit_init = true;
	olc::vi2d level_size = { 768, 768 };
};

class LayerManager {
private:
	LayerManager() {}
	std::unordered_map<std::string, uint32_t> layer_data;
public:
	static LayerManager& Get() {
		static LayerManager layer_mgr;
		return layer_mgr;
	}

	void AddLayer(olc::PixelGameEngine* pge, const std::string& name) {
		layer_data.insert({ name, pge->CreateLayer() });
	}

	uint32_t GetLayer(const std::string& name) {
		return layer_data[name];
	}
};


class State {
protected:
	olc::PixelGameEngine* pge = nullptr;
	const float PI = 3.1415926f;
public:
	std::vector<PolygonShape> polygons;
	ConstraintManager constraint_mgr;
	JointPairManager joint_mgr;
public:
	olc::vf2d offset; // Panning
	bool is_polygon_fill = true;
	float scale_zoom = 1.0f;

	bool is_state_change = false;
	enum class States {
		EDIT,
		PLAY
	} state = States::EDIT;

	State() {}
	State(olc::PixelGameEngine* p)
		: pge(p) {}

	void ChangeState(const States& new_state) {
		state = new_state;
		is_state_change = true;
	}

	virtual void Initialize() {}
	virtual void OnWindowUpdate() {}

	virtual void Input() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawBackground() = 0;
};

class EditState : public State {
private: // Main editor
	bool is_update_render = false;
	bool is_snap_to_grid = false;
	bool is_mass_mode = false;
	bool is_helper_box = false;
	float mass_m = 100.0f;

	olc::vi2d level_size;
	uint32_t id_count = 0;

	gui::TextBox text_box;

	bool IsPointInLevel(const olc::vf2d& point) const;

	olc::vf2d ToWorld(const olc::vf2d& point) const { return (point + offset) * scale_zoom; }
	olc::vf2d ToScreen(const olc::vf2d& point) const { return point / scale_zoom - offset; }
	olc::vi2d ToGrid(const olc::vf2d& point) const { return olc::vi2d(point / (int)unit_size) * (int)unit_size; }

	void Initialize() override;
	void PanAndZoomLevel(const olc::vf2d& m_pos);
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

	void RemoveConstraint();
	std::pair<olc::vf2d, olc::vf2d> point_pair; // For remove constraint function

	void CopyPolygon(const olc::vf2d& pos);

	bool is_feature = false; // Is edit feature being used
	
	enum class EditFeature { NONE = 0, TRANSLATE, SCALE, ROTATE, VERTEX } edit_feature;
	enum class Mode { POLYGON, CONSTRAINTS, JOINTPAIR } mode;
	//void Scale(const olc::vf2d& m_pos);
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
	gui::Button clear_button;

	gui::DragBoxPanel box_panel, constraints_panel;
	gui::ColorPanel color_panel;
	gui::ListBox poly_panel;

	gui::TextPanel help_box;
	olc::Pixel help_box_color;
	void SetHelpBox(int n_slide);

	olc::Renderable icon_set;

	void ButtonFunctions();
	void ListBoxFunctions();

	PolygonShape* add_polygon = nullptr;
	
	void OnWindowUpdate() override;

	void IsRenderGUI(bool state);
public:
	EditState(olc::PixelGameEngine* pge);

	void Input() override;
	void Update() override;
	void Draw() override;
	void DrawBackground() override;
};

class PlayState : public State {
private:
	// GUI
	gui::Button edit_button;
	olc::Renderable icon_set;
private:
	// Main play
	Scene scene;
	olc::vf2d prev_m_pos;
public:
	PlayState(olc::PixelGameEngine* pge);

	void Initialize() override;

	void Input() override;
	void Update() override;
	void Draw() override;
	void DrawBackground() override;
};