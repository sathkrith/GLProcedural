#include "asset/asset_material.hpp"
#include "asset/ppm.hpp"
#include <sstream>
#include <unordered_map>

using namespace std;

void fillVec3(stringstream &stream, glm::vec3 &vec){
    stream >> vec.x;
    stream >> vec.y;
    stream >> vec.z;
}

std::unordered_map<string, AssetMaterial> AssetMaterial::CreateAssetMaterials(AssetType type, std::filesystem::path filePath){
    std::unordered_map<string, AssetMaterial> materials;
    if (type != AssetType::MTL)
        return materials;
    std::ifstream mtlFile = OpenFile(filePath);
    if (!mtlFile.is_open()) {
        return materials;
    }
    glm::vec3 ambient = glm::vec3(0.5, 0.5,0.5);
    glm::vec3 diffuseColor = glm::vec3(0.5, 0.5, 0.5);
    glm::vec3 specular = glm::vec3(0.5, 0.5, 0.5);
    float specHighlight = 0;
    float transparency = 0;
    std::filesystem::path diffTexture;
    std::filesystem::path specTexture;
    std::filesystem::path normapMap;
    string line;
    string token;
    string name;
    stringstream stream;
    while (getline(mtlFile, line)) {
        stream.str("");
        stream.clear();
        stream << line;
        stream >> token;

        if (token == "Ka") {
            fillVec3(stream, ambient);
        }
        if (token == "Ks") {
            fillVec3(stream, specular);
        }
        if (token == "Kd") {
            fillVec3(stream, diffuseColor);
        }
        if (token == "Ns") {
            stream >> token;
            specHighlight = stof(token) / 1000;
        }
        if (token == "Ni") {
            stream >> token;
            transparency = stof(token);
        }
        if (token == "map_Kd") {
            stream >> token;
            filesystem::path path = filePath.parent_path();
            path.append(token);
            diffTexture = path;
        }
        if (token == "map_Ks") {
            stream >> token;
            filesystem::path path = filePath.parent_path();
            path.append(token);
            specTexture = path;
        }
        if (token == "map_Bump") {
            stream >> token;
            filesystem::path path = filePath.parent_path();
            path.append(token);
            normapMap = path;
        }
        if (token == "newmtl") {
            if (!name.empty()) {
                materials.emplace(name, AssetMaterial(type, filePath));
                materials[name].ambientColor = ambient;
                materials[name].diffuseColor = diffuseColor;
                materials[name].specularColor = specular;
                materials[name].diffuseMapPath = !diffTexture.empty() ? std::optional<std::filesystem::path>(diffTexture) : nullopt;
                materials[name].specularMapPath = !specTexture.empty() ? std::optional<std::filesystem::path>(specTexture) : nullopt;
                materials[name].normalMapPath = !normapMap.empty() ? std::optional<std::filesystem::path>(normapMap) : nullopt;
                materials[name].shininess = specHighlight;
                materials[name].transparency = transparency;
                materials[name].mAssetState = AssetState::GOOD;
                ambient = glm::vec3(0.5, 0.5, 0.5);
                diffuseColor = glm::vec3(0.5, 0.5, 0.5);
                specular = glm::vec3(0.5, 0.5, 0.5);
                specHighlight = 0;
                transparency = 0;
                diffTexture = "";
                specTexture = "";
                normapMap = "";
            }
            stream >> name;
        }
    }
    if (!name.empty()) {
        materials.emplace(name, AssetMaterial(type, filePath));
        materials[name].ambientColor = ambient;
        materials[name].diffuseColor = diffuseColor;
        materials[name].specularColor = specular;
        materials[name].diffuseMapPath = !diffTexture.empty() ? std::optional<std::filesystem::path>(diffTexture) : nullopt;
        materials[name].specularMapPath = !specTexture.empty() ? std::optional<std::filesystem::path>(specTexture) : nullopt;
        materials[name].normalMapPath = !normapMap.empty() ? std::optional<std::filesystem::path>(normapMap) : nullopt;
        materials[name].shininess = specHighlight;
        materials[name].transparency = transparency;
        materials[name].mAssetState = AssetState::GOOD;
    }
    return materials;
}

AssetMaterial AssetMaterial::createBrickMTL()
{
    AssetMaterial material(AssetType::MTL, "./");
    material.diffuseMapPath = std::filesystem::path("./media/brick.ppm");
    material.normalMapPath = std::filesystem::path("./media/normal.ppm");
    material.specularMapPath = std::nullopt;
    material.shininess = 0.1f;
    material.ambientColor = glm::vec3(0.01f, 0.01f, 0.01f);
    material.transparency = 0.0f;
    return material;
}
