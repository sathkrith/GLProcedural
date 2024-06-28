#pragma once
#include <vector>
#include "constants.hpp"
class OpenSimplex
{

public:
	OpenSimplex(unsigned int seed = 0);
	double noise(double x, double y);
	HeightMap generateNoiseMap(int width, int height, double scale, int seed);
private:
	std::vector<int> perm;
	std::vector<int> permMod12;
	int grad3[12][2] = {
	{1,1}, {-1,1}, {1,-1}, {-1,-1},
	{1,0}, {-1,0}, {1,0}, {-1,0},
	{0,1}, {0,-1}, {0,1}, {0,-1}
	};;
	int fastFloor(double x);
	double dot(const int* g, double x, double y);
	void generateSection(int yStart, int yEnd, int width, double sensitivity, double clamp, HeightMap &map);
};

