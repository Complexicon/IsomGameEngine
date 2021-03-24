#include "Map.h"
#include "string.h"
#include <stdlib.h>

Map::Map() {
	width = height = 256;
	mapArr = new char[width * height];
}

void Map::Randomize() {
	for(int i = 0; i < height; i++) {

		for(int j = 0; j < width; j++) { mapArr[(i * height) + j] = rand() % 3; }
	}
}

char Map::Get(int x, int y) {
	if(x >= width || y >= height || x < 0 || y < 0) return -1;
	return mapArr[y * width + x];
}

void Map::LoadMap(char* map, unsigned size) {
	mapArr = new char[size * size];
	memcpy(mapArr, map, size * size);
	width = height = size;
}
