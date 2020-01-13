#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <iostream>

#include "Constants.h"

using namespace std;

//Glogbal Variables
//The game window 
SDL_Window* gWindow = NULL;

//The game window contents as a texture
SDL_Renderer* gRenderer = NULL;

//The surface of the window
SDL_Surface* gScreenSurface = NULL;

//The image to be loaded and shown
SDL_Surface* gHelloWorld = NULL;

//The texture to be loaded and shown
SDL_Texture* gBackground = NULL;

//Function Prototypes
void CloseSDL();
void Draw();
bool InitSDL();
bool LoadMedia();
SDL_Surface* LoadSurface(std::string path);
SDL_Texture* LoadTexture(std::string path);
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

	//Deallocate loaded texture
	if (gBackground != NULL) {
		SDL_DestroyTexture(gBackground);
		gBackground = NULL;
	}

	//Release the window.
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gRenderer = NULL;
	gWindow = NULL;

	//Quit SDL subsystems.
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
	//Clear screen/window
	SDL_RenderClear(gRenderer);

	//Render texture to screen
	SDL_RenderCopy(gRenderer, gBackground, NULL, NULL);

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

	gBackground = LoadTexture("gfx/Jumping_Jack_Splash.png");
	if(gBackground == NULL) {
		cout << "\nFailed to load texture image!" << endl;
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