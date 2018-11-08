#include <iostream>
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

#include "include/generator.h"

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 640;
constexpr int TILE_SIZE = 8;

//globals
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

//functions
bool init();
void close();

bool init() {
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}

	if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		std::cout << "Warning: Linear texture filtering not enabled!" << std::endl;
	}
	
	gWindow = SDL_CreateWindow("Bandcamp", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == nullptr) {
		std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return  false;
	}

	// Adding VSync to avoid absurd framerates
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (gRenderer == nullptr) {	
		std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return  false;
	}
	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if( !( IMG_Init( imgFlags ) & imgFlags ) )
	{
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
		return false;
	}
	// Set renderer draw/clear color
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	
	//Start the GSM	
	return true;
}

void close() {
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;

	// Quit SDL subsystems
	SDL_Quit();
}

void drawGrid(std::vector<std::vector<int>> map) {
	int tw = SCREEN_WIDTH / map[0].size();
	int th = SCREEN_HEIGHT / map.size();
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	for(int r=0;r<map.size();r++) {
		SDL_RenderDrawLine(gRenderer, 0, r*th, SCREEN_WIDTH, r*th);
	}
	for(int c=0;c<map[0].size();c++) {		
		SDL_RenderDrawLine(gRenderer, c*tw, 0, c*tw, SCREEN_HEIGHT);
	}
}

void drawMap(std::vector<std::vector<int>> map) {
	int tw = SCREEN_WIDTH / map[0].size();
	int th = SCREEN_HEIGHT / map.size();

	for(int r=0;r<map.size();r++) {
		for(int c=0;c<map[r].size();c++) {
			SDL_Rect tile = { c * tw, r * th, tw, th };
			if(map[r][c] == 1) {
				//this is a floor
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
			}
			else if(map[r][c] == 2) {
				//wall
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
			}
			else {
				//black
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
			}
			SDL_RenderFillRect(gRenderer, &tile);
			//SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0x50);
			//SDL_RenderDrawRect(gRenderer, &tile);
		}
	}
}

void georgeAlg(Generator gen) {
	int wid = SCREEN_WIDTH/TILE_SIZE;
	int hei = SCREEN_HEIGHT/TILE_SIZE;
	
	for(int i=0;i<3;i++) {
		gen.placeRoom(gen.genRoom(), false);
		//display map with rects
		std::vector<std::vector<int>> map = gen.getMap();
		drawMap(map);
		//drawGrid(map);
		SDL_RenderPresent(gRenderer);
		SDL_Delay(100);
	}
	for(int i=0;i<5;i++) {
		gen.placeRoom(gen.genRoom(), true);
		//display map with rects
		std::vector<std::vector<int>> map = gen.getMap();
		drawMap(map);
		//drawGrid(map);
		SDL_RenderPresent(gRenderer);
		SDL_Delay(100);
	}
	
}

int main(int argc, char* argv[]) {

	//init sdl
	if(!init()) return 1;
	bool running = true;
	SDL_Event e;
	//create generator
	int wid = SCREEN_WIDTH/TILE_SIZE;
	int hei = SCREEN_HEIGHT/TILE_SIZE;
	Generator gen = Generator(wid, hei);
	for(int i=0;i<10;i++) {
		georgeAlg(gen);
		SDL_Delay(3000);	
	}
	

	//display map with rects
	//drawMap(map);
	//drawGrid(map);
	//SDL_RenderPresent(gRenderer);

	//main loop
	while(running) {
		//handle events on queue
		while(SDL_PollEvent(&e) != 0) {
			//user requests quit
			if(e.type == SDL_QUIT) {
				running = false;
			}
		}
	}
	close();
}