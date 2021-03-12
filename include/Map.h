#pragma once
class Map {
  private:
	char* mapArr;
	int width, height;

  public:
	Map();
	void Randomize();
	void LoadMap(char* map);
	inline char* GetMap() { return mapArr; }
	inline int GetMapWidth() { return width; }
	inline int GetMapHeight() { return height; }
};
