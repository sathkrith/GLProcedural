#ifndef ASSET_HPP
#define ASSET_HPP

#include <vector>
#include <glm/glm.hpp>
#include "utils.hpp"
#include <filesystem>

typedef unsigned short AssetID;

// Represents any asset in the program.
struct Asset{
    private:
        // The max asset ID assigned so far.
        static AssetID currentMaxAssetID;
        // Set appropriate asset id for the object. Should not called more than once per object.
        void SetAssetID();
        // ID assigned to the asset. 
        AssetID mAssetID;
    public:
        // State of the asset.
        AssetState mAssetState = AssetState::BAD;
        // Path to the asset in filesystem.
        std::filesystem::path mFilePath;

        // Type of the asset.
        AssetType mAssetType = AssetType::UNKNOWN;
        // Create an asset from parameters.
        Asset(AssetType assetType, std::filesystem::path filePath);
        // Get the ID of the asset.
        AssetID GetAssetID() const;
        // Get the type of the asset.
        AssetType GetAssetType() const;
        // Get the state of the asset.
        AssetState GetAssetState() const;
};
#endif