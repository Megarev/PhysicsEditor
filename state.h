#pragma once
#include "olcPixelGameEngine.h"
#include "polygon.h"

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
};

class EditState : public State {
private:
	std::vector<PolygonShape> polygons;
	olc::vf2d prev_m_pos;

	PolygonShape* selected_shape = nullptr;

	enum class EditFeature { NONE = 0, TRANSLATE, SCALE, ROTATE } edit_feature;
private: // Editing functions
	void Scale(const olc::vf2d& m_pos);
	void Rotate(const olc::vf2d& m_pos);
	void Translate(const olc::vf2d& m_pos);
public:
	EditState(olc::PixelGameEngine* pge);

	void Input() override;
	void Update() override;
	void Draw() override;
};