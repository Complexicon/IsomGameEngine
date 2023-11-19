#include "pch.h"
#include "Map.h"
#include "Perlin.h"
#include "string.h"
#include <stdlib.h>
#include <vector>

using std::vector;

struct Wave {
	float seed, frequency, amplitude;
};

struct Biome {
	vector<Tile> tiles;
	float minHeight, minMoisture, minHeat;
};

const Biome biomes[] = {
	{{Tile::SAND}, 0.2, 0, 0.5 },				 // Desert
	{{Tile::GRASS}, 0.3, 0.5, 0.3 },	 // Grassland , Tile::DIRT
	{{Tile::DIRT}, 0.2, 0.4, 0.4}, // Forest, Tile::DIRT
	{{Tile::SNOW}, 0.65, 0, 0.3},					 // Mountain
	{{Tile::GRAVEL}, 0.6, 0, 0.2},					 // Mountain
	{{Tile::WATER}, 0, 0, 0 },					 // Oean
};

inline static vector<float> GenerateNoise(int width, int height, float scale, vector<Wave> waves, fV2D offset) {
	vector<float> noiseMap;
	noiseMap.reserve(width * height);


	siv::BasicPerlinNoise<float> perlin { (siv::PerlinNoise::seed_type)rand() };

	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			float samplePosX = (float)x * scale + offset.x;
			float samplePosY = (float)y * scale + offset.y;

			float normalization = 0.0f;

			for(auto& wave : waves) {
				noiseMap[y * height + x] += wave.amplitude * perlin.noise2D_01(samplePosX * wave.frequency + wave.seed,
																			   samplePosY * wave.frequency + wave.seed);
				normalization += wave.amplitude;
			}

			noiseMap[y * height + x] /= normalization;
		}
	}

	return noiseMap;
}

static inline Tile GetTile(float height, float moisture, float heat) {

	vector<const Biome*> possibleBiomes;

	for(auto& biome : biomes) {
		if(height >= biome.minHeight && moisture >= biome.minMoisture && heat >= biome.minHeat) {
			possibleBiomes.push_back(&biome);
		}
	}

	const Biome* selected = nullptr;

	float curDiff = 0;

	auto diff = [&](const Biome& biome) {
		return 5*(height - biome.minHeight) + 1.2*(moisture - biome.minMoisture) + 1.3*(heat - biome.minHeat);
	};

	for(auto possible : possibleBiomes) {
		if(selected == nullptr) {
			selected = possible;
			curDiff = diff(*possible);
		} else if(diff(*possible) < curDiff) {
			selected = possible;
			curDiff = diff(*possible);
		}
	}

	if(selected == nullptr) selected = &biomes[0];

	return selected->tiles[0]; // rand() & selected->tiles.size()
}

Map::Map() {
	width = height = 300;
	mapArr = new Tile[width * height];
}

const vector<Wave> heightWaves = {{56, 0.05, 1}, {199, 0.2, 0.5}};
const vector<Wave> moistWave = {{544, 0.2, 0.5}};
const vector<Wave> heatWaves = {{123.23, 0.04, 1}, {467.66, 0.01, 0.6}};

void Map::Generate() {

	auto heightMap = GenerateNoise(width, height, 0.1, heightWaves, {0, 0});
	auto moistMap = GenerateNoise(width, height, 0.1, moistWave, {0, 0});
	auto heatMap = GenerateNoise(width, height, 0.1, heatWaves, {0, 0});

	for(int i = 0; i < height; i++) {

		for(int j = 0; j < width; j++) {
			mapArr[(i * height) + j] =
				GetTile(heightMap[(i * height) + j], moistMap[(i * height) + j], heatMap[(i * height) + j]);
		}
	}
}

Tile Map::Get(int x, int y) {
	if(x >= width || y >= height || x < 0 || y < 0) return Tile::NONE;
	return mapArr[y * width + x];
}

void Map::SetTile(int x, int y, Tile newTile) {
	if(x >= width || y >= height || x < 0 || y < 0) return;
	mapArr[y * width + x] = newTile;
}

void Map::LoadMap(Tile* map, unsigned size) {
	mapArr = new Tile[size * size];
	memcpy(mapArr, map, size * size);
	width = height = size;
}
