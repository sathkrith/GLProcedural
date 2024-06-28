
/** @file obj.hpp
 *  @brief A model for OBJ format files.
 *  
 *  Sets up an OpenGL OBJ model. 
 *
 *  @author Sathkrith
 *  @bug No known bugs.
 */
#ifndef OBJ_OBJECT_HPP
#define OBJ_OBJECT_HPP

#include "glm/glm.hpp"
#include "asset/asset_object.hpp"
#include <vector>
#include <string>
#include <sstream>

// Represents an object in .obj format.
struct OBJFile :public AssetObject {
    OBJFile():AssetObject(AssetType::OBJ, "") {};
    // Texture co ordinates.
    std::vector<glm::vec2> mTextureCords;
    // Object mesh.
    std::vector<Triangle> mMesh;
    // Object vertices.
    std::vector<glm::vec3> mVertices;
    // Obeject normals.
    std::vector<glm::vec3> mNormals;
    // Creeate an asset object class object
    // does not load the obj at the path provided.
    OBJFile(std::filesystem::path path):AssetObject(AssetType::OBJ, path){};
    // return the mesh of obj object.
    virtual std::vector<Triangle> GetMesh() const;
    // return asset object after loading .obj file at path.
    static std::optional<OBJFile> LoadOBJ(std::filesystem::path path);
    // create an objfile object representing a brick.
    static OBJFile CreateBrick();
    static OBJFile CreateFloor(float width, float height);

};
#endif
