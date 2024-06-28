#ifndef ASSETOBJ_HPP
#define ASSETOBJ_HPP

#include <vector>
#include <glm/glm.hpp>
#include "asset/asset.hpp"
#include "utils.hpp"

// Contains all information required to render an object on the screen.
struct ObjectRenderInfo {
    AssetID assetID = -1;
    size_t meshSize = -1;
    unsigned int vertexBufferOffset = -1;
    glm::vec3 color = glm::vec3(0.0f, 0.5f, 0.0f);
    AssetID materialAssetID = -1;
    AssetID textureAssetID = -1;
    bool hasTexture = false;
    ColorType type = ColorType::color;
};

// Represents an object that can be drawn on screen in the program.
struct AssetObject: public Asset{
protected:
    // Create an object of the type. 
    // The constructor does not load the object.
    AssetObject(AssetType assetType, std::filesystem::path filePath) :Asset(assetType, filePath) {};
public:
    // Path to mtl file if the object is linked to an mtl file.
    std::optional<std::string> mtlPath;
    // Is texture co ordinates included for the asset.
    bool isTextureIncluded = false;

    // Return the mesh of the object.
    std::vector<Triangle> GetMesh() const;
    // Create an asset from the file at the provided filepath.
    static std::optional<std::shared_ptr<AssetObject>> CreateAssetObject(AssetType assetType, std::filesystem::path filePath);
};
#endif