#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#define PHYSICS_H
#include "editor.h"
#define OLC_PGEX_SPLASHSCREEN
#include "olcPGEX_SplashScreen.h"

class App : public olc::PixelGameEngine {
private:
	Editor editor;
	olcPGEX_SplashScreen splash_screen;
public:
	App() {
		sAppName = "Polygon Sandbox";
	}

	bool OnUserCreate() override {

		editor = Editor{ this };
		editor.DrawBackground();

		splash_screen.SetOptions(4, 2, 2.0f, 0.5f, olc::CYAN * 0.1f, olc::CYAN, olc::YELLOW, olc::WHITE);

		return true;
	}
	bool OnUserUpdate(float dt) override {

		if (splash_screen.AnimateSplashScreen(dt)) return true;

		// Window Update
		editor.WindowUpdate();

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