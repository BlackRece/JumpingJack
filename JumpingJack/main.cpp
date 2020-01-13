#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <iostream>

#include "Constants.h"

using namespace std;

//Glogbals
SDL_Window* gWindow = NULL;

//Function Prototypes
bool InitSDL();
void CloseSDL();

int main(int argc, char* args[]) {

	//Check if SDL was set up correctly.
	if (InitSDL()) {
		//Pause for a few seconds.
		SDL_Delay(5000);
	}

	//Close Window and free resources.
	CloseSDL();

	return 0;
}

bool InitSDL() {
	//Setup SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		cout << "SDL did not initialise. Error: " << SDL_GetError();
		return false;
	}
	else {
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
			cout << "Window was not created. Error: " << SDL_GetError();
			return false;
		}

	}

	return true;
}

void CloseSDL() {
	//Release the window.
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems.
	IMG_Quit();
	SDL_Quit();
}
