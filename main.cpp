#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

struct Mus{
	Music currentTrack;
	float musicVolume = 0.5f;
	float timeElapsed = 0.0f;
	float trackLength = 0.0f;
	float percentagePlayed = 0.00f;
	int elapsedMins = 0;
	int elapsedSecs = 0;
	int totalMins = 0;
	int totalSecs = 0;
};

struct playerCtlRender{
	Rectangle lowerStatusBounds;
	Rectangle pauseButtonBounds;
	Rectangle sliderBounds;
	Rectangle progressBarBounds;
	float bx = 30;
	float by = 30;
	float sx = 90;
	float sy = 30;
	float bx_padding = 15;
	float by_padding = 45;
	float bx_spacing = 45;
	bool isPaused = false;
	bool noMusic = false;
};

void renderGui(int sw, int sh, playerCtlRender& pctl, Mus& m, const char* trackTitle); 
void updateState(float* ptr_sw, float* ptr_sh, Mus& m, playerCtlRender& pctl);
int playSingleTrack(const char* trackDir, Mus& m);

int main(int argc, char *argv[]){
	SetTraceLogLevel(1);
	int init_sw = 550;
	int init_sh = 200;

	float sw = 0.0f;
	float sh = 0.0f;

	float* ptr_sw = &sw;
	float* ptr_sh = &sh;

	const char* trackDir = argv[1];
	const char* trackTitle = argv[1];
	trackTitle = GetFileNameWithoutExt(trackTitle);

	Mus mus;
	playerCtlRender pctl;

	InitWindow(init_sw, init_sh, "Caudio - A C++ music player!");
	GuiLoadStyle("style_cherry.rgs");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(0);
	InitAudioDevice();

	if(argv[1] != NULL){
		playSingleTrack(trackDir, mus);
	}else{
		CloseAudioDevice();
		pctl.noMusic = true;
	}

	while(!WindowShouldClose()){
		updateState(ptr_sw, ptr_sh, mus, pctl);
		BeginDrawing();
			renderGui(sw, sh, pctl, mus, trackTitle);
		EndDrawing();
	}
	CloseWindow();
}

void updateState(float* ptr_sw, float* ptr_sh, Mus& m, playerCtlRender& pctl){
	float current_sw = GetRenderWidth();
	float current_sh = GetRenderHeight();

	*ptr_sw = current_sw;
	*ptr_sh = current_sh;

	if(pctl.isPaused == true && pctl.noMusic == false){
		PauseMusicStream(m.currentTrack);
	}
	if(pctl.isPaused == false && pctl.noMusic == false){
		ResumeMusicStream(m.currentTrack);
	}

	if(pctl.noMusic == true){
		return;
	}

	UpdateMusicStream(m.currentTrack);
	SetMusicVolume(m.currentTrack, m.musicVolume);
	
	m.timeElapsed = GetMusicTimePlayed(m.currentTrack);
	m.trackLength = GetMusicTimeLength(m.currentTrack);
	m.percentagePlayed = (m.timeElapsed / m.trackLength)*100;

	if(IsKeyPressed(KEY_H)){
		if(m.timeElapsed < 6){
			SeekMusicStream(m.currentTrack, 0);
		}
		else{
			SeekMusicStream(m.currentTrack, m.timeElapsed - 5);
		}
	}
	if(IsKeyPressed(KEY_L)){
		SeekMusicStream(m.currentTrack, m.timeElapsed +5);
	}
}

void renderGui(int sw, int sh, playerCtlRender& pctl, Mus& m, const char* trackTitle){
	m.elapsedMins = (int)m.timeElapsed / 60;
	m.elapsedSecs = (int)m.timeElapsed % 60;
	m.totalMins = (int)m.trackLength / 60;
	m.totalSecs = (int)m.trackLength % 60;

	Rectangle statusBounds = {0, 0, (float)sw, (float)30};
	Rectangle closeButtonBounds = {(float)sw - 30, 0, 30, 30};
	Rectangle maximizeButtonBounds = {(float)sw - 58, 0, 30, 30};
	Rectangle panel = {(float)0, (float)5, (float)sw, ((float)sh - 64)};
	pctl.sliderBounds = {pctl.bx_padding + (pctl.bx_spacing * 5), ((float)sh) - pctl.by_padding, pctl.sx, pctl.sy };
	pctl.lowerStatusBounds = {0, (float)sh - 60, (float)sw, 60};
	pctl.pauseButtonBounds = {pctl.bx_padding, (float)sh - pctl.by_padding, pctl.bx, pctl.by};
	pctl.progressBarBounds = {pctl.bx_padding + (pctl.bx_spacing * 2), (float)sh - pctl.by_padding, pctl.sx, pctl.sy};

	GuiPanel(panel, "");
	GuiStatusBar(pctl.lowerStatusBounds, "");
	GuiStatusBar(statusBounds, TextFormat("Now playing - %s", trackTitle));
	GuiToggle(pctl.pauseButtonBounds, "P", &pctl.isPaused);
	GuiSlider(pctl.sliderBounds, "0", "100", &m.musicVolume, 0.0f, 1.0f);
	GuiProgressBar(pctl.progressBarBounds, TextFormat("%d:%02d", m.elapsedMins, m.elapsedSecs),
			TextFormat("%d:%02d", m.totalMins, m.totalSecs), &m.percentagePlayed, 0.00f, 100.00f);
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
