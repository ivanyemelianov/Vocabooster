#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <cmath>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
//Main test color
SDL_Color MAIN_TEXT_COLOR = { 0, 0, 0, 0xFF };

//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Globally used font
TTF_Font* gFont = NULL;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Vocabooster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}

				
			}
		}
	}

	return success;
}

void close()
{

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	SDL_Quit();
}

TTF_Font* loadFont(std::string pathToFont, int fontSize)
{
	// Load our font file and set the font size
	TTF_Font* font = TTF_OpenFont(pathToFont.c_str(), fontSize);

	// Confirm that it was loaded
	if (font == nullptr) {
		std::cout << "Could not load font" << std::endl;
		exit(1);
	}

	return font;
}

int main(int argc, char* args[])
{
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;


		//Load font
		TTF_Font* font = loadFont("Montserrat-Regular.ttf", 32);


		//Field label text
		SDL_Surface* surfaceText = TTF_RenderText_Solid(font, "Write in the input field below:", MAIN_TEXT_COLOR);
		SDL_Texture* textureText = SDL_CreateTextureFromSurface(gRenderer, surfaceText);
		SDL_FreeSurface(surfaceText);

		//The current input text
		std::string inputText = "Some text";
		SDL_Surface* surfaceInput = TTF_RenderText_Solid(font, inputText.c_str(), MAIN_TEXT_COLOR);
		SDL_Texture* textureInput = SDL_CreateTextureFromSurface(gRenderer, surfaceInput);
		SDL_FreeSurface(surfaceInput);

		//Enable text input
		SDL_StartTextInput();

		// Create a rectangle to draw on
		SDL_Rect rectangle;
		rectangle.x = 200;
		rectangle.y = 200;
		rectangle.w = 250;
		rectangle.h = 25;

		SDL_Rect inputRect;
		inputRect.x = 200;
		inputRect.y = 250;
		inputRect.w = 150;
		inputRect.h = 25;

		//While application is running
		while (!quit)
		{
			//The rerender text flag
			bool renderText = false;

			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
				//Special key input
				else if (e.type == SDL_KEYDOWN)
				{
					//Handle backspace
					if (e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
					{
						//lop off character
						inputText.pop_back();
						renderText = true;
					}
					//Handle copy
					else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
					{
						SDL_SetClipboardText(inputText.c_str());
					}
					//Handle paste
					else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
					{
						inputText = SDL_GetClipboardText();
						renderText = true;
					}
				}
				//Special text input event
				else if (e.type == SDL_TEXTINPUT)
				{
					//Not copy or pasting
					if (!(SDL_GetModState() & KMOD_CTRL && (e.text.text[0] == 'c' || e.text.text[0] == 'C' || e.text.text[0] == 'v' || e.text.text[0] == 'V')))
					{
						//Append character
						inputText += e.text.text;
						renderText = true;
					}
				}
			}

			//Rerender text if needed
			if (renderText)
			{
				//Text is not empty
				if (inputText != "")
				{
					//Render text
					SDL_Surface* surfaceInput = TTF_RenderText_Solid(font, inputText.c_str(), MAIN_TEXT_COLOR);
					SDL_Texture* textureInput = SDL_CreateTextureFromSurface(gRenderer, surfaceInput);
					SDL_FreeSurface(surfaceInput);
				}
				//Text is empty
				else
				{
					//Render space texture
					SDL_Surface* surfaceInput = TTF_RenderText_Solid(font, " ", MAIN_TEXT_COLOR);
					SDL_Texture* textureInput = SDL_CreateTextureFromSurface(gRenderer, surfaceInput);
					SDL_FreeSurface(surfaceInput);
				}
			}


			//Clear screen
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(gRenderer);

			// Render our text on a rectangle
			SDL_RenderCopy(gRenderer, textureText, NULL, &rectangle);
			SDL_RenderCopy(gRenderer, textureInput, NULL, &inputRect);

			//Update screen
			SDL_RenderPresent(gRenderer);
		}

		//Disable text input
		SDL_StopTextInput();
	}

	//Free resources and close SDL
	close();

	return 0;
}
	
