#include "asset/asset_texture.hpp"
#include "asset/ppm.hpp"

std::optional<AssetTexture> AssetTexture::CreateAssetTexture(AssetType assetType, std::filesystem::path filePath, bool loadFile){
    switch(assetType){
        case AssetType::PPM :{
            std::optional<AssetTexture> tex;
            if (loadFile)
                tex = PPM::LoadPPM(filePath);
            else
                tex = AssetTexture::AssetTexture(assetType, filePath);
            return tex;
        }
        break;
        default:
            std::cout << "Texture type not supported:" << (int)assetType << std::endl;
    }
    return std::nullopt;
}

void AssetTexture::FlipImageVertically() {
    size_t rowSize = width * 3; // 3 is for RGB; use 4 for RGBA
    std::vector<unsigned char> tempRow(rowSize);
    for (int i = 0; i < height / 2; ++i) {
        std::copy(pixels.begin() + i * rowSize, pixels.begin() + (i + 1) * rowSize, tempRow.begin());
        std::copy(pixels.begin() + (height - i - 1) * rowSize, pixels.begin() + (height - i) * rowSize, pixels.begin() + i * rowSize);
        std::copy(tempRow.begin(), tempRow.end(), pixels.begin() + (height - i - 1) * rowSize);
    }
}
