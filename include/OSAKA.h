#ifndef OSAKA_H
#define OSAKA_H

#include <stdlib.h>
#include <stdbool.h>

#include "raylib.h"

#define TITLE_CHARACTER_LENGTH 128
#define PATH_CHARACTER_LENGTH 256

#define RESOURCES_PATH "./data/resources/"

#define ICON_FILE_NAME RESOURCES_PATH "icon.png"

extern bool running;

extern char NAME[TITLE_CHARACTER_LENGTH];
extern int windowWidth;
extern int windowHeight;

#include "OSAKA_resources.h"

void OSAKA_Run(char name[TITLE_CHARACTER_LENGTH], int width, int height, 
               void (*init)(), void (*update)(), void (*render)(), void (*quit)());

void OSAKA_Init(char name[TITLE_CHARACTER_LENGTH], int width, int height);

void OSAKA_InitWindow(char name[TITLE_CHARACTER_LENGTH], int width, int height, 
                      char fileName[PATH_CHARACTER_LENGTH]);
					  
void OSAKA_InitIcon(char fileName[PATH_CHARACTER_LENGTH]);
void OSAKA_InitAudio();
void OSAKA_MainLoop(void (*init)(), void (*update)(), void (*render)(), void (*quit)());
void OSAKA_Quit(int exitCode);

#endif /* OSAKA_H */