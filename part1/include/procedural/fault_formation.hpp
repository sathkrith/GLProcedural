#pragma once
#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include <random>
#include <constants.hpp>

class FaultFormation
{
private:
	float mMaxHeight;
	int mWidth;
	int mDepth;
	int mNumIterations;
	glm::vec2 point1;
	glm::vec2 point2;	
	float filter;
	void generatePoints();
	void normalize(float minHeight, float maxHeight);
	void applyFIR(float filter);
	float firPoint(int x, int z, float prevVal, float filter);
	std::mt19937 generator; // mt19937 is a Mersenne Twister pseudo-random generator.
	std::uniform_int_distribution<int> wDistribution; // Define range [1, 100]
	std::uniform_int_distribution<int> hDistribution;
public:
	HeightMap mHeightMap;
	FaultFormation(float width, float depth, float maxHeight, int numIterations,float filter);
	void CreateFaultFormation();
};

