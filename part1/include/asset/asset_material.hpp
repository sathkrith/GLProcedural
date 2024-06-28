#ifndef ASSETMAT2_HPP
#define ASSETMAT2_HPP

#include "asset/asset.hpp"
#include "asset/asset_texture.hpp"
#include <glm/vec3.hpp>
#include <unordered_map>

// Represnts a meterial used in the program.
class AssetMaterial: public Asset{
public:
    // Ambient color of the material.
    glm::vec3 ambientColor = glm::vec3(0.5f, 0.5f, 0.5f);
    // Diffuse color of the material.
    glm::vec3 diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);
    // Specular color of the material.
    glm::vec3 specularColor = glm::vec3(0.5f, 0.5f, 0.5f);
    // Shininess of the material.
    float shininess = 0.2f;
    // Transparency of the material.
    float transparency = 0.0f;
    // Index of meterial in material shader.
    std::optional<int> materialShaderIndex;
    // Index of diffuse texture in asset manager.
    std::optional<unsigned short> diffuseMapIndex;
    // Index of specular texture in asset manager.
    std::optional<unsigned short> specularMapIndex;
    // Index of normal map texture in asset manager.
    std::optional<unsigned short> normalMapIndex;
    // Path to diffuse map.
    std::optional<std::filesystem::path> diffuseMapPath;
    // Path to specular map.
    std::optional<std::filesystem::path> specularMapPath;
    // Path to normal map.
    std::optional<std::filesystem::path> normalMapPath;
    // Create a meterial object from the mtl file at the path provided.
    static std::unordered_map<std::string, AssetMaterial> CreateAssetMaterials(AssetType assetType, std::filesystem::path filePath);
    // Assignment operator to ensure that assignment is by reference.
    AssetMaterial& operator=(AssetMaterial& material) { return material; };
    // Default constructor - necessary for maps.
    AssetMaterial():Asset(AssetType::MTL, "") {};
    // Create material for brick.
    static AssetMaterial createBrickMTL();
protected:
    // Create an asset material object from the path specified - note this does not load file and properties
    // Asset materials must be created using CreateAssetMaterial call.
    AssetMaterial(AssetType assetType, std::filesystem::path filePath): Asset(assetType, filePath){};
};
#endif