#include "raylib.h"
#include <string>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

struct Mus{
	Music currentTrack;
	float musicVolume = 0.00f;
};

struct playerCtlRender{
	Rectangle lowerStatusBounds;
	Rectangle pauseButtonBounds;
	Rectangle sliderBounds;
	bool isPaused = false;
};

void renderGui(int sw, int sh, playerCtlRender& pctl, Mus& m); 
void updateState(float* ptr_sw, float* ptr_sh, Mus& m, playerCtlRender& pctl);
int playSingleTrack(const char* trackDir, Mus& m);

int main(int argc, char *argv[]){
	SetTraceLogLevel(1);
	int init_sw = 800;
	int init_sh = 600;

	float sw = 0.0f;
	float sh = 0.0f;

	float* ptr_sw = &sw;
	float* ptr_sh = &sh;

	const char* trackDir = argv[1];

	Mus mus;
	playerCtlRender pctl;

	InitWindow(init_sw, init_sh, "Debug");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(0);
	InitAudioDevice();
	playSingleTrack(trackDir, mus);

	while(!WindowShouldClose()){
		updateState(ptr_sw, ptr_sh, mus, pctl);
		BeginDrawing();
			ClearBackground(WHITE);
			renderGui(sw, sh, pctl, mus);
			DrawText(argv[1], 50, 50, 12, BLACK);
		EndDrawing();
	}
	CloseWindow();
}

void updateState(float* ptr_sw, float* ptr_sh, Mus& m, playerCtlRender& pctl){
	float current_sw = GetRenderWidth();
	float current_sh = GetRenderHeight();

	*ptr_sw = current_sw;
	*ptr_sh = current_sh;
	if(pctl.isPaused == true){
		PauseMusicStream(m.currentTrack);
	}
	if(pctl.isPaused == false){
		ResumeMusicStream(m.currentTrack);
	}
	UpdateMusicStream(m.currentTrack);
	SetMusicVolume(m.currentTrack, m.musicVolume);
}

void renderGui(int sw, int sh, playerCtlRender& pctl, Mus& m){
	Rectangle statusBounds = {0, 0, (float)sw, (float)30};
	Rectangle closeButtonBounds = {(float)sw - 30, 0, 30, 30};
	Rectangle maximizeButtonBounds = {(float)sw - 58, 0, 30, 30};
	pctl.sliderBounds = {((float)sw) - 160, ((float)sh) - 60, 120, 60 };
	pctl.lowerStatusBounds = {0, (float)sh - 60, (float)sw, 60};
	pctl.pauseButtonBounds = {((float)sw / 2.0f) - 15, (float)sh - 60, 60, 60};

	GuiStatusBar(pctl.lowerStatusBounds, "test");
	GuiStatusBar(statusBounds, "Status Bar");
	GuiToggle(pctl.pauseButtonBounds, "P", &pctl.isPaused);
	GuiSlider(pctl.sliderBounds, "0", "100", &m.musicVolume, 0.0f, 1.0f);

	if(GuiButton(closeButtonBounds, "X")){
		CloseWindow();
	}
	if(GuiButton(maximizeButtonBounds, "O")){
		ToggleFullscreen();
	}
}

int playSingleTrack(const char* trackDir, Mus& m){
	if(DirectoryExists(trackDir) == false){
		TraceLog(1, "No valid directory provided as arg 1 to program call.");
	}
	m.currentTrack = LoadMusicStream(trackDir);
	PlayMusicStream(m.currentTrack);
	return 0;
}

