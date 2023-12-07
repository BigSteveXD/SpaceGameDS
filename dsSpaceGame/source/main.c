/*---------------------------------------------------------------------------------

	Space Game for the Nintendo DS by Steven S.
	December 2023

	Basic template code for creating a DS game made by
	Relminator (Richard Eric M. Lope)
	Http://Rel.Phatcode.Net
	November 2010

	Easy GL2D

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>
#include <stdbool.h>//added
#include <gl2d.h>//easy gl2d
//---------------------------------------------------------------------------------

// GRIT auto-generated files
#include "player.h"
#include "tiles.h"
#include "astroid.h"

// Texture Packer auto-generated UV coords
#include "uvcoord_player.h"
#include "uvcoord_astroid.h"

typedef struct {
	int xSize;
	int ySize;
	int x, y; //object location
	float centerX;//*x + (size * 0.5) //center location //= x + (size * 0.5)
	float centerY;//*y + (size * 0.5) //= y + (size * 0.5)
} Collision;

int playerX;//extern
int playerY;
int playerSize;
int playerHealth = 5;
bool playerInvincible = false;

int enemyX;
int enemyY;
int enemySize;
int enemyHealth = 1;
bool enemyInvincible = false;

int astroidX;
int astroidY;
int astroidSize;
int astroidHealth = 1;
bool astroidInvincible = false;

bool gameOver = false;

int lazerXSize;
int lazerYSize;

int screenX = 256;
int screenY = 192;

// Declare our BG drawing routine
void DrawBG(glImage *images);

void UpdateAnimSet(int *UpdateFrame, int *UpdatePlayer);//, int SetFrame


bool boxCollision(Collision box1, Collision box2);


int main(void) {
//---------------------------------------------------------------------------------
	playerX = 0;
	playerY = 160;//192 - 32
	playerSize = 32;
	int playerFlip = 0;
	int playerSpeed = 2;//1
	//int PlayerGrav = 0;//9
	//int PlayerShots = 3;
	int score = 0;

	enemyX = rand() % 224;//224 - 16
	enemyY = 0;
	enemySize = 32;
	int enemyFlip = 0;
	int enemySpeed = 3;
	int NumEnemy = 0;
	//int enemyGrav = 1;
	
	astroidX = rand() % 224;//256 - 32 - 16 //224 - 16
	astroidY = -32;//32 + 16/2
	astroidSize = 32;
	int astroidFlip = 0;
	int astroidSpeed = 2;

	lazerXSize = 32;
	lazerYSize = 160;

	
	Collision playerCollision;
	playerCollision.xSize = playerSize;
	playerCollision.ySize = playerSize;
	//playerCollision.health = playerHealth;

	Collision enemyCollision;
	enemyCollision.xSize = enemySize;
	enemyCollision.ySize = enemySize;
	//enemyCollision.health = enemyHealth;

	Collision astroidCollision;
	astroidCollision.xSize = astroidSize;
	astroidCollision.ySize = astroidSize;
	//astroidCollision.health = astroidHealth;

	Collision lazerCollision;
	lazerCollision.xSize = lazerXSize;
	lazerCollision.ySize = lazerYSize;


	// Instantiate a glImage object
	//glImage Player[1];  // only a single sprite for now
	// This imagesets would use our texture packer generated coords so it's kinda
	// safe and easy to use 
	// ENEMIES_NUM_IMAGES is a value from "uvcoord_crono.h"
	// ZERO_NUM_IMAGES is a value from "uvcoord_zero.h"
	glImage Player[PLAYER_NUM_IMAGES];	// spriteset

	glImage Astroid[ASTROID_NUM_IMAGES];

	// This tileset won't make use of our texture packer generated coords.
	// Messy, manual and prone to errors.
	// BMP is 256x256 and tiles are 16x16 so.. (256/16) * (256 /16) = 16 * 16
	glImage  Tiles[(256 / 32) * (256 / 32)];//(256 / 16) * (256 / 16)

	//put bg 0 at a lower priority than the text background
	//bgSetPriority(0, 1);


	// Set it to my favorite mode (Any mode that supports 3D should work)
	videoSetMode(MODE_5_3D);

	//original glScreen2D(); spot

	// Init console so that we could
	// print some stuff
	consoleDemoInit();
	//iprintf("Hello World!");

	// Set up enough texture memory for our textures
	// Bank A is 128kb. (enough for our needs)
	// since the sprite we're using is just
	// 2 kb
	vramSetBankA(VRAM_A_TEXTURE);
	// Set up enough texture memory for our textures
	// Bank A is 128kb. Not enough(we are using 164 kb) so we also map
	// Vram B to texture
	vramSetBankB(VRAM_B_TEXTURE);

	// Very important or you'll get black textures
	vramSetBankE(VRAM_E_TEX_PALETTE);  // Allocate VRAM bank for all the palettes //NEEDED so textures don't appear black
	//vramSetBankF(VRAM_F_TEX_PALETTE);  // Allocate VRAM bank for all the palettes //vramSetBankE(VRAM_E_MAIN_BG);	 // Main bg text/8bit bg. Bank E size == 64kb, exactly enough for 8bit * 256 * 192 + text layer

	// Initialize GL2D
	glScreen2D(); //moved here

	// load our single image
    // sprite.
    // Note that glLoadTileset can also
    // load single images.
	//glLoadTileSet(Player,            // pointer to glImage array
	//	32,                // sprite width //16 //64
	//	32,                // sprite height //16 //64
	//	32,                // bitmap image width //16 //64
	//	32,                // bitmap image height //16 //64
	//	GL_RGB16,        // texture type for glTexImage2D() in videoGL.h 16 colors
	//	TEXTURE_SIZE_32,    // sizeX for glTexImage2D() in videoGL.h //16 //64
	//	TEXTURE_SIZE_32,    // sizeY for glTexImage2D() in videoGL.h //16 //64
	//					 // Set texture params setting color 0 as transparent
	//	GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT,
	//	256,                    // Length of the palette to use (256 colors)
	//	(u16*)playerPal,    // Load our 16 color shuttle palette
	//	(u8*)playerBitmap  // image data generated by GRIT
	//);

	// Load our Zero texture
	// We used glLoadSpriteSet since the texture was made
	// with my texture packer.
	// No need for another palette since enemies and zero
	// share the same palette.
	int PlayerTextureID =
		glLoadSpriteSet(Player,					// pointer to glImage array
			PLAYER_NUM_IMAGES, 		// Texture packer auto-generated #define
			player_texcoords,		// Texture packer auto-generated array
			GL_RGB256,				// texture type for glTexImage2D() in videoGL.h 
			TEXTURE_SIZE_128,		// sizeX for glTexImage2D() in videoGL.h //128 //32
			TEXTURE_SIZE_256,		// sizeY for glTexImage2D() in videoGL.h //256 //192
			GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
			256,					// Length of the palette to use (256 colors) //16
			(u16*)playerPal,		// Zero and Enemies share the same palette
			(u8*)playerBitmap	 	// image data generated by GRIT
		);
	int EnemyTextureID =
		glLoadSpriteSet(Player,					// pointer to glImage array ////////////////////////////////////////////////////Enemy Sprite
			PLAYER_NUM_IMAGES, 		// Texture packer auto-generated #define
			player_texcoords,		// Texture packer auto-generated array
			GL_RGB256,				// texture type for glTexImage2D() in videoGL.h 
			TEXTURE_SIZE_128,		// sizeX for glTexImage2D() in videoGL.h //128 //32
			TEXTURE_SIZE_256,		// sizeY for glTexImage2D() in videoGL.h //256 //192
			GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
			256,					// Length of the palette to use (256 colors) //16
			(u16*)playerPal,		// Zero and Enemies share the same palette
			(u8*)playerBitmap	 	// image data generated by GRIT
		);
	int AstroidTextureID =
		glLoadSpriteSet(Astroid,					// pointer to glImage array
			ASTROID_NUM_IMAGES, 		// Texture packer auto-generated #define
			astroid_texcoords,		// Texture packer auto-generated array
			GL_RGB256,				// texture type for glTexImage2D() in videoGL.h 
			TEXTURE_SIZE_32,		// sizeX for glTexImage2D() in videoGL.h //128 //32
			TEXTURE_SIZE_256,		// sizeY for glTexImage2D() in videoGL.h //256 //192 doesnt work
			GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
			256,					// Length of the palette to use (256 colors)
			(u16*)astroidPal,		// Zero and Enemies share the same palette
			(u8*)astroidBitmap	 	// image data generated by GRIT
		);
	

	// Our texture handle for our tiles
	// I used glLoadTileSet since the texture 
	// is just a bunch of 16x16 tiles in a 256x256
	// tileset so we don't need a texture packer for this.
	int TilesTextureID =
		glLoadTileSet(Tiles,				// pointer to glImage array
			32,					// sprite width //16
			32,					// sprite height //16
			256,					// bitmap width
			256,					// bitmap height
			GL_RGB256,			// texture type for glTexImage2D() in videoGL.h 
			TEXTURE_SIZE_256,	// sizeX for glTexImage2D() in videoGL.h
			TEXTURE_SIZE_256,	// sizeY for glTexImage2D() in videoGL.h
			GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
			256,					// Length of the palette to use (256 colors)
			(u16*)tilesPal,		// Load our 256 color tiles palette
			(u8*)tilesBitmap		// image data generated by GRIT
		);


	int Frame = 0;
	int PlayerFrame = 0;//ANIMATION FRAME
	int EnemyFrame = 14;//6
	int AstroidFrame = 0;
	int LazerFrame = 4;//7
	int LazerFrame2 = 4;
	int LazerFrame3 = 4;
	int LazerFrame4 = 4;
	int LazerFrame5 = 4;

	int HealthFrame = 7;

	int enemyTimeDelay = -10;//TIME
	int playerTimeDelay = -10;
	int astroidTimeDelay = -10;

	while(1) {
		Frame++;

		// set up GL2D for 2d mode
		glBegin2D();

		// Set our palette to our tiles (256 colors)
		// and draw our background
		DrawBG(Tiles);


		// USING DIFFERENT PALETTES
			// Set the active texture to Zero
			// and use our special all white palette
		//glSetActiveTexture(PlayerTextureID);
		//glAssignColorTable(0, PaletteID);

		// Normalize color
		//glColor(RGB15(31, 31, 31));
		glSprite(playerX, playerY, playerFlip, &Player[PlayerFrame]);//glSprite(0, 42 * 3, GL_FLIP_V | GL_FLIP_H, &Zero[ZeroFrame]);

		glSprite(playerX, playerY-32, playerFlip, &Player[LazerFrame]);//LAZER
		glSprite(playerX, playerY - 64, playerFlip, &Player[LazerFrame2]);
		glSprite(playerX, playerY - 96, playerFlip, &Player[LazerFrame3]);
		glSprite(playerX, playerY - 128, playerFlip, &Player[LazerFrame4]);
		glSprite(playerX, playerY - 160, playerFlip, &Player[LazerFrame5]);

		glSprite(0, 192-32, playerFlip, &Player[HealthFrame]);

		glSprite(enemyX, enemyY, enemyFlip, &Player[EnemyFrame]);//Enemy Sprite//

		if (enemyY > 192) {//If enemy goes off screen
			enemyY = -32;
			enemyX = rand() % 224;//256 - 32
		}

		if (playerX<0) {//If player goes off screen
			playerX = 256-32;
		}
		if ((playerX+32) > 256) {
			playerX = 0;
		}

		glSprite(astroidX, astroidY, astroidFlip, &Astroid[AstroidFrame]);

		if (astroidY > 192) {
			astroidY = -32;
			astroidX = rand() % 224;//256 - 32
			int tempRan = rand() % 10;
			if (tempRan > 5) {
				astroidFlip = GL_FLIP_NONE;
			}
			else {
				astroidFlip = GL_FLIP_H;
			}
			tempRan = rand() % 10;
			if (tempRan > 5) {
				astroidFlip = GL_FLIP_V;
			}
			else {
				astroidFlip = GL_FLIP_NONE;
			}
		}

		// call Easy GL2D's 2D rendering functions here
		// draw a row of sprites in different flipping

		// end 2D rendering mode
		glEnd2D();

		glFlush(0);
		swiWaitForVBlank();


		astroidY = astroidY + astroidSpeed;//1

		enemyY = enemyY + enemySpeed;//enemyGrav

		scanKeys();
		int pressed = keysDown();
		int keys = keysHeld();
		if (pressed & KEY_START) break;

		if (keys & KEY_RIGHT) {
			playerX = playerX + playerSpeed;//1
		}
		else if (keys & KEY_LEFT) {
			//playerFlip = GL_FLIP_H;//1
			playerX = playerX - playerSpeed;//1
		}
		
		if (keys & KEY_X) {//Teleport Top
			playerSpeed = 4;
		}
		else {
			playerSpeed = 2;
		}

		if (Frame == playerTimeDelay) {//TIME//PLAYER
			playerInvincible = false;
		}
		if (Frame == enemyTimeDelay) {//TIME//ENEMY//
			if (EnemyFrame == 21) {//NEXT FRAME ENEMY //enemyTimeDelay //5
				EnemyFrame = 22;
				UpdateAnimSet(&Frame, &EnemyFrame);
				enemyTimeDelay = Frame + (1 * 15);//180 //60 for 60 fps
			}
			else if (EnemyFrame == 22) {
				EnemyFrame = 23;
				UpdateAnimSet(&Frame, &EnemyFrame);
				enemyTimeDelay = Frame + (1 * 15);//180
			}
			else if (EnemyFrame == 23) {
				EnemyFrame = 24;
				UpdateAnimSet(&Frame, &EnemyFrame);
				enemyTimeDelay = Frame + (1 * 15);//180
			}
			else if (EnemyFrame == 24) {
				EnemyFrame = 25;
				UpdateAnimSet(&Frame, &EnemyFrame);
				enemyTimeDelay = Frame + (1 * 15);//180
			}
			else if (EnemyFrame ==  25) {
				EnemyFrame = 4;
				UpdateAnimSet(&Frame, &EnemyFrame);
				enemyTimeDelay = Frame + (1 * 15);//180
			}
			else if (EnemyFrame == 4) {//RESET ENEMY
				EnemyFrame = 14;//6?
				UpdateAnimSet(&Frame, &EnemyFrame);
				enemyX = rand() % 224;
				enemyY = -32;
				enemySpeed = 3;
				enemyInvincible = false;
			}
		}
		if (Frame == astroidTimeDelay) {
			if (AstroidFrame == 1) {//NEXT FRAME ENEMY //enemyTimeDelay //5
				AstroidFrame = 2;
				UpdateAnimSet(&Frame, &AstroidFrame);
				astroidTimeDelay = Frame + (1 * 15);//180 //60 for 60 fps
			}
			else if (AstroidFrame == 2) {
				AstroidFrame = 3;
				UpdateAnimSet(&Frame, &AstroidFrame);
				astroidTimeDelay = Frame + (1 * 15);//180
			}
			else if (AstroidFrame == 3) {
				AstroidFrame = 4;
				UpdateAnimSet(&Frame, &AstroidFrame);
				astroidTimeDelay = Frame + (1 * 15);//180
			}
			else if (AstroidFrame == 4) {
				AstroidFrame = 5;
				UpdateAnimSet(&Frame, &AstroidFrame);
				astroidTimeDelay = Frame + (1 * 15);//180
			}
			else if (AstroidFrame == 5) {
				AstroidFrame = 0;
				UpdateAnimSet(&Frame, &AstroidFrame);
				astroidX = rand() % 224;
				astroidY = -32;
				astroidSpeed = 1;
				astroidInvincible = false;
			}
		}

		if (keys & KEY_Y) {//LAZER VISIBLE
			LazerFrame = 1;
			UpdateAnimSet(&Frame, &LazerFrame);//, LazerFrame);
			LazerFrame2 = 2;
			UpdateAnimSet(&Frame, &LazerFrame2);
			LazerFrame3 = 2;
			UpdateAnimSet(&Frame, &LazerFrame3);
			LazerFrame4 = 2;
			UpdateAnimSet(&Frame, &LazerFrame4);
			LazerFrame5 = 2;
			UpdateAnimSet(&Frame, &LazerFrame5);
			
			if (boxCollision(lazerCollision, enemyCollision) == true && (enemyInvincible == false)) {//LAZER COLLISION ENEMY //enemyCollision.invincible == false
				enemyHealth = enemyHealth - 1;
				if (enemyHealth <= 0) {//enemyCollision.health <= 0
					enemyInvincible = true;
					EnemyFrame = 21;//5
					UpdateAnimSet(&Frame, &EnemyFrame);
					enemySpeed = 0;
					enemyTimeDelay = Frame + (1 * 15);//RESET TIME //180 //60 for 60 fps
				}
				score = score + 60;
				iprintf("%d\n", score);
			}
			if (boxCollision(lazerCollision, astroidCollision) == true && (astroidInvincible == false)) {//LAZER COLLISION ASTROID
				astroidHealth = astroidHealth - 1;
				if (astroidHealth <= 0) {//astroidCollision.health
					astroidInvincible = true;
					AstroidFrame = 1;//1
					UpdateAnimSet(&Frame, &AstroidFrame);
					astroidSpeed = 0;
					astroidTimeDelay = Frame + (1 * 15);//60 for 60 fps //15 = 60 / 4 = 15 so 4 fps
				}
				//iprintf(astroidCollision.health);
				//iprintf("\nLAZERASTROID ");
				score = score + 30;
				iprintf("%d\n", score);
			}
		}
		else {//LAZER INVISIBLE
			LazerFrame = 4;
			UpdateAnimSet(&Frame, &LazerFrame);
			LazerFrame2 = 4;
			UpdateAnimSet(&Frame, &LazerFrame2);
			LazerFrame3 = 4;
			UpdateAnimSet(&Frame, &LazerFrame3);
			LazerFrame4 = 4;
			UpdateAnimSet(&Frame, &LazerFrame4);
			LazerFrame5 = 4;
			UpdateAnimSet(&Frame, &LazerFrame5);
		}


		//UPDATE LOCATIONS
		playerCollision.x = playerX;
		playerCollision.y = playerY;
		enemyCollision.x = enemyX;
		enemyCollision.y = enemyY;
		astroidCollision.x = astroidX;
		astroidCollision.y = astroidY;
		lazerCollision.x = playerX;
		lazerCollision.y = playerY-160;//playerY-160 //-32


		if ((boxCollision(playerCollision, enemyCollision)==true) && (playerInvincible==false)) {
			playerHealth = playerHealth - 1;
			HealthFrame = HealthFrame + 1;
			UpdateAnimSet(&Frame, &HealthFrame);
			//iprintf("\nENEMY ");
			//iprintf("\nHEALTH %d", playerHealth);
			playerInvincible = true;
			playerTimeDelay = Frame + (5 * 60);//5 sec delay 60 fps * 5 = 5 sec
		}
		if ((boxCollision(playerCollision, astroidCollision)==true) && (playerInvincible==false)) {
			playerHealth = playerHealth - 1;
			HealthFrame = HealthFrame + 1;
			UpdateAnimSet(&Frame, &HealthFrame);
			//iprintf("\nASTROID ");
			//iprintf("\nHEALTH %d", playerHealth);
			playerInvincible = true;
			playerTimeDelay = Frame + (5 * 60);
		}
		if (playerHealth <= 0 && gameOver == false) {
			iprintf("\nGAME OVER\n");
			gameOver = true;
			score = 0;
			playerHealth = 5;
			HealthFrame = 7;
			gameOver = false;
		}
		
	}

}


// Draws the background
void DrawBG(glImage *images)
{
	int x, y, i;

	for (y = 0; y < 256 / 32; y++)//y = 0; y < 256 / 16; y++ //8
	{
		for (x = 0; x < 256 / 32; x++)//x = 0; x < 256 / 16; x++ //8
		{
			i = y * 8 + x;//16 //8 works not 32
			glSprite(x * 32, y * 32, GL_FLIP_NONE, &images[i & 255]);//x * 16, y * 16, GL_FLIP_NONE, &images[i & 255]
		}
	}
}

void UpdateAnimSet(int *UpdateFrame, int *UpdatePlayer) {
	if ((*UpdateFrame & 3) == 0) {
		
	}
}

bool boxCollision(Collision box1, Collision box2) {
	if (box1.x < box2.x + box2.xSize &&
		box1.x + box1.xSize > box2.x &&
		box1.y < box2.y + box2.ySize &&
		box1.ySize + box1.y > box2.y) {
		return true;
	}
	else {
		return false;
	}
}
