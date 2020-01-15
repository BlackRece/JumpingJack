#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <iostream>

#include "Constants.h"

using namespace std;

/*
NOTE: For mouse targeting - rotate from current pos to mouse pos
future maths: rotate to point (cangle between two points)
using Mathf.Atan2 with 2d points, do;
y2-y1 and x2-x1
this will give (in radians) the angle to rotate to meet the target point

The alternative would be to just always target the mouse...

Either way, if player object is to target the mouse when directly above or below,
then face the direction of current/last movemeant. For example, if player was moving left,
then use left-facing sprite. If player moves mouse beyond that point, turn player object round 
by using the correct sprite.
*/

/*
Sprite Sheet Info: 
Target Sprite Size: 32w x 64h

File: player_spritesheet.png
Target: Player
Idle Pose:
	Sprite Size: 32w x 64h
	Position = 0, 0
	1 frame.
Running:
	Sprite Size: 32w x 64h
	Position = x33, y0.
	Total Area = 198w.
	6 frames.
Jumping:
	Sprite Size: 32w x 64h
	Position = x231.
	Total Area = 131w.
	4 frames. (Frame width: 131 / 4 = 32?)
Lie Down:
	Sprite Size: 64w x 64h
	Position = x363.
	Total Area = 259w.
	4 frames.

File: zombies.png
Target: Enemies
Sprite Size: 32w x 48h
Idle Pose (All 1 frame):
	M1: x32, y0
	M2: x128, y0
	M3: x224, y0
	M4: x320, y0
	F1: x32, y48
	F2: x128, y48
	F3: x224, y48
	F4: x320, y48
Walking:
	Total Area = 96w
	M1: x0, y0
	M2: x96, y0
	M3: x192, y0
	M4: x288, y0
	F1: x0, y48
	F2: x96, y48
	F3: x192, y48
	F4: x288, y48
*/

//Glogbal Variables
//The game window 
SDL_Window* gWindow = NULL;

//The game window contents as a texture
SDL_Renderer* gRenderer = NULL;

//The surface of the window
SDL_Surface* gScreenSurface = NULL;

//The image to be loaded and shown
SDL_Surface* gHelloWorld = NULL;

//The textures to be loaded and shown
SDL_Texture* gPlayer = NULL;
SDL_Texture* gEnemies = NULL;
SDL_Texture* gBackground = NULL;

//Animation frame counter

//Function Prototypes
void CloseSDL();
void Draw();
bool InitSDL();
bool LoadMedia();
SDL_Surface* LoadSurface(std::string path);
SDL_Texture* LoadTexture(std::string path);
SDL_Texture* LoadTexture(std::string path, int cRed, int cGreen, int cBlue);
bool Update();

int main(int argc, char* args[]) {

	//Check if SDL was set up correctly.
	if (InitSDL()) {
		//Flag to check if we wish to quit.
		bool quit = false;

		//Load assets (eg graphics and sounds).
		if (!LoadMedia()) {
			//Something didn't load
			cout << "\nUnable to load media." << endl;
			quit = true;
		} else {
			
		}

		//Game Loop.
		while (!quit) {
			quit = Update();
			Draw();
		}
	}

	//Close Window and free resources.
	CloseSDL();

	return 0;
}

void CloseSDL() {
	//Deallocate loaded surface
	if (gHelloWorld != NULL) {
		SDL_FreeSurface(gHelloWorld);
		gHelloWorld = NULL;
	}

	//Deallocate loaded textures
	if (gBackground != NULL) {
		SDL_DestroyTexture(gBackground);
		gBackground = NULL;
	}

	if (gPlayer != NULL) {
		SDL_DestroyTexture(gPlayer);
		gPlayer = NULL;
	}

	if (gEnemies != NULL) {
		SDL_DestroyTexture(gEnemies);
		gEnemies = NULL;
	}

	//Release the window.
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gRenderer = NULL;
	gWindow = NULL;

	//Quit SDL subsystems.
	//TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Draw() {
	/*
	Using surfaces

	//Create Rect that defines the image dimensions required
	SDL_Rect stretchRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	//Apply the image stretched
	SDL_BlitScaled(gHelloWorld, NULL, gScreenSurface, &stretchRect);
	
	//Done by prev statement
	//Draw image to screen
	SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
	
	
	//Update/refresh the screen
	SDL_UpdateWindowSurface(gWindow);
	*/

	//Using textures
	//Set colour to clear screen/window
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

	//Clear screen/window
	SDL_RenderClear(gRenderer);

	//Render background texture to screen
	SDL_RenderCopy(gRenderer, gBackground, NULL, NULL);

	//Render player texture to screen
	SDL_Rect renderRect = { 0, 0, 32, 64 };
	SDL_Rect positionRect = { 20, 20, 32, 64 };
	SDL_RenderCopy(gRenderer, gPlayer, &renderRect, &positionRect);

	//Render enemy texture to screen
	renderRect = { 128, 48, 32, 48 };
	positionRect = { 60, 20, 32, 64 };
	SDL_RenderCopy(gRenderer, gEnemies, &renderRect, &positionRect);

	//Update screen/window
	SDL_RenderPresent(gRenderer);
}

bool InitSDL() {
	//Initialisation Flag
	bool success = true;

	//Setup SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		cout << "\nSDL did not initialise. Error: " << SDL_GetError();
		success = false;
	} else {
		//All good, so attempt to create the window.
		gWindow = SDL_CreateWindow("SDL Template",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN);

		//Did the window get created?
		if (gWindow == NULL) {
			
			//Nope.
			cout << "\nWindow was not created. Error: " << SDL_GetError();
			success = false;
		} else {

			//Create window renderer
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL) {
				cout << "\nRenderer could not be created! SDL Error: " << SDL_GetError();
				success = false;
			} else {
				//Initialise renderer colour (Black is default. Uses RGBA)
				SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

				//Initialise PNG loading
				if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
					cout << "\nSDL_image could not initialise! SDL_image Error: " << IMG_GetError();
					success = false;
				} else {
					//Get window surface
					gScreenSurface = SDL_GetWindowSurface(gWindow);
				}

			}
		}

	}

	return success;
}


bool LoadMedia() {
	//Loading success flag
	bool success = true;

	//Load splash image
	/*
	//gHelloWorld = SDL_LoadBMP("gfx/Jumping_Jack_Splash.bmp");
	//gHelloWorld = LoadSurface("gfx/Jumping_Jack_Splash.png");
	if (gHelloWorld == NULL) {
		cout << "\nUnable to load image! Error: " << SDL_GetError();
		success = false;
	}
	*/

	gPlayer = LoadTexture("gfx/player_RGBA.png");// , 64, 128, 0);
	if (gPlayer == NULL) {
		cout << "\nFailed to load player texture image!" << endl;
		success = false;
	}

	gEnemies = LoadTexture("gfx/zombies_RGBA.png");// , 0x78, 0xC3, 0x80);
	if (gEnemies == NULL) {
		cout << "\nFailed to load enemy texture image!" << endl;
		success = false;
	}

	gBackground = LoadTexture("gfx/Jumping_Jack_Splash.png");
	if(gBackground == NULL) {
		cout << "\nFailed to load background texture image!" << endl;
		success = false;
	}

	return success;
}

SDL_Surface* LoadSurface(std::string path) {
	//The final optimised image
	SDL_Surface* optimisedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		cout << "\nUnable to load image " << path << "! SDL_image Error: " << IMG_GetError();
	} else {
		//Convert surface to screen format
		optimisedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
		if (optimisedSurface == NULL) {
			cout << "\nUnable to optimise image " << path << "! SDL_image Error: " << IMG_GetError();
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return optimisedSurface;
}

SDL_Texture* LoadTexture(std::string path, int cRed, int cGreen, int cBlue) {
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image from specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		cout << "\nUnable to load image " << path << "! SDL_image Error: " << IMG_GetError();
	}
	else {
		//Colour key image
		//SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, cRed, cBlue, cGreen));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL) {
			cout << "\nUnable to create texture from " << path << "! SDL_Error: " << SDL_GetError();
		}

		//Clean up loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

SDL_Texture* LoadTexture(std::string path) {
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image from specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		cout << "\nUnable to load image " << path << "! SDL_image Error: " << IMG_GetError();
	}
	else {
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL) {
			cout << "\nUnable to create texture from " << path << "! SDL_Error: " << SDL_GetError();
		}

		//Clean up loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

bool Update() {
	//Event Handle.
	SDL_Event e;

	//Get the events.
	SDL_PollEvent(&e);

	//Handle any events.
	switch(e.type) {
	
	//Click the 'X' to quit.
	case SDL_QUIT:
		return true;
		break;

	//A key was released
	case SDL_KEYUP:
		switch (e.key.keysym.sym) {
		//Escape was released
		case SDLK_ESCAPE:
			return true;
			break;

		}
		break;

	//A mouse button was pressed
	case SDL_MOUSEBUTTONDOWN:
		switch (e.button.button) {
		//Left mouse button was pressed
		case 1:
			return true;
			break;
		//Middle mouse button was pressed
		case 2:
			return true;
			break;
		//Right mouse button was pressed
		case 3:
			return true;
			break;
		}
		break;
	}

	return false;
}