
/** @file obj.hpp
 *  @brief A model for OBJ format files.
 *  
 *  Sets up an OpenGL OBJ model. 
 *
 *  @author Sathkrith
 *  @bug No known bugs.
 */
#ifndef OBJ_OBJECT2_HPP
#define OBJ_OBJECT2_HPP

#include "glm/glm.hpp"
#include "asset/asset_object.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>

// Represents an object in .obj format.
struct OBJFile2 :public AssetObject {
    OBJFile2():AssetObject(AssetType::OBJ, "") {};
    // Texture co ordinates.
    std::vector<glm::vec2> mTextureCords;
    // Object mesh.
    std::unordered_map<std::string, std::vector<Triangle>> mMeshList;
    // Object vertices.
    std::vector<glm::vec3> mVertices;
    // Obeject normals.
    std::vector<glm::vec3> mNormals;
    // Creeate an asset object class object
    // does not load the obj at the path provided.
    OBJFile2(std::filesystem::path path):AssetObject(AssetType::OBJ, path){};
    // return asset object after loading .obj file at path.
    static std::optional<OBJFile2> LoadOBJ(std::filesystem::path path);

};
#endif
