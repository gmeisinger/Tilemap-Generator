#ifndef BANDCAMP_GENERATOR_H_
#define BANDCAMP_GENERATOR_H_

#include <SDL.h>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string>
#include <iostream>

class Generator
{
private:
	int width;
	int height;
	int cur_width;
	int cur_height;
	std::vector< std::vector< int > > map;
	std::vector<SDL_Rect> rooms;
public:
	Generator();
	Generator(int w, int h);
	~Generator();
	std::vector< std::vector< int> > getMap();
	void placeRooms(int attempts);
	void placeRoom(SDL_Rect room, bool fill);
	SDL_Rect genRoom();
	std::vector<std::pair<int,int>> getCorners();
	bool checkOverlap(SDL_Rect room);
	bool growsMap(SDL_Rect room);
	int getNumRooms();
	void cutRoom(SDL_Rect room);
	std::pair<int,int> randomPointInCircle(int radius);
};

#endif  //  BANDCAMP_GENERATOR_H_