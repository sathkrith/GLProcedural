#include "asset/asset_object.hpp"
#include "asset/obj.hpp"
#include "asset/stl.hpp"

std::optional<std::shared_ptr<AssetObject>> AssetObject::CreateAssetObject(AssetType assetType, std::filesystem::path filePath){
    std::optional<std::shared_ptr<AssetObject>> object;
    switch (assetType)
    {
    case AssetType::OBJ :
        {
            std::optional<OBJFile> obj = OBJFile::LoadOBJ(filePath);
            if(obj.has_value()){
                object = std::make_shared<OBJFile>(obj.value());
            }
            break;
        }
    case AssetType::STL :
        {
            std::optional<STLFile> obj = STLFile::LoadSTL(filePath);
            if(obj.has_value()){
                STLFile shared = obj.value();
                object = std::shared_ptr<STLFile>(&shared);
            }
            break;
        }
    default:
        std::cout << "Invalid type:" << (int)assetType << std::endl;
        break;
    }
    return object;
}