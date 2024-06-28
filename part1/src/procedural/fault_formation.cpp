#include "procedural/fault_formation.hpp"
#include <random>
using namespace std;
using namespace glm;
float getMin(HeightMap &vec) {
	float min = 10000000.0;
	for (int i = 0; i < vec.size(); i++) {
		for (int j = 0; j < vec[i].size(); j++) {
			if (vec[i][j].height < min)
				min = vec[i][j].height;
		}
	}
	return min;
}

void FaultFormation::CreateFaultFormation()
{
	double max = -1;
	for (int i = 0; i < mNumIterations; i++) {
		double iterHeight = (((float)(mNumIterations - i)) / mNumIterations) * mMaxHeight;
		generatePoints();
		int dirX = point2.x - point1.x;
		int dirZ = point2.y - point1.y;
		for (int z = 0; z <= mDepth; z++) {
			for (int x = 0; x <= mWidth; x++) {
				int nDirX = x - point1.x;
				int nDirZ = z - point1.y;
				int crossP = nDirX * dirZ - nDirZ * dirX;
				if (crossP > 0) {
					mHeightMap[z][x].height += iterHeight;
				}
				if (mHeightMap[z][x].height > max)
					max = mHeightMap[z][x].height;
			}
		}
	}
	float min = getMin(mHeightMap);
	normalize(min, max);
	applyFIR(filter);
}

void FaultFormation::generatePoints()
{
	point1 = vec2(wDistribution(generator), hDistribution(generator));
	int c = 0;
	while (c < 1000) {
		point2 = vec2(wDistribution(generator), hDistribution(generator));
		c++;
		if (point1.x != point2.x || point1.y != point2.y)
			break;
	} 
}


void FaultFormation::normalize(float min, float max)
{
	float actRange = max - min;
	float reqRange = mMaxHeight;
	for (int z = 0; z <= mDepth; z++) {
		for (int x = 0; x <= mWidth; x++) {
			mHeightMap[z][x].height = (((mHeightMap[z][x].height - min) / actRange) * reqRange)/mMaxHeight;
		}
	}
}

void FaultFormation::applyFIR(float filter)
{
	for (int z = 0; z <= mDepth; z++) {
		float preV = mHeightMap[0][z].height;
		for (int x = 0; x <= mWidth; x++) {
			preV = firPoint(x, z, preV, filter);
		}
	}

	for (int z = 0; z <= mDepth; z++) {
		float preV = mHeightMap[0][z].height;
		for (int x = mWidth; x >=0 ; x--) {
			preV = firPoint(x, z, preV, filter);
		}
	}

	for (int x = 0; x <= mWidth; x++) {
		float preV = mHeightMap[x][0].height;
		for (int z = 0; z <= mDepth; z++) {
			preV = firPoint(x, z, preV, filter);
		}
	}

	for (int x = 0; x <= mWidth; x++) {
		float preV = mHeightMap[x][0].height;
		for (int z = mDepth; z >=0; z--) {
			preV = firPoint(x, z, preV, filter);
		}
	}
}

float FaultFormation::firPoint(int x, int z, float prevVal, float filter)
{
	float val = mHeightMap[x][z].height;
	float nVal = filter * prevVal + (1 - filter) * val;
	mHeightMap[x][z].height = nVal;
	return nVal;
}

FaultFormation::FaultFormation(float width, float depth, float maxHeight, int numIterations, float filter)
{
	mDepth = depth;
	mMaxHeight = maxHeight;
	mWidth = width;
	mNumIterations = numIterations;
	this->filter = filter;
	mHeightMap = vector<vector<Cell>>(width+1, vector<Cell>(depth+1));
	std::mt19937 generator(100);
	this->generator = generator;
	std::uniform_int_distribution<int> distribution(0, width);
	wDistribution = distribution;
	std::uniform_int_distribution<int> distribution2(0, depth);
	hDistribution = distribution2;
}
