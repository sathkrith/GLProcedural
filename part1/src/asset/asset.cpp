#include "asset/asset.hpp"

unsigned short Asset::currentMaxAssetID = 0;

Asset::Asset(AssetType assetType, std::filesystem::path filePath): mAssetType(assetType), mFilePath(filePath){
    this->SetAssetID();
}

void Asset::SetAssetID(){
    this->mAssetID = currentMaxAssetID++;
}

unsigned short Asset::GetAssetID() const{
    return this->mAssetID;
}

AssetType Asset::GetAssetType() const{
    return this->mAssetType;
}

AssetState Asset::GetAssetState() const{
    return this->mAssetState;
}