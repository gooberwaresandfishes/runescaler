#include "OSAKA.h"

bool running;

char NAME[TITLE_CHARACTER_LENGTH];
int windowWidth;
int windowHeight;

void OSAKA_Run(char name[TITLE_CHARACTER_LENGTH], int width, int height, void (*init)(), void (*update)(), void (*render)(), void (*quit)())
{
	OSAKA_Init(name, width, height);
	
	OSAKA_MainLoop(init, update, render, quit);
	
	OSAKA_Quit(0);
}

void OSAKA_Init(char name[TITLE_CHARACTER_LENGTH], int width, int height)
{
	TraceLog(LOG_INFO, "initialising OSAKA engine, AMERICA YA :D !");
	
	OSAKA_InitWindow(name, width, height, ICON_FILE_NAME);
	
	OSAKA_InitAudio();
	
	OSAKA_InitResources();
	
	// misc
	SetExitKey(0);
	SetTargetFPS(60);
	
	TraceLog(LOG_INFO, "successfully initialised OSAKA engine, HALLO :D ! HALLO :D ! HALLO :D !");
}

void OSAKA_InitWindow(char name[TITLE_CHARACTER_LENGTH], int width, int height, char fileName[PATH_CHARACTER_LENGTH])
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, name);

	(IsWindowReady()) ?
		TraceLog(LOG_INFO, "successfully initialised window") :
		TraceLog(LOG_FATAL, "failed to initialise window, quitting...");
		
	windowWidth = width;
	windowHeight = height;
	
	OSAKA_InitIcon(fileName);
}

void OSAKA_InitIcon(char fileName[PATH_CHARACTER_LENGTH])
{
	Image icon = LoadImage(fileName);
	
	if (icon.data)
	{
		SetWindowIcon(icon);
		UnloadImage(icon);
		
		TraceLog(LOG_INFO, "succesfully loaded icon (icon file name : %s)", ICON_FILE_NAME);
	}
	else
	{
		TraceLog(LOG_ERROR, "failed to load icon (icon file name : %s)", ICON_FILE_NAME);
	}
}

void OSAKA_InitAudio()
{
	InitAudioDevice();
	
	(IsAudioDeviceReady()) ?
		TraceLog(LOG_INFO, "successfully initialised audio device") :
		TraceLog(LOG_FATAL, "failed to initialise device, quitting...");
}

void OSAKA_MainLoop(void (*init)(), void (*update)(), void (*render)(), void (*quit)())
{
	init();
	
	// initialise buffer and resize dimensions
	RenderTexture2D screenBuffer = LoadRenderTexture(windowWidth, windowHeight);
	
	Rectangle originalDimensions = { 0, 0, (float)windowWidth, -(float)windowHeight };
	Rectangle dimensions = { 0, 0, (float)windowWidth, (float)windowHeight };
	
	running = true;
	
	while (running)
	{	
		running = !WindowShouldClose();
		
		dimensions.width = windowWidth = GetScreenWidth();
		dimensions.height = windowHeight = GetScreenHeight();
		
		
		update();
		
		
		// draw to buffer
		BeginTextureMode(screenBuffer);
        ClearBackground(BLACK);
		
        render();
		
        EndTextureMode();
		
		// draw buffer to screen
		BeginDrawing();
		ClearBackground(BLACK);
		
		// draw scaled buffer
		DrawTexturePro(screenBuffer.texture, originalDimensions, dimensions, (Vector2){ 0, 0 }, 0.0f, WHITE);
		
		EndDrawing();
	}
	
	UnloadRenderTexture(screenBuffer);
	
	quit();
}

void OSAKA_Quit(int exitCode)
{
	TraceLog(LOG_INFO, "quitting OSAKA engine, BYE BYE :D !");
	
	OSAKA_QuitResources();

	CloseAudioDevice();
	
	CloseWindow();
	
	TraceLog(LOG_INFO, "successfully quitted OSAKA engine, BYE BYE :D !");
	
	exit(exitCode);
}