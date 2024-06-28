#pragma once
#include <glad/glad.h>
#include <vector>

#include <filesystem>
#include <asset/asset_material.hpp>


class InstancedAssetRenderer
{
public:
    std::unordered_map<std::string, GLuint> vaos;
	GLuint instanceVBO=0;
    std::unordered_map<std::string, GLuint> textures;
    std::unordered_map<std::string, AssetMaterial> materials;
	int instanceCount;
    std::unordered_map < std::string, long> meshSize;
    float width;
    float height;
    float depth;
    float minx = FLT_MIN, miny = FLT_MIN, minz = FLT_MIN;
    float maxx = FLT_MAX, maxy = FLT_MAX, maxz = FLT_MAX;
	std::vector<glm::vec3> instancePositions;
    GLuint shaderID;
    bool LoadObj(const std::filesystem::path& modelPath);
    void setup(const std::filesystem::path& modelPath);
    void updateInstances();
    void updateShader(GLuint shaderID, std::string key);
    void render(GLuint shaderID);
};

