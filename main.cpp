#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#define PHYSICS_H
#include "editor.h"

class App : public olc::PixelGameEngine {
private:
	Editor editor;
public:
	App() {
		sAppName = "Title";
	}

	bool OnUserCreate() override {

		editor = Editor{ this };
		editor.DrawBackground();

		return true;
	}

	bool OnUserUpdate(float dt) override {

		// State Machine
		editor.StateUpdate();

		// Input
		editor.Input();

		// Logic
		editor.Update();

		// Render
		Clear(olc::BLANK);
		editor.DrawBackground();
		editor.Draw();

		return true;
	}
};

int main() {

	srand((unsigned)time(0));

	App app;
	if (app.Construct(608, 512, 1, 1, false, true)) {
		app.Start();
	}

	return 0;
}