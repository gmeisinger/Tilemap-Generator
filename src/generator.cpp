#include "include/generator.h"

/*

*/

constexpr int ROOM_MAX_SIZE = 15;
constexpr int ROOM_MIN_SIZE = 5;

//default constructor
Generator::Generator() {
	width = 100;
	height = 100;
	cur_width = 0;
	cur_height = 0;
	rooms = {};
	map = std::vector< std::vector< int > >(height, std::vector<int>(width, 0));
	srand(time(NULL));
}

//custom size constructor
Generator::Generator(int w, int h) {
	width = w;
	height = h;
	cur_width = 0;
	cur_height = 0;
	rooms = {};
	map = std::vector< std::vector< int > >(height, std::vector<int>(width, 0));
	srand(time(NULL));
}

//destructor
Generator::~Generator() {
}

//returns the 2d vector representing the map
std::vector< std::vector< int > > Generator::getMap() {
	return map;
}


// makes x attempts to randomly place a room within a circle, discarding it if it overlaps with another room
void Generator::placeRooms(int attempts) {
	
	for(int i=0;i<attempts;i++) {
		//create a random room, stored as a rect
		int room_w = rand() % ROOM_MAX_SIZE + ROOM_MIN_SIZE;
		int room_h = rand() % ROOM_MAX_SIZE + ROOM_MIN_SIZE;
		std::pair<int,int> point = randomPointInCircle(map.size()/2);
		SDL_Rect room = {0, 0, room_w, room_h};

		bool collides = false;
		//check for collisions
		//to ensure there is a wall between rooms we can adjust the params a bit
		//currently treating the room as SMALLER so we can pack it
		SDL_Rect adj_room = {room.x+1, room.y+1, room.w-2, room.h-2};
		for(auto r : rooms) {
			if(SDL_HasIntersection(&adj_room, &r)) {
				collides = true;
				break;
			}
			if((r.x + r.w) > map[0].size() || (r.y + r.h) > map.size()) {
				collides = true;
				break;
			}
		}

		//cut out room
		if(!collides) {
			rooms.push_back(room);
			for(int r=room.y;r<(room.y + room.h);r++){
				for(int c=room.x;c<(room.x + room.w);c++) {
					if(r == room.y || r == (room.y + room.h) || c == room.x || c == (room.x + room.w)) {
						map[r][c] = 2;
					}
					else {
						map[r][c] = 1;
					}	
				}
			}
		}
	}
}

//checks a room against all rooms for overlap
bool Generator::checkOverlap(SDL_Rect room) {
	room.x += 1;
	room.y += 1;
	room.w -= 2;
	room.h -= 2;
	bool collides = false;
	for(auto r : rooms) {
		if(SDL_HasIntersection(&room, &r)) {
			collides = true;
			break;
		}
		if((r.x + r.w) > map[0].size() || (r.y + r.h) > map.size()) {
			collides = true;
			break;
		}
	}
	if(collides) std::cout << "Collides." << std::endl;
	return collides;
}

//place a room using rectangle packing
void Generator::placeRoom(SDL_Rect room, bool fill) {
	if(rooms.size() == 0) {
		std::cout << "First room" << std::endl;
		rooms.push_back(room);
		cur_width = room.w;
		cur_height = room.h;
		cutRoom(room);
	}
	else {
		//find a corner closest to the origin
		/*
			we want a corner like this:

			xx
			xo

			lets search the map and keep a list of coordinates to these corners
		*/
		std::vector<std::pair<int,int>> corners = getCorners();
		//now we check each corner to see if room fits
		for(auto corner : corners) {
			room.x = corner.second;
			room.y = corner.first;
			if(fill && growsMap(room)) {
				//dont use that corner
			}
			else if(!checkOverlap(room)) {
				rooms.push_back(room);
				//cut the room
				cutRoom(room);
				//adjust cur width and height
				if((room.x + room.w) > cur_width) {
					cur_width = room.x + room.w;
				}
				if((room.y + room.h) > cur_height) {
					cur_height = room.y + room.h;
				}
				std::cout << "Placing room:\n" << room.x << " " <<room.y << " " << room.w << " " << room.h << std::endl;
			}
		}
	}
}

bool Generator::growsMap(SDL_Rect room) {
	bool ret = false;
	if((room.x + room.w) > cur_width) {
		ret = true;
	}
	if((room.y + room.h) > cur_height) {
		ret = true;
	}
	return ret;
}

void Generator::cutRoom(SDL_Rect room) {
	for(int r=room.y;r<(room.y + room.h);r++){
		for(int c=room.x;c<(room.x + room.w);c++) {
			if(r == room.y || r == (room.y + room.h -1) || c == room.x || c == (room.x + room.w -1)) {
				map[r][c] = 2;
			}
			else {
				map[r][c] = 1;
			}	
		}
	}
}

//return a list of all corner candidates
std::vector<std::pair<int,int>> Generator::getCorners() {
	std::vector<std::pair<int,int>> corners = {};
	//iterate over map looking for walls
	for(int r=0;r<map.size();r++) {
		for(int c=0;c<map[0].size();c++) {
			if(map[r][c] == 2) {
				//wall found, check if its a valid corner
				if((r == 0 && map[r+1][c] == 2 && map[r+1][c+1] == 0) || (c == 0 && map[r][c+1] == 2 && map[r+1][c+1] == 0) || (map[r][c+1] == 2 && map[r+1][c] == 2 && map[r+1][c+1] == 0)) {
					//valid, add to list
					std::pair<int,int> corner = {r, c};
					corners.push_back(corner);
				}
			}
		}
	}
	std::cout << corners.size() << " corners found." << std::endl;
	return corners;
}

//create a room of random size
SDL_Rect Generator::genRoom() {
	//create a random room, stored as a rect
	int room_w = rand() % ROOM_MAX_SIZE + ROOM_MIN_SIZE;
	int room_h = rand() % ROOM_MAX_SIZE + ROOM_MIN_SIZE;
	int room_x = 0;
	int room_y = 0;
	SDL_Rect rect = {room_x, room_y, room_w, room_h};
	return rect;
}

int Generator::getNumRooms() {
	return rooms.size();
}

/*
	George's helper functions
*/

//gets a random point in a circle (uniformly)
std::pair<int,int> Generator::randomPointInCircle(int radius) {
	float random = float(rand()) / RAND_MAX;
	float r = radius * sqrt(random);
	random = float(rand()) / RAND_MAX;
	float theta = random * 2 * M_PI;
	float x = r * cos(theta);
	float y = r * sin(theta);
	std::pair<int,int> point = { x , y };
	//normalize origin to top left
	point.first += radius;
	point.second += radius;
	return point;
}