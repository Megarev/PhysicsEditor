#include "editor.h"

void Editor::StateUpdate() {
	if (state->is_state_change) {
		switch (state->state) {
		
		case State::States::EDIT: {
			PlayState* play_state = static_cast<PlayState*>(state.get());

			const std::vector<PolygonShape> polygons = play_state->polygons;
			const olc::vf2d offset = play_state->offset;
			const bool is_polygon_fill = play_state->is_polygon_fill;
			
			ConstraintManager mgr = play_state->constraint_mgr;
			JointPairManager joint_mgr = play_state->joint_mgr;

			state = std::make_unique<EditState>(pge);
			state->is_polygon_fill = is_polygon_fill;
			state->polygons = polygons;
			state->offset = offset;
			state->constraint_mgr = std::move(mgr);
			state->joint_mgr = std::move(joint_mgr);
			state->Initialize();
		}
		break;
		case State::States::PLAY: {
			EditState* edit_state = static_cast<EditState*>(state.get());

			const olc::vf2d offset = edit_state->offset;
			const std::vector<PolygonShape> polygons = edit_state->polygons;
			const bool is_polygon_fill = edit_state->is_polygon_fill;
			//std::cout << "PolygonData: " << offset << " " << polygons.size() << std::endl;

			ConstraintManager mgr = edit_state->constraint_mgr;
			JointPairManager joint_mgr = edit_state->joint_mgr;

			state = std::make_unique<PlayState>(pge);
			state->offset = offset;
			state->polygons = polygons;
			state->is_polygon_fill = is_polygon_fill;
			state->constraint_mgr = std::move(mgr);
			state->joint_mgr = std::move(joint_mgr);
			state->Initialize();
		}
		break;
		}
		pge->Clear(olc::BLACK);

		state->is_state_change = false;
	}
}

void Editor::WindowUpdate() {
	const olc::vi2d& new_size = pge->GetWindowSize() / pge->GetPixelSize();
	if (window_size != new_size) {
		window_size = new_size;
		pge->SetScreenSize(window_size.x, window_size.y);
		
		state->OnWindowUpdate();
	}
}
