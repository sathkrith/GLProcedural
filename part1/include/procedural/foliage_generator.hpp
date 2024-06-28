#pragma once
#include "constants.hpp"
#include "open_simplex.hpp"
#include <random>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
// Define Foliage types
enum FoliageType {
    EMPTY = 0,
    TALL_TREE,
    SHORT_TREE,
    SHRUB,
    GRASS,
    SMALL_ROCK,
    LARGE_ROCK,
    FLOWER
};

enum AssetSize {
	SMALL = 0,
	MEDIUM,
	LARGE
};


float rockDensity = 0.06f;
float grassDensity = 0.01f;
float flowerDensity = 0.01f;
float smallRockDensity = 0.06f;
float largeRockDensity = 0.0001f;
float tallTreeDensity = 0.01f;
float shortTreeDensity = 0.01f;
float shrubDensity = 0.001f;
float forestDensity = 0.03f;
float fieldDensity = 0.30f;
float smallAssetDensity = 0.01f;
const int shurbPerUnit = 2;
const int grassPerUnit = 4;
const int flowerPerUnit = 4;
const int smallRockPerUnit = 5;



struct AssetLocation {
    float x, y;  // Normalized coordinates within the unit
};


std::unordered_map<AssetSize, std::vector<AssetLocation>> AssetLocations = {
    {AssetSize::LARGE, {AssetLocation(0.5f, 0.5f)}},
    {AssetSize::MEDIUM, {AssetLocation(0.25f, 0.25f),AssetLocation(0.75f, 0.25f), AssetLocation(0.25f, 0.75f), AssetLocation(0.75f, 0.75f)}},
    {AssetSize::SMALL, {}},
};


struct MapUnit {
    float height;
    std::vector<AssetLocation> tallTrees;
    std::vector<AssetLocation> shortTrees;
    std::vector<AssetLocation> shrubs;
    std::vector<AssetLocation> grass;
    std::vector<AssetLocation> smallRocks;
    std::vector<AssetLocation> largeRocks;
    std::vector<AssetLocation> flowers;
    bool isForest;
    bool isField; 
};
typedef std::vector<std::vector<MapUnit>> FoliageMap;

class ForestGenerator {
private:
    FoliageMap& foliageMap;
    HeightMap& heightMap;
    std::mt19937 rng;
    float maxHeight;
    std::uniform_real_distribution<float> dist;
public:
    ForestGenerator& operator=(const ForestGenerator& other) {
        if (this != &other) {
			foliageMap = other.foliageMap;
			heightMap = other.heightMap;
			rng = other.rng;
			maxHeight = other.maxHeight;
		}
        return *this;
    }
    ForestGenerator(FoliageMap& _foliageMap,
        HeightMap& _heightMap,
        float _maxHeight,
        int seed) : foliageMap(_foliageMap), heightMap(_heightMap), maxHeight(_maxHeight), rng(seed) {
        dist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }

    void generateForestPatch(int centerX, int centerY, double radius) {
        std::uniform_real_distribution<float> dist(0.0, 1.0);
        int count= 0;
        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                int x = centerX + dx;
                int y = centerY + dy;
                if(x<0 || x>= foliageMap[0].size() || y<0 || y>= foliageMap.size()) continue; // Skip if out of bounds)
                if(foliageMap[y][x].isForest) continue; // Skip if already forested)
                float distance = std::sqrt(dx * dx + dy * dy);
                float heightProb = (maxHeight - heightMap[y][x].height) / maxHeight;
                MapUnit& unit = foliageMap[y][x];
                unit.isForest = true;
                if (distance < radius *0.5) {
                    if (dist(rng) < std::min(heightProb, tallTreeDensity)) {  // Central area for tall trees
                        unit.tallTrees.push_back(AssetLocations[AssetSize::LARGE][0]);  // Center of the unit
                        count++;
                    }
                    else if (dist(rng) < std::min(heightProb, shortTreeDensity)/2) {  // Boundary area for short trees
                        unit.shortTrees.push_back(AssetLocations[AssetSize::LARGE][0]);
                    }
                }
                else {
                    if (dist(rng) < std::min(heightProb, tallTreeDensity)/2) {  // Central area for tall trees
                        unit.tallTrees.push_back(AssetLocations[AssetSize::LARGE][0]);  // Center of the unit
                        count++;
                    }
                    else if (dist(rng) < std::min(heightProb, shortTreeDensity)) {  // Boundary area for short trees
                        unit.shortTrees.push_back(AssetLocations[AssetSize::LARGE][0]);
                    }
                }
                
            }
        }
        std:: cout << "Forest generated with " << count << " trees" << std::endl;
    }
};


class FieldGenerator {
private:
    FoliageMap& foliageMap;
    HeightMap& heightMap;
    std::mt19937 rng;
    float maxHeight;
    std::uniform_real_distribution<float> dist;

public:
    FieldGenerator& operator=(const FieldGenerator& other) {
        if (this != &other) {
            foliageMap = other.foliageMap;
            heightMap = other.heightMap;
            rng = other.rng;
            maxHeight = other.maxHeight;
        }
        return *this;
    }
    FieldGenerator(FoliageMap& _foliageMap,
        HeightMap& _heightMap,
        float _maxHeight,
        int seed) : foliageMap(_foliageMap), heightMap(_heightMap), maxHeight(_maxHeight), rng(seed) {
        dist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }

    

    void generateField(int centerX, int centerY, double radius) {
        bool isFlower = dist(rng) > 0.5 ? true : false;
        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                int x = centerX + dx;
                int y = centerY + dy;
                if(x<0 || x>= foliageMap[0].size() || y<0 || y>= foliageMap.size()) continue; // Skip if out of bounds)
                if (foliageMap[y][x].isField || foliageMap[y][x].isForest) continue; // Skip if already a field
                if (x >= 0 && x < foliageMap[0].size() && y >= 0 && y < foliageMap.size()) {
                    float heightProb = (maxHeight - heightMap[y][x].height) / maxHeight;
                    MapUnit& unit = foliageMap[y][x];
                    unit.isField = true;
                    float maxProb = std::min(heightProb, isFlower? flowerDensity: grassDensity);
                    if (dist(rng)< maxProb){
                        if (isFlower)
                            unit.flowers.insert(unit.flowers.begin(), AssetLocations[AssetSize::SMALL].begin(), AssetLocations[AssetSize::SMALL].end());
                        else
                            unit.grass.insert(unit.grass.begin(), AssetLocations[AssetSize::SMALL].begin(), AssetLocations[AssetSize::SMALL].end());
                    }
                }
            }
        }
    }
};


class FoliageGenerator
{
private:
    float width;
    float depth;
    float maxHeight;
    HeightMap& heightMap;
    ForestGenerator forestGen;
    FieldGenerator fieldGen;
    OpenSimplex noise;
    std::mt19937 rng;
    std::uniform_real_distribution<float> floatDist  = std::uniform_real_distribution<float>(0.0f, 1.0f);
    std::uniform_int_distribution<int> intDist = std::uniform_int_distribution<int>(0, 100);
public:
    FoliageMap foliageMap;

    void initializeFoliageSizeLocations() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                AssetLocations.at(AssetSize::SMALL).push_back(AssetLocation(0.1f + i*0.1f, 0.1f + j*0.1f));
            }
            for(int j = 6; j< 10; j++) {
				AssetLocations.at(AssetSize::SMALL).push_back(AssetLocation(0.1f + i*0.1f, 0.1f + j*0.1f));
			}
        }
        for (int i = 6; i < 10; i++) {
            for (int j = 0; j < 4; j++) {
                AssetLocations.at(AssetSize::SMALL).push_back(AssetLocation(0.1f + i * 0.1f, 0.1f + j * 0.1f));
            }
            for (int j = 6; j < 10; j++) {
                AssetLocations.at(AssetSize::SMALL).push_back(AssetLocation(0.1f + i * 0.1f, 0.1f + j * 0.1f));
            }
        }
	}

    FoliageGenerator(int seed, float maxHeight, float depth, float width, HeightMap& heightMap)
        : noise(seed), rng(seed), heightMap(heightMap), width(width), depth(depth), maxHeight(maxHeight)
        , forestGen(foliageMap, heightMap, maxHeight, seed), fieldGen(foliageMap, heightMap, maxHeight, seed)
    {
        foliageMap = std::vector<std::vector<MapUnit>>(depth+1, std::vector<MapUnit>(width+1));
        for(int i = 0; i<= depth; i++){
			for(int j = 0; j<= width; j++){
				foliageMap[i][j].height = heightMap[i][j].height;
			}
		}
        initializeFoliageSizeLocations();
        grassDensity = grassDensity/ (grassDensity + smallRockDensity + flowerDensity);
        flowerDensity = flowerDensity / (smallRockDensity + flowerDensity);
    }

    double getNoise(double x, double y, double scale) {
        return noise.noise(x / scale, y / scale);
    }

    void initializeForests() {
        std::vector<std::thread> threads;
        std::mutex forestMutex; // Mutex to synchronize access to the `forests` vector

        for (int y = 0; y < depth; y += 100) {
            for (int x = 0; x < width; x += 100) {
                threads.emplace_back([&, x, y]() {
                    int numForests = 1 + intDist(rng) % (4); // 0 to 3 forests
                    for (int i = 0; i < numForests; i++) {
                        int centerX =  5 + x + intDist(rng)%(std::min((long)width - x, (long)80));
                        int centerY =  5 + y + intDist(rng)%(std::min((long)depth - y, (long)80));
                        int xBoundary = std::min(std::abs(width - centerX), std::abs(width + 100 - centerX));
                        int yBoundary = std::min(std::abs(depth - centerY), std::abs(depth + 100 - centerY));
                        int maxRadius = 1+ std::min(std::min(xBoundary, yBoundary), 15);
                        int radius = 1+ intDist(rng)%maxRadius; // Forest radius from 10 to 20 units
                        forestGen.generateForestPatch(centerX, centerY, radius);
                    }

                    // Lock and copy local forests to the shared vector
                    });
            }
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
    }

    
    void initializeFields() {
        std::vector<std::thread> threads;
        std::mutex forestMutex; // Mutex to synchronize access to the `forests` vector

        for (int y = 50; y <= depth; y += 100) {
            for (int x = 50; x <= width; x += 100) {
                threads.emplace_back([&, x, y]() {
                    int numFields = intDist(rng) % (5)+1; // 0 to 3 forests

                    for (int i = 0; i < numFields; i++) {
                        int centerX = 5 + x + intDist(rng) % (std::min((long)width - x, (long)80));
                        int centerY = 5 + y + intDist(rng) % (std::min((long)depth - y, (long)80));
                        int xBoundary = std::min(std::abs(width - centerX), std::abs(width + 100 - centerX));
                        int yBoundary = std::min(std::abs(depth - centerY), std::abs(depth + 100 - centerY));
                        int maxRadius = 1+std::min(std::min(xBoundary, yBoundary), 15);
                        int radius = 1 + intDist(rng) % maxRadius; // Forest radius from 10 to 20 units
                        fieldGen.generateField(centerX, centerY, radius);
                    }

                    // Lock and copy local forests to the shared vector
                    });
            }
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
    }

    void populateSmallAssets(MapUnit& unit) {
        int numAssets = intDist(rng) % 4;
        std::unordered_set<int> assetIndices;
        for (int i = 0; i < numAssets; i++) {
			int assetIndex = intDist(rng) % AssetLocations[AssetSize::SMALL].size();
			if (assetIndices.find(assetIndex) == assetIndices.end()) {
				assetIndices.insert(assetIndex);
				unit.smallRocks.push_back(AssetLocations[AssetSize::SMALL][assetIndex]);
			}
		}
        for(int index: assetIndices){
            if(floatDist(rng) < grassDensity){
				unit.grass.push_back(AssetLocations[AssetSize::SMALL][index]);
			}
			else if(floatDist(rng) < flowerDensity){
				unit.flowers.push_back(AssetLocations[AssetSize::SMALL][index]);
			}
            else {
                unit.smallRocks.push_back(AssetLocations[AssetSize::SMALL][index]);
            }
		}
    }

    void distributeIsolatedAssets() {
        for (int y = 0; y <= depth; ++y) {
            for (int x = 0; x <= width; ++x) {
                if (!foliageMap[y][x].isForest && !foliageMap[y][x].isField) {
                    float heightProb = (maxHeight - heightMap[y][x].height) / maxHeight;


					if (floatDist(rng) < std::min(largeRockDensity, heightProb )) {
						foliageMap[y][x].largeRocks.push_back(AssetLocations[AssetSize::LARGE][0]);
                        continue;
					}
                    if(floatDist(rng) < std::min(tallTreeDensity/20, heightProb )) {
						foliageMap[y][x].tallTrees.push_back(AssetLocations[AssetSize::LARGE][0]);
                        continue;
					}
					if (floatDist(rng) < std::min(shortTreeDensity/20, heightProb )) {
						foliageMap[y][x].shortTrees.push_back(AssetLocations[AssetSize::LARGE][0]);
                        continue;
					}
                    for (int i = 0; i < shurbPerUnit; i++){
                        if (floatDist(rng) < std::min(shrubDensity, heightProb)) {
                            foliageMap[y][x].shrubs.push_back(AssetLocations[AssetSize::MEDIUM][0]);
                        }
                    }
                    if(floatDist(rng) < std::min(smallAssetDensity, heightProb )){
						populateSmallAssets(foliageMap[y][x]);
					}
				}
			}
		}
    }

    void generateMap() {
        initializeForests();
        initializeFields();
        distributeIsolatedAssets();
    }
};

