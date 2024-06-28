#ifndef ASSETTXT_HPP
#define ASSETTXT_HPP

#include "asset/asset.hpp"

// Represents a texure in the program.
struct AssetTexture: public Asset{
    // Width of the texture image.
    int width;
    // Height of the texture image.
    int height;
    // Max color of the texture image.
    int maxColor;
    // pixel format of the texture image.
    GLenum format = GL_RGB;
    // vector of pixels in the format that can be used to load texture image in opengl.
    std::vector<unsigned char> pixels;
    // Create a texture asset from the image at the path provided.
    static std::optional<AssetTexture> CreateAssetTexture(AssetType assetType, std::filesystem::path filePath, bool loadFile = false);
    // Flip the image vertically.
    void FlipImageVertically();
protected:
    // Create a texture object
    // Does not load the texture from the image at the path.
    AssetTexture(AssetType assetType, std::filesystem::path filePath):width(0), height(0), maxColor(0),Asset(assetType, filePath){};
};
#endif