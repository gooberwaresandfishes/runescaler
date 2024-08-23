#ifndef OSAKA_RESOURCES_H
#define OSAKA_RESOURCES_H

#define TEXTURES_LENGTH 64
#define SOUNDS_LENGTH 32
#define MUSIC_LENGTH 16
#define FONTS_LENGTH 8

#define TEXTURES_PATH RESOURCES_PATH "textures/"
#define SOUNDS_PATH RESOURCES_PATH "sounds/"
#define MUSIC_PATH RESOURCES_PATH "music/"
#define FONTS_PATH RESOURCES_PATH "fonts/"

#define MISSING_TEXTURE_FILE_NAME TEXTURES_PATH "missing.png"
#define MISSING_SOUND_FILE_NAME SOUNDS_PATH "missing.wav"
#define MISSING_MUSIC_FILE_NAME MUSIC_PATH "missing.mp3"
#define MISSING_FONT_FILE_NAME FONTS_PATH "missing.ttf"

extern Texture2D textures[TEXTURES_LENGTH];
extern Sound sounds[SOUNDS_LENGTH];
extern Music musicTracks[MUSIC_LENGTH];
extern Font fonts[FONTS_LENGTH];

int OSAKA_LoadTexture(char fileName[PATH_CHARACTER_LENGTH], int index);
void OSAKA_UnloadTexture(int index);

int OSAKA_LoadSound(char fileName[PATH_CHARACTER_LENGTH], int index);
void OSAKA_UnloadSound(int index);

int OSAKA_LoadMusic(char fileName[PATH_CHARACTER_LENGTH], int index);
void OSAKA_UnloadMusic(int index);

int OSAKA_LoadFont(char fileName[PATH_CHARACTER_LENGTH], int index);
void OSAKA_UnloadFont(int index);

void OSAKA_InitResources();

void OSAKA_QuitResources();

#endif /* OSAKA_RESOURCES_H */