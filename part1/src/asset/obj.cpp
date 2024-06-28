#include "asset/obj.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <utils.hpp>
#include <glm/mat2x2.hpp>

using namespace std;
using namespace glm;


void computeTBN(Triangle &t) {
    vec2 deltaUV1 = t.tex[1] - t.tex[0];
    vec2 deltaUV2 = t.tex[2] - t.tex[1];
    vec3 edge1 = t.vertices[1] - t.vertices[0];
    vec3 edge2 = t.vertices[2] - t.vertices[1];
    // E1 = deltaU1.T + deltaV1.B
    // E2 = deltaU2.T + deltaV2.B
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    t.tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    t.tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    t.tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    t.tangent = normalize(t.tangent);
    t.biTangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    t.biTangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    t.biTangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    t.biTangent = normalize(t.biTangent);
}

void streamToFace(stringstream &stream, vector<vec3> &vertices, vector<vec3> &normals,vector<vec2> &tex,  vector<Triangle> &mesh){
    Triangle t;
    string token;
    int index = 0;
    int vertexIndex = 0;
    bool isTextureIncluded = false;
    while(stream >> token){
        //set vertex
        std::stringstream parser(token);
        std::string subToken;
        std::getline(parser, subToken, '/');
        index = stoi(subToken);
        t.vertices[vertexIndex] = vertices[index - 1];
        //set texture
        std::getline(parser, subToken, '/');
        if (!subToken.empty()) {
            index = stoi(subToken);
            t.tex[vertexIndex] = tex[index - 1];
            isTextureIncluded = true;
        }
        // set normal
        std::getline(parser, subToken, '/');
        index = stoi(subToken);
        t.normals[vertexIndex++] = normals[index - 1];       
    }
    if(isTextureIncluded)
        computeTBN(t);
    mesh.push_back(t);
}

std::optional<OBJFile> OBJFile::LoadOBJ(std::filesystem::path filePath){
    std::optional<OBJFile> object;
    std::ifstream infile = OpenFile(filePath);
    if(!infile.is_open()){
        return object;
    }
    vector<vec3> vertices, normals;
    vector<vec2> tex;
    vector<Triangle> mesh;
    string line;
    string token;
    string mtlLib="";
    stringstream stream;
    bool isTextureIncluded = false;
    try {
        while (getline(infile, line)) {
            stream.str("");
            stream.clear();
            stream << line;
            while (stream >> token) {
                if (token == "v") {
                    vertices.push_back(streamToVec3(stream));
                }
                else if (token == "vn") {
                    normals.push_back(streamToVec3(stream));
                }
                else if (token == "vt") {
                    tex.push_back(streamToVec2(stream));
                    isTextureIncluded = true;
                }
                else if (token == "f") {
                    streamToFace(stream, vertices, normals, tex, mesh);
                }
                else if (token == "mtllib") {
                    stream >> mtlLib;
                }
            }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Failed to parse obj file" << ex.what() << "\n";
    }
    std::cout << "Number of triangles in the mesh:" << mesh.size()<<"\n";
    std::cout <<"Successfully loaded the OBJFile file\n";
    infile.close();
    object = OBJFile(filePath);
    object.value().mVertices = vertices;
    object.value().mNormals = normals;
    object.value().mMesh = mesh;
    object.value().mTextureCords = tex;
    object.value().mAssetState = AssetState::GOOD;
    object.value().mtlPath = mtlLib;
    object.value().isTextureIncluded = isTextureIncluded;
    return object;
}

OBJFile OBJFile::CreateBrick()
{
    OBJFile obj("");

    obj.mVertices = { vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f) , vec3(1.0f, 1.0f, 0.0f) , vec3(-1.0f, 1.0f, 0.0f) };
    obj.mNormals = vector<vec3>(4, vec3(0.0f, 0.0f, 1.0f));
    obj.mTextureCords = { vec2(0.0f, 0.0f),vec2(1.0f, 0.0f), vec2(1.0f, 1.0f) , vec2(0.0f, 1.0f) };

    Triangle t;
    t.vertices = { obj.mVertices[0],obj.mVertices[1], obj.mVertices[2] };
    t.normals = { obj.mNormals[0],obj.mNormals[1], obj.mNormals[2] };
    t.tex = { obj.mTextureCords[0],obj.mTextureCords[1], obj.mTextureCords[2] };
    computeTBN(t);
    obj.mMesh.push_back(t);

    Triangle t2;
    t2.vertices = { obj.mVertices[2],obj.mVertices[3], obj.mVertices[0] };
    t2.normals = { obj.mNormals[2],obj.mNormals[3], obj.mNormals[0] };
    t2.tex = { obj.mTextureCords[2],obj.mTextureCords[3], obj.mTextureCords[0] };
    computeTBN(t2);
    obj.mMesh.push_back(t2);
    obj.isTextureIncluded = true;
    return obj;
}

OBJFile OBJFile::CreateFloor(float height, float width)
{
    OBJFile obj("");

    obj.mVertices = { vec3(-width/2, 0.0f, -height/2), vec3(width / 2, 0.0f, -height / 2) , vec3(width / 2, 0.0f, height / 2) , vec3(-width / 2, 0.0f, height / 2) };
    obj.mNormals = vector<vec3>(4, vec3(0.0f, 1.0f, 0.0f));
    obj.mTextureCords = { vec2(0.0f, 0.0f),vec2(1.0f, 0.0f), vec2(1.0f, 1.0f) , vec2(0.0f, 1.0f) };

    Triangle t;
    t.vertices = { obj.mVertices[0],obj.mVertices[1], obj.mVertices[2] };
    t.normals = { obj.mNormals[0],obj.mNormals[1], obj.mNormals[2] };
    t.tex = { obj.mTextureCords[0],obj.mTextureCords[1], obj.mTextureCords[2] };
    computeTBN(t);
    obj.mMesh.push_back(t);

    Triangle t2;
    t2.vertices = { obj.mVertices[2],obj.mVertices[3], obj.mVertices[0] };
    t2.normals = { obj.mNormals[2],obj.mNormals[3], obj.mNormals[0] };
    t2.tex = { obj.mTextureCords[2],obj.mTextureCords[3], obj.mTextureCords[0] };
    computeTBN(t2);
    obj.mMesh.push_back(t2);
    obj.isTextureIncluded = true;
    return obj;
}

std::vector<Triangle> OBJFile::GetMesh() const {
    return mMesh;
}
