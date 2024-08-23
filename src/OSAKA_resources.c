#include "OSAKA.h"

Texture2D textures[TEXTURES_LENGTH];
Sound sounds[SOUNDS_LENGTH];
Music musicTracks[MUSIC_LENGTH];
Font fonts[FONTS_LENGTH];

// textures ------------------------------------------------------------------------------------------------------------

int OSAKA_LoadTexture(char fileName[PATH_CHARACTER_LENGTH], int index)
{
	if (index < 0 || index >= TEXTURES_LENGTH)
    {
        TraceLog(LOG_ERROR, "could not load texture, index out of bounds (file name : %s) (index : %i) (textures length : %i)", fileName, index, TEXTURES_LENGTH);
        return 0;
    }
	
	if (textures[index].id)
    {
        TraceLog(LOG_ERROR, "could not load texture, slot is not empty (file name : %s) (index : %i)", fileName, index);
        return 0;
    }
	
	Texture2D texture = LoadTexture(fileName);
	
	if (!texture.id)
    {
		TraceLog(LOG_ERROR, "failed to load texture, invalid file name (file name : %s) (index : %i)", fileName, index);
		return 0;
    }
	
	textures[index] = texture;
	TraceLog(LOG_INFO, "successfully loaded texture (file name : %s) (index : %i)", fileName, index);
	
	return index;
}

void OSAKA_UnloadTexture(int index)
{
	if (index < 0 || index >= TEXTURES_LENGTH)
    {
        TraceLog(LOG_ERROR, "could not unload texture, index out of bounds (index : %i) (textures length : %i)", index, TEXTURES_LENGTH);
        return;
    }
	
	if (!textures[index].id)
    {
        TraceLog(LOG_ERROR, "could not unload texture, slot is empty (index : %i)", index);
        return;
    }
	
	UnloadTexture(textures[index]);
    textures[index] = (Texture2D){0};	// make index empty by reinitialising
	
	TraceLog(LOG_INFO, "successfully unloaded texture (index : %i)", index);
}

// sounds --------------------------------------------------------------------------------------------------------------

int OSAKA_LoadSound(char fileName[PATH_CHARACTER_LENGTH], int index)
{
	if (index < 0 || index >= SOUNDS_LENGTH)
    {
        TraceLog(LOG_ERROR, "could not load sound, index out of bounds (index : %i) (sounds length : %i)", index, SOUNDS_LENGTH);
        return 0;
    }
	
	if (sounds[index].frameCount)
    {
        TraceLog(LOG_ERROR, "could not load sound, slot is not empty (file name : %s) (index : %i)", fileName, index);
        return 0;
    }
	
	Sound sound = LoadSound(fileName);
	
	if (!sound.frameCount)
    {
		TraceLog(LOG_ERROR, "failed to load sound, invalid file name (file name : %s) (index : %i)", fileName, index);
		return 0;
    }
	
	sounds[index] = sound;
	TraceLog(LOG_INFO, "successfully loaded sound (file name : %s) (index : %i)", fileName, index);
	
	return index;
}

void OSAKA_UnloadSound(int index)
{
	if (index < 0 || index >= SOUNDS_LENGTH)
    {
        TraceLog(LOG_ERROR, "could not unload sound, index out of bounds (index : %i) (sounds length : %i)", index, SOUNDS_LENGTH);
        return;
    }
	
	if (!sounds[index].frameCount)
    {
        TraceLog(LOG_ERROR, "could not unload sound, slot is empty (index : %i)", index);
        return;
    }
	
	UnloadSound(sounds[index]);
    sounds[index] = (Sound){0};	// make index empty by reinitialising
	
	TraceLog(LOG_INFO, "successfully unloaded sound (index : %i)", index);
}

// music ---------------------------------------------------------------------------------------------------------------

int OSAKA_LoadMusic(char fileName[PATH_CHARACTER_LENGTH], int index)
{
	if (index < 0 || index >= MUSIC_LENGTH)
    {
        TraceLog(LOG_ERROR, "could not load music, index out of bounds (file name : %s) (index : %i) (music length : %i)", fileName, index, MUSIC_LENGTH);
        return 0;
    }
	
	if (musicTracks[index].frameCount)
    {
        TraceLog(LOG_ERROR, "could not load music, slot is not empty (file name : %s) (index : %i)", fileName, index);
        return 0;
    }
	
	Music music = LoadMusicStream(fileName);
	
	if (!music.frameCount)
    {
		TraceLog(LOG_ERROR, "failed to load music, invalid file name (file name : %s) (index : %i)", fileName, index);
		return 0;
    }
	
	musicTracks[index] = music;
	TraceLog(LOG_INFO, "successfully loaded music (file name : %s) (index : %i)", fileName, index);
	
	return index;
}

void OSAKA_UnloadMusic(int index)
{
	if (index < 0 || index >= MUSIC_LENGTH)
    {
        TraceLog(LOG_ERROR, "could not unload music, index out of bounds (index : %i) (music length : %i)", index, MUSIC_LENGTH);
        return;
    }
	
	if (!musicTracks[index].frameCount)
    {
        TraceLog(LOG_ERROR, "could not unload music, slot is empty (index : %i)", index);
        return;
    }
	
	UnloadMusicStream(musicTracks[index]);
    musicTracks[index] = (Music){0};	// make index empty by reinitialising
	
	TraceLog(LOG_INFO, "successfully unloaded music (index : %i)", index);
}

// fonts ---------------------------------------------------------------------------------------------------------------

int OSAKA_LoadFont(char fileName[PATH_CHARACTER_LENGTH], int index)
{
	if (index < 0 || index >= FONTS_LENGTH)
    {
        TraceLog(LOG_ERROR, "could not load font, index out of bounds (file name : %s) (index : %i) (fonts length : %i)", fileName, index, FONTS_LENGTH);
        return 0;
    }
	
	if (fonts[index].glyphCount)
    {
        TraceLog(LOG_ERROR, "could not load font, slot is not empty (file name : %s) (index : %i)", fileName, index);
        return 0;
    }
	
	Font font = LoadFont(fileName);
	
	if (!font.glyphCount)
    {
		TraceLog(LOG_ERROR, "failed to load font, invalid file name (file name : %s) (index : %i)", fileName, index);
		return 0;
    }
	
	fonts[index] = font;
	TraceLog(LOG_INFO, "successfully loaded font (file name : %s) (index : %i)", fileName, index);
	
	return index;
}

void OSAKA_UnloadFont(int index)
{
	if (index < 0 || index >= FONTS_LENGTH)
    {
        TraceLog(LOG_ERROR, "could not unload font, index out of bounds (index : %i) (fonts length : %i)", index, FONTS_LENGTH);
        return;
    }
	
	if (!fonts[index].glyphCount)
    {
        TraceLog(LOG_ERROR, "could not unload font, slot is empty (index : %i)", index);
        return;
    }
	
	UnloadFont(fonts[index]);
    fonts[index] = (Font){0};	// make index empty by reinitialising
	
	TraceLog(LOG_INFO, "successfully unloaded font (index : %i)", index);
}

// ---------------------------------------------------------------------------------------------------------------------

void OSAKA_InitResources()
{
	OSAKA_LoadTexture(MISSING_TEXTURE_FILE_NAME, 0);
	OSAKA_LoadSound(MISSING_SOUND_FILE_NAME, 0);
	OSAKA_LoadMusic(MISSING_MUSIC_FILE_NAME, 0);
	OSAKA_LoadFont(MISSING_FONT_FILE_NAME, 0);
}

void OSAKA_QuitResources()
{
	 // unload all textures
    for (int i = 0; i < TEXTURES_LENGTH; i++) {
        if (textures[i].id) {
            UnloadTexture(textures[i]);
            textures[i] = (Texture2D){0};  // reset the texture slot
        }
    }
	
	 TraceLog(LOG_INFO, "unloaded all textures");

    // unload all sounds
    for (int i = 0; i < SOUNDS_LENGTH; i++) {
        if (sounds[i].frameCount) {
            UnloadSound(sounds[i]);
            sounds[i] = (Sound){0};  // reset the sound slot
        }
    }
	
	TraceLog(LOG_INFO, "unloaded all sounds");

    // unload all music
    for (int i = 0; i < MUSIC_LENGTH; i++) {
        if (musicTracks[i].frameCount) {
            UnloadMusicStream(musicTracks[i]);
            musicTracks[i] = (Music){0};  // reset the music slot
            }
    }
	
	TraceLog(LOG_INFO, "unloaded all music");

    // unload all fonts
    for (int i = 0; i < FONTS_LENGTH; i++) {
        if (fonts[i].glyphCount) {
            UnloadFont(fonts[i]);
            fonts[i] = (Font){0};  // reset the font slot
        }
    }
	
	TraceLog(LOG_INFO, "unloaded all fonts");
}