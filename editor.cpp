#include "editor.h"

void Editor::StateUpdate() {
	if (state->is_state_change) {
		switch (state->state) {
		
		case State::States::EDIT: {
			PlayState* play_state = static_cast<PlayState*>(state.get());

			const std::vector<PolygonShape> polygons = play_state->polygons;
			const olc::vf2d offset = play_state->offset;
			state = std::make_unique<EditState>(pge);
			state->polygons = polygons;
			state->offset = offset;
		}
		break;
		case State::States::PLAY: {
			EditState* edit_state = static_cast<EditState*>(state.get());

			const olc::vf2d offset = edit_state->offset;
			const std::vector<PolygonShape> polygons = edit_state->polygons;
			std::cout << "PolygonData: " << offset << " " << polygons.size() << std::endl;

			state = std::make_unique<PlayState>(pge);
			state->offset = offset;
			state->polygons = polygons;
			state->Initialize();
		}
		break;
		}
		pge->Clear(olc::BLACK);

		state->is_state_change = false;
	}
}
