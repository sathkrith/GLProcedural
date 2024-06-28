#ifndef PPM_HPP
#define PPM_HPP

#include "asset/asset_texture.hpp"

// PPM texture loader.
struct PPM: public AssetTexture{
    // Open PPM file and return a PPM texture.
    static std::optional<PPM> LoadPPM(std::filesystem::path filePath);
    // Check whether the file at filepath is a valid asciee PPM.
    static bool isAscii(std::filesystem::path filePath);
    static GLuint loadTextureToRender(std::filesystem::path path, bool& isLoadSuccessfull);
    // Create a PPM object. 
    PPM(std::filesystem::path filePath):AssetTexture(AssetType::PPM, filePath){}
};

#endif