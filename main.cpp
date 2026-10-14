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

struct screenDimensions{
	int init_sw = 550;
	int init_sh = 200;
	float sw = 0.0f;
	float sh = 0.0f;
};

struct ioState{
	int btnActive = -1;
	char textInput[256] = {0};
	char targetDir[256] = {0};
	int scrollIndex = 0;
	int itemActive = -1;
	int itemFocused = -1;
	int prevItemActive = -1;
	const char* fileName = {0};
};

void renderGui(FilePathList& fl, ioState& io, screenDimensions&, playerCtlRender& pctl, Mus& m, const char* trackTitle); 
void updateState(FilePathList& fl, ioState& io, screenDimensions& sd, Mus& m, playerCtlRender& pctl);
int playSingleTrack(const char* trackDir, Mus& m);
void crawlDir(ioState& io, FilePathList& fl);

int main(int argc, char *argv[]){
	SetTraceLogLevel(1);

	const char* trackDir = argv[1];
	const char* trackTitle = argv[1];
	
	ioState io;
	screenDimensions sd;
	Mus mus;
	playerCtlRender pctl;
	FilePathList fl;
	fl.paths = {0};
	fl.count = {0};

	InitWindow(sd.init_sw, sd.init_sh, "Caudio - A C++ music player!");
	GuiLoadStyle("style_dark.rgs");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(144);
	InitAudioDevice();

	if(argv[1] != NULL){
		playSingleTrack(trackDir, mus);
	}else{
		pctl.noMusic = true;
	}

	while(!WindowShouldClose()){
		updateState(fl, io, sd, mus, pctl);
		BeginDrawing();
			renderGui(fl, io, sd, pctl, mus, trackTitle);
		EndDrawing();
	}
	CloseWindow();
}

void updateState(FilePathList& fl, ioState& io, screenDimensions& sd, Mus& m, playerCtlRender& pctl){
	sd.sw = GetRenderWidth();
	sd.sh = GetRenderHeight();

	if(pctl.isPaused == true && pctl.noMusic == false){
		PauseMusicStream(m.currentTrack);
	}
	if(pctl.isPaused == false && pctl.noMusic == false){
		ResumeMusicStream(m.currentTrack);
	}

	if(io.btnActive == 1){
		crawlDir(io, fl);	
	}
	if(io.btnActive == 0){
		TraceLog(1, "State 0");
	}
	if(io.btnActive == -1){
		TraceLog(1, "State -1");
	}
	if(io.itemActive != io.prevItemActive){
    		io.prevItemActive = io.itemActive;
    			if(io.itemActive >= 0 && io.itemActive < fl.count){
        			if(!pctl.noMusic) UnloadMusicStream(m.currentTrack);
        	playSingleTrack(fl.paths[io.itemActive], m);
        	pctl.noMusic = false;
    		}
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

void renderGui(FilePathList& fl, ioState& io, screenDimensions& sd, playerCtlRender& pctl, Mus& m, const char* trackTitle){
	m.elapsedMins = (int)m.timeElapsed / 60;
	m.elapsedSecs = (int)m.timeElapsed % 60;
	m.totalMins = (int)m.trackLength / 60;
	m.totalSecs = (int)m.trackLength % 60;

	Rectangle statusBounds = {0, 0, (float)sd.sw, (float)30};
	Rectangle closeButtonBounds = {(float)sd.sw - 30, 0, 30, 30};
	Rectangle maximizeButtonBounds = {(float)sd.sw - 58, 0, 30, 30};
	Rectangle panel = {(float)0, (float)5, (float)sd.sw, ((float)sd.sh - 30)};
	Rectangle textBoxBounds{(float)0, (float)29, (float)300, (float)sd.sh - 85};
	Rectangle listViewBounds = {(float)300, (float)29, ((float)sd.sw)-300, (float)sd.sh - 85};
	pctl.sliderBounds = {pctl.bx_padding + (pctl.bx_spacing * 5) + 20, ((float)sd.sh) - pctl.by_padding, pctl.sx, pctl.sy };
	pctl.lowerStatusBounds = {0, (float)sd.sh - 60, (float)sd.sw, 60};
	pctl.pauseButtonBounds = {pctl.bx_padding, (float)sd.sh - pctl.by_padding, pctl.bx, pctl.by};
	pctl.progressBarBounds = {pctl.bx_padding + (pctl.bx_spacing * 2), (float)sd.sh - pctl.by_padding, pctl.sx, pctl.sy};

	ClearBackground(BLACK);
	GuiListViewEx(listViewBounds, fl.paths, fl.count, &io.scrollIndex, &io.itemActive, &io.itemFocused);
	GuiTextInputBox(textBoxBounds, "", "Enter the directory for your music here!", io.textInput, 256, "Ok;Cancel", &io.btnActive, NULL);
	//GuiPanel(panel, "");
	GuiStatusBar(pctl.lowerStatusBounds, "");
	GuiStatusBar(statusBounds, "Caudio - Lightweight performant music player...");
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
	if(FileExists(trackDir) == false){
		TraceLog(1, "No valid directory provided as arg 1 to program call.");
		return 1;
	}
	m.currentTrack = LoadMusicStream(trackDir);
	PlayMusicStream(m.currentTrack);
	return 0;
}

void crawlDir(ioState& io, FilePathList& fl){
	UnloadDirectoryFiles(fl);
	if(DirectoryExists(io.textInput)){
		TraceLog(1, "Valid Directory");
		TextCopy(io.targetDir, io.textInput);
		io.btnActive = -1;
	}else{
		return;
	}

	fl = LoadDirectoryFilesEx(io.textInput, ".mp3", 1);
	TraceLog(1, TextFormat("%d", fl.count));

	io.btnActive = -1;
}
