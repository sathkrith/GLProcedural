#include "asset/stl.hpp"

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>

#include "utils.hpp"

using namespace std;
using namespace glm;

void streamToNormal(stringstream &stream, vector<vec3> &normal){
    glm::vec3 v = streamToVec3(stream);
    normal.push_back(v);
    normal.push_back(v);
    normal.push_back(v);
}

// Load the STL File
std::optional<STLFile> STLFile::LoadSTL(std::filesystem::path filePath){
    std::optional<STLFile> object;
    // File input stream for our file
    std::ifstream infile = OpenFile(filePath);
    if(!infile.is_open()){
        return object;
    }

    string line;
    string token;
    stringstream stream;
    vector<vec3> vertices, normals;
    vector<Triangle> mesh;
    while(getline(infile, line)){
        stream.str("");
        stream.clear();
        stream << line;
        while(stream >> token){
            if(token == "vertex"){
                vertices.push_back(streamToVec3(stream));
            }
            else if(token == "facet"){
                stream >> token;
                if(token == "normal")
                    streamToNormal(stream, normals);
            }
        }
    }

    // Compute the surface normal manually -- STL file is not trustworthy...
    // Iterate through 9 vertices at a time
    for(int i=0; i < vertices.size(); i+=3){
        Triangle t;
        t.vertices[0] = vertices[i];
        t.vertices[1] = vertices[i+1];
        t.vertices[2] = vertices[i+2];
        glm::vec3 edge1 = t.vertices[1] - t.vertices[0];
        glm::vec3 edge2 = t.vertices[2] - t.vertices[1];

        glm::vec3 normal = cross(edge1,edge2);
        std::cout << "mine:" << normal.x << "," << normal.y << "," << normal.z << std::endl;
        std::cout << "them:" <<  normals[i].x << "," << normals[i].y << "," <<  normals[i].z << std::endl;

        // Replace the normlals
        t.normals[i] = normal;
        t.normals[i+1] = normal;
        t.normals[i+2] = normal;
        mesh.push_back(t);
    }
    object = STLFile(filePath);
    object.value().mVertices = vertices;
    object.value().mNormals = normals;
    object.value().mMesh = mesh;
    object.value().mAssetState = AssetState::GOOD;
    return object;
}

vector<glm::vec3> STLFile::GetNormals() const{
    return this->mNormals;
}

vector<Triangle> STLFile::GetMesh() const{
    return this->mMesh;
}


vector<glm::vec3> STLFile::GetVertices() const{
    return this->mVertices;
}