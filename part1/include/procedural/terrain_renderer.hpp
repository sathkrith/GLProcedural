#pragma once
#include "asset/obj.hpp"
#include "procedural/open_simplex.hpp"
#include "procedural/fault_formation.hpp"
#include "glad/glad.h"
#include "InstancedAssetRenderer.hpp"
/*
* Class to generate quads that can be used to view noise generated.
*/
class TerrainRenderer
{
	InstancedAssetRenderer largeTreeRenderer;
	InstancedAssetRenderer rockRenderer;
	InstancedAssetRenderer grassRenderer;
	InstancedAssetRenderer shrubRenderer;
	InstancedAssetRenderer mediumTreeRenderer;
	InstancedAssetRenderer flowerRenderer;
	InstancedAssetRenderer largeRockRenderer;
public:
	void Initialize(int depth, int width, double height, unsigned int seed, int scale, float filter);
	void Draw();
	void UpdateMesh(int& x, int& z, int& width, std::vector<GLfloat>& mesh, HeightMap& map);
	TerrainRenderer() {};
private:
	// solid color objects vertex array.
	GLuint mVertexArrayObject = 0;
	// solid color objects buffer.
	GLuint mVertexBufferObject = 0;

	GLuint mShader = 0;
	GLuint instanceShaderID = 0;
	GLuint EBO = 0;
	OBJFile floor;
	OpenSimplex noise;
	int terrainTileWidth = 4;
	int terrainTileHeight = 4;
	std::vector<GLuint> indices;
	std::vector<glm::vec3> colorMap;
	GLuint rockTexture = -1;
	GLuint grassTexture = -1;
	GLuint dirtTexture = -1;
	GLuint noiseTexture = -1;
	void LoadTerrains();

	void simulateErosion(HeightMap& terrain, int iterations, float rainAmount, float erosionRate, float depositionRate, float evaporationRate) {
		for (int it = 0; it < iterations; ++it) {
			// Simulate Rainfall
			for (auto& row : terrain) {
				for (auto& cell : row) {
					cell.water += rainAmount; // Add a fixed amount of water to simulate rainfall
				}
			}

			// Calculate water flow across the terrain
			calculateWaterFlow(terrain);

			// Erode the terrain based on water flow
			erodeTerrain(terrain, erosionRate);

			// Handle sediment deposition and water evaporation
			depositSedimentAndEvaporateWater(terrain, evaporationRate, depositionRate);
		}
	}

	void calculateWaterFlow( HeightMap& map) {
		for (int y = 1; y < map.size() - 1; y++) {
			for (int x = 1; x < map[0].size() - 1; x++) {
				float totalHeight = map[y][x].height + map[y][x].water;
				float maxSlope = 0.0f;
				std::vector<std::pair<int, int>> lowestPoints;

				for (int dy = -1; dy <= 1; dy++) {
					for (int dx = -1; dx <= 1; dx++) {
						if (dx != 0 || dy != 0) {
							int nx = x + dx, ny = y + dy;
							float neighborHeight = map[ny][nx].height + map[ny][nx].water;
							float slope = totalHeight - neighborHeight;
							if (slope > maxSlope) {
								maxSlope = slope;
								lowestPoints.clear();
								lowestPoints.emplace_back(nx, ny);
							}
							else if (slope == maxSlope) {
								lowestPoints.emplace_back(nx, ny);
							}
						}
					}
				}

				if (!lowestPoints.empty() && maxSlope > 0) {
					float flowAmount = std::min(maxSlope * map[y][x].water * 0.5f, map[y][x].water / lowestPoints.size());
					for (auto& p : lowestPoints) {
						map[p.second][p.first].flow += flowAmount;
						map[y][x].flow -= flowAmount;
					}
				}
			}
		}

		// Update water map with computed flow
		for (int y = 0; y < map.size(); y++) {
			for (int x = 0; x < map[0].size(); x++) {
				map[y][x].water += map[y][x].flow;
			}
		}
	}

	void erodeTerrain(HeightMap& terrain, float erosionRate) {
		for (int y = 0; y < terrain.size(); y++) {
			for (int x = 0; x < terrain[0].size(); x++) {
				float eroded = erosionRate * sqrt(terrain[y][x].water);  // Erosion proportional to the square root of water volume
				terrain[y][x].height -= eroded;
				terrain[y][x].sediment += eroded;
			}
		}
	}

	void depositSedimentAndEvaporateWater(HeightMap& terrain, float evaporationRate, float depositionRate) {
		for (int y = 0; y < terrain.size(); y++) {
			for (int x = 0; x < terrain[0].size(); x++) {
				terrain[y][x].height += terrain[y][x].sediment * depositionRate;
				terrain[y][x].sediment *= (1 - depositionRate);
				terrain[y][x].water *= (1 - evaporationRate);
			}
		}
	}

};