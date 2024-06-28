#ifndef STL_OBJECT_HPP
#define STL_OBJECT_HPP

#include <vector>
#include <sstream>
#include <string>
#include "asset/asset_object.hpp"

// The STL format is a simple 'triangle soup' format that outputs
// one normal per triangle, and then the following three triangles
// The .stl bunny was obtained from: https://en.m.wikipedia.org/wiki/File:Stanford_Bunny.stl
// And then modified in blender3D.
struct STLFile: public AssetObject{
	// Colors in the object
	std::vector<glm::vec2> mColors;
	// Mesh of the object.
	std::vector<Triangle> mMesh;
	// Object vertices
	std::vector<glm::vec3> mVertices;
	// Obejct normals.
	std::vector<glm::vec3> mNormals;
	// Create STLFile object.
	// Does not load the file at filepath.
	STLFile(std::filesystem::path filePath):AssetObject(AssetType::STL, filePath){};
	// Load stl file at path and return corresponding stl object.
	static std::optional<STLFile> LoadSTL(std::filesystem::path path);
	// Return vertices.
	std::vector<glm::vec3> GetVertices() const;
	// return Normals.
	std::vector<glm::vec3> GetNormals() const;
	// Return mesh.
	std::vector<Triangle> GetMesh() const;
};

#endif