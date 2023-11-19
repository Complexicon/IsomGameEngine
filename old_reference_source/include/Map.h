#pragma once

#include "./ResourceManager.h"

class Map {
  private:
	Tile* mapArr;
	int width, height;

  public:
	Map();
	void Generate();
	void LoadMap(Tile* map, unsigned size);
	Tile Get(int x, int y);
	void SetTile(int x, int y, Tile newTile);
	inline Tile* GetMap() { return mapArr; }
	inline int GetMapWidth() { return width; }
	inline int GetMapHeight() { return height; }
};
