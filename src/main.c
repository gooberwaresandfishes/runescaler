#include "OSAKA.h"
#include <string.h>
#include <math.h>

#define LIVE_ENTITY_LENGTH 10

#define TILE_SIZE 64
#define GRID_WIDTH 19
#define GRID_HEIGHT 13
#define FRICTION 0.02
#define GRAVITY 9.8
#define TERMINAL_VELOCITY 500

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ullong;

typedef struct LiveEnt LiveEnt;

void liveEntUpdate(LiveEnt* ent);
void liveEntRender(LiveEnt* ent);
bool checkCollision(LiveEnt* ent, LiveEnt* collider);
bool checkTileCollision(LiveEnt* ent, int tileX, int tileY);

void playerUpdate(LiveEnt* ent);
void playerOnXCollision(LiveEnt* ent, LiveEnt* collider);
void playerOnYCollision(LiveEnt* ent, LiveEnt* collider);
void playerOnTileXCollision(LiveEnt* ent, int tileX, int tileY);
void playerOnTileYCollision(LiveEnt* ent, int tileX, int tileYr);

void runeOnXCollision(LiveEnt* ent, LiveEnt* collider);
void runeOnYCollision(LiveEnt* ent, LiveEnt* collider);
void runeOnPlayerCollision(LiveEnt* ent, LiveEnt* collider);

void monsterUpdate(LiveEnt* ent);

LiveEnt createPlayer(int index,int x, int y, int width, int height);

LiveEnt createMonster(int index, int x, int y, int width, int height);

LiveEnt createPlatform(int index,int x, int y, int width, int height);

LiveEnt createItem(int index,int x, int y, float scaleX, float scaleY);

void level1();
void level2();
void level3();
void level4();
void level5();
void level6();
void level7();
void level8();
void level9();
void level10();
void level11();
void menu();

void init();
void update();
void render();
void quit();

void (*levels[12])() = {level1, level2, level3, level4, level5, level6, level7, level8, level9, level10, level11, menu};
int currentLevel;
bool initLevel;
bool isHard;
bool viewingStory;
bool viewingAnalysis;

float time;

// grid ----------------------------------------------------------------------------------------------------------------

int grid[GRID_HEIGHT][GRID_WIDTH];

// entity --------------------------------------------------------------------------------------------------------------

struct LiveEnt
{
	int index;
	bool initialised;
	bool facingRight;
	int flippedIndex;
	int type;
	float scaleX, scaleY;
	float x, y, dx, dy, ddx, ddy, fx, fy;
	bool onGround;
	float width, height;
	int imageIndex;
	
	void (*update)(LiveEnt* ent);
	void (*render)(LiveEnt* ent);
	
	void (*onXCollision)(LiveEnt* ent, LiveEnt* collider);
    void (*onYCollision)(LiveEnt* ent, LiveEnt* collider);
	void (*onTileXCollision)(LiveEnt* ent, int tileX, int tileY);
    void (*onTileYCollision)(LiveEnt* ent, int tileX, int tileY);
};

LiveEnt liveEnts[LIVE_ENTITY_LENGTH];
LiveEnt* selectedRune;

void liveEntUpdate(LiveEnt* ent)
{
	
	// Apply friction to horizontal movement
    ent->dx *= (1 - FRICTION);
	ent->dy *= (1 - FRICTION);
    
    // Update acceleration based on force and mass
    uint mass = ((ent->width + ent->height))/7;
	
	if (currentLevel ==8) mass = ((ent->width + ent->height) + 20)/7;
	
	if (mass < 10 && ent->type == 0) mass = 15;
	
    ent->ddx = ent->fx / mass;
    ent->ddy = ent->fy / mass;

    // Apply acceleration to velocity
    ent->dx += ent->ddx * GetFrameTime();
    ent->dy += (ent->ddy + GRAVITY) * GetFrameTime();

    // Apply terminal velocity to prevent infinite falling speed
    if (ent->dy > TERMINAL_VELOCITY) {
        ent->dy = TERMINAL_VELOCITY;
    }
	
	if (ent->dy < 0) ent->onGround = false;

    ent->x += ent->dx;

	// check for collisions with other entities
    for (int i = 0; i < LIVE_ENTITY_LENGTH; i++)
	{
        if (&liveEnts[i] != ent)
		{
            if (checkCollision(ent, &liveEnts[i]))
            {
                ent->onXCollision(ent, &liveEnts[i]);
            }
        }
    }

    // check for tile collisions
    for (int x = 0; x < GRID_WIDTH; x++)
	{
        for (int y = 0; y < GRID_HEIGHT; y++)
		{
            if (checkTileCollision(ent, x , y))
            {
                ent->onTileXCollision(ent, x, y);
            }
        }
    }
	
	
    ent->y += ent->dy;  

	// check for collisions with other entities
    for (int i = 0; i < LIVE_ENTITY_LENGTH; i++)
	{
        if (&liveEnts[i] != ent)
		{
            if (checkCollision(ent, &liveEnts[i]))
            {
                ent->onYCollision(ent, &liveEnts[i]);
            }
        }
    }

    // check for tile collisions
    for (int x = 0; x < GRID_WIDTH; x++)
	{
        for (int y = 0; y < GRID_HEIGHT; y++)
		{
            if (checkTileCollision(ent, x , y))
            {
                ent->onTileYCollision(ent, x, y);
            }
        }
    }       
	
	// boundaries
	if (ent->x < 0)
	{
		ent->x = 0;
	}
	else if (ent->x > 1216 - ent->width)
	{
		if (ent->type || (currentLevel == 9 && liveEnts[7].initialised) || currentLevel == 10)
		{
			ent->x = 1216 - ent->width;
		}
		else
		{
			currentLevel++;
			initLevel = true;
		}
	}
	
	if (ent->y < 0)
	{
		ent->y = 0;
	}
	else if (ent->y > 832 - ent->height)
	{
		ent->y = 832 - ent->height;
	}

	// reset forces for next frame
    ent->fx = 0;
    ent->fy = 0;
}

void liveEntRender(LiveEnt* ent)
{
	DrawTexturePro(
        textures[ent->facingRight ? ent->imageIndex : ent->flippedIndex],
        (Rectangle){ 0, 0, textures[ent->imageIndex].width, textures[ent->imageIndex].height },
        (Rectangle){ ent->x, ent->y, ent->width+2, ent->height+2 },
        (Vector2){ 0, 0 },
        0.0f,
        WHITE
    );
}

bool checkCollision(LiveEnt* ent, LiveEnt* collider)
{
    float entLeft = ent->x;
    float entRight = ent->x + ent->width;
    float entTop = ent->y;
    float entBottom = ent->y + ent->height;

    float colliderLeft = collider->x;
    float colliderRight = collider->x + collider->width;
    float colliderTop = collider->y;
    float colliderBottom = collider->y + collider->height;

    // true if theres no gaps between then on x or y
    return !((entRight < colliderLeft || colliderRight < entLeft) ||
		(entBottom < colliderTop || colliderBottom < entTop));
}

bool checkTileCollision(LiveEnt* ent, int tileX, int tileY)
{
    float entLeft = ent->x;
    float entRight = ent->x + ent->width;
    float entTop = ent->y;
    float entBottom = ent->y + ent->height;

    float tileLeft = tileX * TILE_SIZE;
    float tileRight = tileLeft + TILE_SIZE;
    float tileTop = tileY * TILE_SIZE;
    float tileBottom = tileTop + TILE_SIZE;

    // true if theres no gaps between then on x or y
    return !((entRight < tileLeft || tileRight < entLeft) || (entBottom < tileTop || tileBottom < entTop));
}

// player --------------------------------------------------------------------------------------------------------------

void playerUpdate(LiveEnt* ent)
{
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)){
		ent->fx = -100;
		ent->facingRight = false;
	}
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
		ent->fx = 100;
		ent->facingRight = true;
	}
	if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_UP)) && ent->onGround)
	{
		ent->fy = -15000;
		
	}
	
	if (selectedRune)
	{
		selectedRune->x = ent->facingRight ? ent->x + ent->width : ent->x - selectedRune->width;
		selectedRune->y = ent->y;
	}
	
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && selectedRune)
	{
		selectedRune->fx = ent->facingRight ? 2500 : -2500;
		selectedRune->fy = -3000;
		
		selectedRune = NULL;
		
		PlaySound(sounds[3]);
	}
	
	if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && selectedRune)
	{
		ent->width *= selectedRune->scaleX;
		ent->height *= selectedRune->scaleY;
		liveEnts[selectedRune->index] = (LiveEnt){0};;
		selectedRune = NULL;
		
		PlaySound(sounds[2]);
	}
	
	if (IsKeyDown(KEY_R))
	{
		initLevel = true;
	}
	
	viewingAnalysis = IsKeyDown(KEY_H);
	
	// remove later
	//if (IsKeyDown(KEY_G))
	//{
//		currentLevel = (sizeof(levels) / sizeof(levels[0])) -1;
//initLevel = true;
//	}
}

void playerOnXCollision(LiveEnt* ent, LiveEnt* collider)
{
	if (collider->type == 1 && ent->type == 0) return;
	
	if (ent->type == 0 && collider->type ==3)
	{
		if ((ent->width + ent->height) > (collider->width + collider->height))
		{
			liveEnts[collider->index] = (LiveEnt){0};
			PlaySound(sounds[4]);
		}
		else
		{
			initLevel = true;
			if (isHard) currentLevel = 0;
			PlaySound(sounds[4]);
		}
		return;
	}
	
	if (ent->type == 3 && collider->type ==0)
	{
		if ((ent->width + ent->height) < (collider->width + collider->height))
		{
			
			ent->initialised = false;
			ent->x= 99999999;
			
			PlaySound(sounds[4]);
		}
		else
		{
			initLevel = true;
			if (isHard) currentLevel = 0;
			PlaySound(sounds[4]);
		}
		return;
	}
	
	if (collider->type == 1)
	{
		if (collider == selectedRune) return;
		
		if (collider->scaleY > 1)
		{
			ent->y -= ent->height;
		}
		
		ent->width *= collider->scaleX;
		ent->height *= collider->scaleY;
		liveEnts[collider->index] = (LiveEnt){0};
		PlaySound(sounds[2]);
		return;
	}
	
    if (ent->dx > 0) {  // Moving right
        ent->x = collider->x - ent->width - ent->dx;
		if (ent->type==3) ent->facingRight = false;
    } else if (ent->dx < 0) {  // Moving left
        ent->x = collider->x + collider->width - ent->dx;
		if (ent->type==3)ent->facingRight = true;
    }
    ent->dx = 0;  // Stop horizontal movement on collision
    ent->fx = 0;  // Reset horizontal force
}

void playerOnYCollision(LiveEnt* ent, LiveEnt* collider)
{
	if (collider->type == 1 && ent->type == 0) return;
	
	if (ent->type == 0 && collider->type ==3)
	{
		if ((ent->width + ent->height) > (collider->width + collider->height))
		{
			liveEnts[collider->index] = (LiveEnt){0};
			PlaySound(sounds[4]);
		}
		else
		{
			initLevel = true;
			if (isHard) currentLevel = 0;
			PlaySound(sounds[4]);
		}
		return;

	}
	
	if (ent->type == 3 && collider->type ==0)
	{
		if ((ent->width + ent->height) < (collider->width + collider->height))
		{
			
			ent->initialised = false;
			ent->x= 99999999;
			
			PlaySound(sounds[4]);
			
		}
		else
		{
			initLevel = true;
			if (isHard) currentLevel = 0;
			PlaySound(sounds[4]);
		}
		return;
	}
	
	if (collider->type == 1)
	{
		if (collider == selectedRune) return;
	
		if (collider->scaleY > 1)
		{
			ent->y -= ent->height;
		}
	
		ent->width *= collider->scaleX;
		ent->height *= collider->scaleY;
		liveEnts[collider->index] = (LiveEnt){0};
		PlaySound(sounds[2]);
		return;
	}
	
	if (ent->dy > 0) {  // Falling down
        ent->y = collider->y - ent->height - 2;
        ent->onGround = true;  // Set a flag to indicate the entity is on the ground
    } else if (ent->dy < 0) {  // Moving up (jumping)
        ent->y = collider->y + collider->height - ent->dy;
    }
    ent->dy = 0;  // Stop vertical movement on collision
    ent->fy = 0;  // Reset vertical force
}

void playerOnTileXCollision(LiveEnt* ent, int tileX, int tileY)
{
	if (ent == selectedRune) return;
	
	if (grid[tileY][tileX] == 2 && ent->type != 2)
	{
		if (ent->type == 0)
		{
			initLevel = true;
			if (isHard) currentLevel = 0;
			PlaySound(sounds[4]);
		}
		else
		{
			ent->initialised = false;
			ent->x= 99999999;
			
			PlaySound(sounds[4]);
		}
			
		return;
	}
	
	if (grid[tileY][tileX] == 1)
	{
		if (ent->dx > 0) {  // Moving right
			ent->x = tileX*TILE_SIZE - ent->width - ent->dx;
			
			if (ent->type == 3)
			{
				ent->facingRight = false;
			}
		} else if (ent->dx < 0) {  // Moving left
			ent->x = tileX*TILE_SIZE + TILE_SIZE - ent->dx;
			
			if (ent->type == 3)
			{
				ent->facingRight = true;
			}
		}
		ent->dx = 0;  // Stop horizontal movement on collision
		ent->fx = 0;  // Reset horizontal force
	}
}

void playerOnTileYCollision(LiveEnt* ent, int tileX, int tileY)
{
	if (ent == selectedRune) return;
	
	if (grid[tileY][tileX] == 2 && ent->type != 2)
	{
		if (ent->type == 0)
		{
			initLevel = true;
			if (isHard) currentLevel = 0;
			PlaySound(sounds[4]);
		}
		else
		{
			ent->initialised = false;
			ent->x= 99999999;
			PlaySound(sounds[4]);
		}
			
		return;
	}
	
	if (grid[tileY][tileX])
	{
		if (ent->dy > 0) {  // Falling down
			ent->y = tileY*TILE_SIZE - ent->height - ent->dy;
			ent->onGround = true;  // Set a flag to indicate the entity is on the ground
		} else if (ent->dy < 0) {  // Moving up (jumping)
			ent->y -= ent->dy;
		}
		ent->dy = 0;  // Stop vertical movement on collision
		ent->fy = 0;  // Reset vertical force
	}
}

void runeOnXCollision(LiveEnt* ent, LiveEnt* collider)
{
	if (!collider->type)
	{
		runeOnPlayerCollision(ent, collider);
	}
}

void runeOnYCollision(LiveEnt* ent, LiveEnt* collider)
{
	if (!collider->type)
	{
		runeOnPlayerCollision(ent, collider);
	}
}

void runeOnPlayerCollision(LiveEnt* ent, LiveEnt* collider)
{
	if (!selectedRune && IsKeyDown(KEY_E))
	{
		PlaySound(sounds[1]);
		selectedRune = ent;
	}
}

void monsterUpdate(LiveEnt* ent)
{
	ent->fx = ent->facingRight ? 200 : -200;
}

void wizardUpdate(LiveEnt* ent)
{
	LiveEnt* target = &liveEnts[0];
	
	// Calculate the direction vector from the follower to the target
    float directionX = target->x - ent->x;
    float directionY = target->y - ent->y;

    // Calculate the distance between the two entities
    float distance = sqrt(directionX * directionX + directionY * directionY);

    // Normalize the direction vector
    if (distance != 0) {
        directionX /= distance;
        directionY /= distance;
    }

    // Apply the direction vector to the follower's force, scaled by speed
    ent->fx = directionX * 50;
    ent->fy = directionY * 800;
	
	ent->facingRight = ent->fx > 0;
	
}



// run -----------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	OSAKA_Run("RUNESCALER", 1216, 832, init, update, render, quit);

    return 0;
}

void init()
{
	OSAKA_LoadTexture(TEXTURES_PATH "tile0.png", 1);
	OSAKA_LoadTexture(TEXTURES_PATH "tile1.png", 2);
	OSAKA_LoadTexture(TEXTURES_PATH "player.png", 3);
	OSAKA_LoadTexture(TEXTURES_PATH "playerflipped.png", 4);
	OSAKA_LoadTexture(TEXTURES_PATH "platform.png", 5);
	OSAKA_LoadTexture(TEXTURES_PATH "2H.png", 6);
	OSAKA_LoadTexture(TEXTURES_PATH "1H.png", 7);
	OSAKA_LoadTexture(TEXTURES_PATH "2A.png", 8);
	OSAKA_LoadTexture(TEXTURES_PATH "1A.png", 9);
	OSAKA_LoadTexture(TEXTURES_PATH "2V.png", 10);
	OSAKA_LoadTexture(TEXTURES_PATH "1V.png", 11);
	OSAKA_LoadTexture(TEXTURES_PATH "monster.png", 12);
	OSAKA_LoadTexture(TEXTURES_PATH "monsterflipped.png", 13);
	OSAKA_LoadTexture(TEXTURES_PATH "spike.png", 14);
	OSAKA_LoadTexture(TEXTURES_PATH "wizard.png", 15);
	OSAKA_LoadTexture(TEXTURES_PATH "wizardflipped.png", 16);
	OSAKA_LoadTexture(TEXTURES_PATH "end.png", 17);
	OSAKA_LoadTexture(TEXTURES_PATH "menu.png", 18);
	OSAKA_LoadTexture(TEXTURES_PATH "start.png", 19);
	OSAKA_LoadTexture(TEXTURES_PATH "runeanalysis.png", 20);
	
	OSAKA_LoadMusic(MUSIC_PATH "music.mp3", 1);
	OSAKA_LoadMusic(MUSIC_PATH "battlemusic.mp3", 2);
	OSAKA_LoadMusic(MUSIC_PATH "menumusic.mp3", 3);
	
	OSAKA_LoadSound(SOUNDS_PATH "pickupCoin.wav", 1);
	OSAKA_LoadSound(SOUNDS_PATH "powerUp.wav", 2);
	OSAKA_LoadSound(SOUNDS_PATH "throw.wav", 3);
	OSAKA_LoadSound(SOUNDS_PATH "death.wav", 4);
	
	PlayMusicStream(musicTracks[3]);
	
	currentLevel = 11;
	initLevel = true;
}

void menu()
{
	// reset entities
	for (int i = 0; i < LIVE_ENTITY_LENGTH; i++) {
       liveEnts[i] = (LiveEnt){0};
    }
	
	int levelgrid[GRID_HEIGHT][GRID_WIDTH] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};
	
	memcpy(grid, levelgrid, sizeof(levelgrid));
}

void level1()
{
	// reset entities
	for (int i = 0; i < LIVE_ENTITY_LENGTH; i++) {
       liveEnts[i] = (LiveEnt){0};
    }
	
	liveEnts[0] = createPlayer(0,0,625, 126, 126);
	liveEnts[1] = createItem(1, 300, 765, 1, 0.5);
	
	int levelgrid[GRID_HEIGHT][GRID_WIDTH] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};
	
	memcpy(grid, levelgrid, sizeof(levelgrid));
}

void level2()
{
	// reset entities
	for (int i = 0; i < LIVE_ENTITY_LENGTH; i++) {
       liveEnts[i] = (LiveEnt){0};
    }
	
	liveEnts[0] = createPlayer(0,0,625, 62, 124);
	liveEnts[1] = createItem(1, 1100, 700, 1, 0.5);
	
	int levelgrid[GRID_HEIGHT][GRID_WIDTH] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,2,2,1,1,2,2,1,1,2,2,1,1,2,2,1,1,1}
	};
	
	memcpy(grid, levelgrid, sizeof(levelgrid));
	
	PlaySound(sounds[2]);
}

void level3()
{
	// reset entities
	for (int i = 0; i < LIVE_ENTITY_LENGTH; i++) {
       liveEnts[i] = (LiveEnt){0};
    }
	
	liveEnts[0] = createPlayer(0,0,700, 64, 60);
	liveEnts[1] = createItem(1, 1100, 250, 1, 0.5);
	liveEnts[2] = createPlatform(2,1152,700, 62, 114);
	
	int levelgrid[GRID_HEIGHT][GRID_WIDTH] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1},
		{0,0,1,0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,1},
		{0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0},
		{0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0},
		{1,1,1,2,2,2,2,1,2,2,2,2,1,2,2,2,1,1,1}
	};
	
	memcpy(grid, levelgrid, sizeof(levelgrid));
}

void level4()
{
	// reset entities
	for (int i = 0; i < LIVE_ENTITY_LENGTH; i++) {
       liveEnts[i] = (LiveEnt){0};
    }

	liveEnts[0] = createPlayer(0,0,700, 64, 62);
    liveEnts[1] = createItem(1, 0, 0, 2, 1);
	liveEnts[2] = createItem(2, 64, 0, 2, 1);
	liveEnts[3] = createItem(3, 128, 0, 2, 1);
    liveEnts[4] = createItem(4, 192, 0, 2, 1);
	liveEnts[5] = createPlatform(5, 193, 765, 62,62);
	
	int levelgrid[GRID_HEIGHT][GRID_WIDTH] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{1,1,1,1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}
	};
	
	memcpy(grid, levelgrid, sizeof(levelgrid));
}

void level5()
{
	// reset entities
	for (int i = 0; i < LIVE_ENTITY_LENGTH; i++) {
       liveEnts[i] = (LiveEnt){0};
    }
	
	liveEnts[0] = createPlayer(0,0,700, 70, 30);
	liveEnts[1] = createItem(1, 0, 0, 1, 2);
	liveEnts[2] = createItem(2, 64, 0, 1, 0.5);
	liveEnts[3] = createPlatform(3,1152,705, 62, 20);
	liveEnts[4] = createMonster(4,900,700, 70, 34);
	liveEnts[5] = createMonster(5,256,0, 70, 34);
	
	int levelgrid[GRID_HEIGHT][GRID_WIDTH] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,2},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
		{1,1,1,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1}
	};
	
	memcpy(grid, levelgrid, sizeof(levelgrid));
	
	PlaySound(sounds[2]);
}




void level6()
{
	// reset entities
	for (int i = 0; i < LIVE_ENTITY_LENGTH; i++) {
       liveEnts[i] = (LiveEnt){0};
    }
	
	liveEnts[0] = createPlayer(0,0,0, 62, 62);
	liveEnts[1] = createItem(1, 1080, 290, 1, 2);
	liveEnts[2] = createMonster(2,256,380, 62, 62);
	liveEnts[3] = createMonster(3,832,600, 124, 124);
	liveEnts[4] = createMonster(4,256,650, 64, 60);

	
	int levelgrid[GRID_HEIGHT][GRID_WIDTH] = {
		{0,0,0,0,0,0,0,2,0,0,0,0,0,2,0,0,0,0,1},
		{0,0,0,0,2,0,0,0,0,0,2,0,0,0,0,0,0,0,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1},
		{0,0,0,0,2,2,0,0,0,0,0,0,0,0,2,2,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
		{0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{0,0,0,2,2,0,0,0,0,0,0,0,0,2,2,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};
	
	memcpy(grid, levelgrid, sizeof(levelgrid));
	
	PlaySound(sounds[2]);
}

void level7()
{
	// reset entities
	for (int i = 0; i < LIVE_ENTITY_LENGTH; i++) {
       liveEnts[i] = (LiveEnt){0};
    }
	
	liveEnts[0] = createPlayer(0,0,700, 60, 60);
	liveEnts[1] = createMonster(1,832,0, 90, 63);
	liveEnts[1].facingRight = false;
	liveEnts[2] = createItem(2, 686, 384, 2, 2);
	liveEnts[3] = createItem(3, 70, 320, 1, 0.5);

	
	int levelgrid[GRID_HEIGHT][GRID_WIDTH] = {
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1},
		{1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,2},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,2},
		{0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,2},
		{0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,0,0,2},
		{1,1,2,2,1,1,1,2,2,1,1,2,2,1,1,1,2,2,2}
	};
	
	memcpy(grid, levelgrid, sizeof(levelgrid));
}

void level8()
{
	// reset entities
	for (int i = 0; i < LIVE_ENTITY_LENGTH; i++) {
       liveEnts[i] = (LiveEnt){0};
    }

	liveEnts[0] = createPlayer(0,0,700, 59, 60);
	liveEnts[1] = createPlatform(1,897 ,127, 58,630);
	liveEnts[2] = createMonster(2,400,0, 110, 64);
	liveEnts[3] = createMonster(3,600,0, 110, 64);
	liveEnts[6] = createItem(6, 644, 320, 0.5, 1);
	liveEnts[7] = createItem(7, 708, 320, 0.5, 1);
	liveEnts[8] = createItem(8, 772, 320, 1, 0.5);
	
	int levelgrid[GRID_HEIGHT][GRID_WIDTH] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
		{1,1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,1,1,1,2,2,2,2,2,1,1,2,2,2,2,2,2,2,1}
	};
	
	memcpy(grid, levelgrid, sizeof(levelgrid));
	
	PlaySound(sounds[2]);

}


void level9()
{
	// reset entities
	for (int i = 0; i < LIVE_ENTITY_LENGTH; i++) {
       liveEnts[i] = (LiveEnt){0};
    }
	
	liveEnts[0] = createPlayer(0,0,700, 57, 57);
	liveEnts[1] = createPlatform(1,1025,650, 26, 254);
	liveEnts[2] = createMonster(2,385,630, 62, 124);
	liveEnts[3] = createItem(3, 1100, 192, 2, 2);
	liveEnts[4] = createItem(4,768, 700, 0.5, 0.5);
	liveEnts[5] = createItem(5, 832, 700, 0.5, 1);
	
	int levelgrid[GRID_HEIGHT][GRID_WIDTH] = {
		{2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,1},
		{2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,1},
		{2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,1},
		{2,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,1},
		{1,1,0,0,2,0,0,0,1,1,1,1,0,0,2,2,2,1,1},
		{2,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,1},
		{2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{2,0,0,1,1,1,0,0,1,1,1,1,1,1,1,1,0,1,1},
		{2,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
		{0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
		{0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};
	
	memcpy(grid, levelgrid, sizeof(levelgrid));
}

void level10()
{
	// reset entities
	for (int i = 0; i < LIVE_ENTITY_LENGTH; i++) {
       liveEnts[i] = (LiveEnt){0};
    }
	
	liveEnts[0] = createPlayer(0,0, 700, 28, 28);
	
	liveEnts[7] = createMonster(7,700, 700, 256, 256);
	liveEnts[7].update = wizardUpdate;
	liveEnts[7].imageIndex = 15;
	liveEnts[7].flippedIndex = 16;

	liveEnts[1] = createItem(1, 16, 64, 1, 0.5);
	liveEnts[2] = createItem(2, 80, 320, 1, 2);
	liveEnts[3] = createItem(3, 1104, 64, 2, 1);
	liveEnts[4] = createItem(4, 1040, 320, 0.5, 1);
	liveEnts[5] = createItem(5, 528, 128, 0.5, 0.5);
	liveEnts[6] = createItem(6, 656, 128, 2, 2);

	
	int levelgrid[GRID_HEIGHT][GRID_WIDTH] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
		{0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};
	
	memcpy(grid, levelgrid, sizeof(levelgrid));
	
	StopMusicStream(musicTracks[1]);
	PlayMusicStream(musicTracks[2]);
	
	PlaySound(sounds[2]);

}

void level11()
{
	// reset entities
	for (int i = 0; i < LIVE_ENTITY_LENGTH; i++) {
       liveEnts[i] = (LiveEnt){0};
    }
	
	liveEnts[0] = createPlayer(0,200, 625, 126, 126);
	liveEnts[1] = createItem(1, 300, 765, 1, 0.5);
	liveEnts[2] = createItem(2, 400, 765, 1, 2);
	liveEnts[3] = createItem(3, 500, 765, 2, 1);
	liveEnts[4] = createItem(4, 600, 765, 0.5, 1);
	liveEnts[5] = createItem(5, 700, 765, 0.5, 0.5);
	liveEnts[6] = createItem(6, 800, 765, 2, 2);

	
	int levelgrid[GRID_HEIGHT][GRID_WIDTH] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};
	
	memcpy(grid, levelgrid, sizeof(levelgrid));
	
	StopMusicStream(musicTracks[2]);
	PlayMusicStream(musicTracks[3]);
	
	PlaySound(sounds[2]);
}




LiveEnt createPlayer(int index, int x, int y, int width, int height)
{
	LiveEnt player = {
		index,true,true,4,0,0,0,x,y,0,0,0,0,0,0, false, width,height,3, playerUpdate, NULL,
		playerOnXCollision,playerOnYCollision, playerOnTileXCollision, playerOnTileYCollision };
		
	return player;
}

LiveEnt createMonster(int index, int x, int y, int width, int height)
{
	LiveEnt mosnter = {
		index,true,true,13,3,0,0,x,y,0,0,0,0,0,0, false, width,height,12, monsterUpdate, NULL,
		playerOnXCollision,playerOnYCollision, playerOnTileXCollision, playerOnTileYCollision };
		
	return mosnter;
}

LiveEnt createPlatform(int index, int x, int y, int width, int height)
{
	LiveEnt platform = {
		index,true,true,5,2,0,0,x,y,0,0,0,0,0,0, false, width,height,5, NULL, NULL,
		playerOnXCollision, playerOnYCollision, playerOnTileXCollision, playerOnTileYCollision };
		
	return platform;
}

LiveEnt createItem(int index, int x, int y, float scaleX, float scaleY)
{
	int imageIndex = 0;
	
	if (scaleX == 2 && scaleY == 1) imageIndex = 6;
	else if (scaleX == 0.5 && scaleY == 1) imageIndex = 7;
	else if (scaleX == 2 && scaleY == 2) imageIndex = 8;
	else if (scaleX == 0.5 && scaleY == 0.5) imageIndex = 9;
	else if (scaleX == 1 && scaleY == 2) imageIndex = 10;
	else if (scaleX == 1 && scaleY == 0.5) imageIndex = 11;
	
	LiveEnt item = {
		index,true,true,imageIndex,1,scaleX,scaleY,x,y,0,0,0,0,0,0, false, TILE_SIZE/2,TILE_SIZE/2,imageIndex, NULL, NULL,
		runeOnXCollision, runeOnYCollision, playerOnTileXCollision, playerOnTileYCollision };
		
	return item;
}


void update()
{
	if (initLevel)
	{
		selectedRune = NULL;
		levels[currentLevel]();
		initLevel = false;
	}
	
	if (currentLevel > 9)
	{
		UpdateMusicStream(musicTracks[3]);

	}
	else if ( currentLevel == 9)
	{
		UpdateMusicStream(musicTracks[2]);
	}
	else
	{
		UpdateMusicStream(musicTracks[1]);
	}
	
	for (int i = 0; i < LIVE_ENTITY_LENGTH; i++)
	{
        if (liveEnts[i].initialised)
		{
			if (liveEnts[i].update) liveEnts[i].update(&liveEnts[i]);
		
			liveEntUpdate(&liveEnts[i]);
		}
		else
		{
			liveEnts[i] = (LiveEnt){0};
		}
    }
}

void render()
{
	for (int x = 0; x < GRID_WIDTH; x++)
	{
        for (int y = 0; y < GRID_HEIGHT; y++)
		{
			int imageIndex = grid[y][x]+1;
			
			if (grid[y][x] == 2) imageIndex = 14;
			
            DrawTexturePro(
				textures[imageIndex],
				(Rectangle){ 0, 0, textures[imageIndex].width, textures[imageIndex].height },
				(Rectangle){ x*TILE_SIZE,y*TILE_SIZE, TILE_SIZE, TILE_SIZE },
				(Vector2){ 0, 0 },
				0.0f,
				WHITE
			);
        }
    }
	
	if (currentLevel == 10)
	{
		DrawTexturePro(
			textures[17],
			(Rectangle){ 0, 0, textures[17].width, textures[17].height },
			(Rectangle){ 0, 0, 1216, 832 },
			(Vector2){ 0, 0 },
			0.0f,
			WHITE
		);
	}
	else if (currentLevel == 11)
	{
		if (!viewingStory) {
			DrawTexturePro(
				textures[18],
				(Rectangle){ 0, 0, textures[18].width, textures[18].height },
				(Rectangle){ 0, 0, 1216, 832 },
				(Vector2){ 0, 0 },
				0.0f,
				WHITE
			);
			
			if (IsKeyDown(KEY_ONE))
			{
				viewingStory = true;
				isHard = false;
			}
			
			if (IsKeyDown(KEY_TWO))
			{
				viewingStory = true;
				isHard = true;
			}
		}
		else{
			DrawTexturePro(
				textures[19],
				(Rectangle){ 0, 0, textures[19].width, textures[19].height },
				(Rectangle){ 0, 0, 1216, 832 },
				(Vector2){ 0, 0 },
				0.0f,
				WHITE
			);
			
			if (IsKeyDown(KEY_ENTER))
			{
				currentLevel = 0;
				initLevel = true;
				
				StopMusicStream(musicTracks[3]);
				PlayMusicStream(musicTracks[1]);
			}
		}
	}
	
	
	for (int i = 0; i < LIVE_ENTITY_LENGTH; i++)
	{
        if (liveEnts[i].initialised)
		{
			if (liveEnts[i].render) liveEnts[i].render(&liveEnts[i]);
		
			liveEntRender(&liveEnts[i]);
		}
		else
		{
			liveEnts[i] = (LiveEnt){0};
		}
    }
	
	switch (currentLevel)
    {
        case 0:
            // Load level 1 resources, setup level 1 objects
            DrawText("WASD or arrow keys to move\n\n\nE to pick up runes\n\n\nLEFT CLICK to throw runes\n\n\nRIGHT CLICK to use runes\n\n\nR to restart the level\n\n\nH to view Zebolios' rune research", 15, 15, 40, LIGHTGRAY);
            break;
		case 1:
            // Load level 2 resources, setup level 2 objects
            DrawText("Avoid the red blocks", 15, 15, 40, LIGHTGRAY);
            break;
        case 2:
            // Load level 2 resources, setup level 2 objects
            DrawText("Throwing runes on objects makes them shrink or grow", 15, 15, 40, LIGHTGRAY);
            break;
        case 3:
            // Load level 3 resources, setup level 3 objects
            DrawText("Runes have different effects based on their appearance", 15, 15, 40, LIGHTGRAY);
            break;
		case 4:
            // Load level 3 resources, setup level 3 objects
            DrawText("Be careful of monsters, unless you're bigger than them", 15, 15, 40, LIGHTGRAY);
            break;
		case 5:
            // Load level 3 resources, setup level 3 objects
            DrawText("", 15, 15, 40, LIGHTGRAY);
            break;
		case 10:
            // Load level 3 resources, setup level 3 objects
            DrawText("you escaped! thanks for playing!", 230, 15, 40, GRAY);
            break;
        default:
            DrawText("", 10, 10, 20, DARKGRAY);
            break;
    }
	
	if (currentLevel == 9 && !liveEnts[7].initialised)
	{
		int levelgrid[GRID_HEIGHT][GRID_WIDTH] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
		{0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};
	
		memcpy(grid, levelgrid, sizeof(levelgrid));
	}

	char buffer[20];

	if (currentLevel < 10)
	{
		sprintf(buffer, "level %i", currentLevel+1);
		DrawText(buffer, 10, 775, 25, LIGHTGRAY);
		
		time += GetFrameTime();
	}
	
	char buffer2[50];
	
	if (currentLevel < 11)
	{
		sprintf(buffer2, "time : %.2f", time);
		DrawText(buffer2, 10, 805, 25, LIGHTGRAY);
	}
	
	if (viewingAnalysis){
		DrawTexturePro(
				textures[20],
				(Rectangle){ 0, 0, textures[20].width, textures[20].height },
				(Rectangle){ 0, 0, 1216, 832 },
				(Vector2){ 0, 0 },
				0.0f,
				WHITE
			);
	}
}

void quit()
{
	return;
}