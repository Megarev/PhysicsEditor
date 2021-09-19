#pragma once
#include "olcPixelGameEngine.h"
#include "state.h"

class Editor {
private:
	olc::PixelGameEngine* pge = nullptr;
	std::unique_ptr<State> state;
public:
	Editor() {}
	Editor(olc::PixelGameEngine* p)
		: pge(p) {
		state = std::make_unique<EditState>(pge);
	}

	void Input() { state->Input(); }
	void Update() { state->Update(); }
	void Draw() { state->Draw(); }
};