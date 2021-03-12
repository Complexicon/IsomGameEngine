#include "Map.h"
#include <stdlib.h>

Map::Map() {

	mapArr = new char[512 * 512];
	width = height = 512;
}

void Map::Randomize() {
	for(int i = 0; i < height; i++) {

		for(int j = 0; j < width; j++) { mapArr[(i * height) + j] = rand() % 3; }
	}
}

void Map::LoadMap(char* map) {}
