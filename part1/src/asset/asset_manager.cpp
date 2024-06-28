#include "asset/asset_manager.hpp"
#include "asset/obj.hpp"
#include "asset/stl.hpp"
#include "asset/ppm.hpp"
#include "asset/asset_material.hpp"
#include "asset/asset_texture.hpp"
#include <glm/gtc/matrix_transform.hpp> 
#include "game_state.hpp"
#include "constants.hpp"
#include <glad/glad.h>



// Load texture to opengl and return corresponding texture ID.
unsigned int loadTexture(AssetTexture& texture) {
    unsigned int textureID;
    int width = 0, height = 0, nrComponents = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, texture.format, texture.width, texture.height, 0, texture.format, GL_UNSIGNED_BYTE, &texture.pixels[0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLint error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error setting texture:" << error << "\n";
    }
    return textureID;
}

AssetID AssetManager::LoadAsset(std::filesystem::path filePath, AssetType type, bool isStatic){
    bool isObjFollowedByMtl = false;
    AssetID objectId = -1;
    switch (type)
    {
    case AssetType::OBJ :
    case AssetType::STL :
        {
            ObjectRenderInfo info;
            std::optional<std::shared_ptr<AssetObject>> asset = AssetObject::CreateAssetObject(type, filePath);
            if(!asset.has_value())
                return false;
            info.assetID = asset.value()->GetAssetID();
            objectId = info.assetID;
            mObjects[objectId] = asset.value();
            if(asset.value()->isTextureIncluded)
                mMaterialObjects[objectId] = info;
            else {
                mPlainObjects[objectId] = info;
                mPlainObjects[objectId].type = ColorType::color;
                mPlainObjects[objectId].color = gGameState.defaultColor;
                mPlainObjects[objectId].hasTexture = false;
                break;
            }
            // try to find mtl file in the same directory
            std::filesystem::path mtlPath;
            if (asset.value()->mtlPath.has_value()) {
                mtlPath = filePath.parent_path().append(asset.value()->mtlPath.value());
                if (!std::filesystem::exists(mtlPath)) {
                    mMaterialObjects[asset.value()->GetAssetID()].type = ColorType::color;
                    mMaterialObjects[asset.value()->GetAssetID()].color = gGameState.defaultColor;
                    mMaterialObjects[objectId].hasTexture = false;
                    break;
                }
            }
            else {
                mMaterialObjects[asset.value()->GetAssetID()].type = ColorType::color;
                mMaterialObjects[asset.value()->GetAssetID()].color = gGameState.defaultColor;
                mMaterialObjects[objectId].hasTexture = false;
                break;
            }
            mMaterialObjects[info.assetID].type = ColorType::material;
            isObjFollowedByMtl = true;
            std::cout << "mtl file exists for the object, attempting to load " << mtlPath << std::endl;
            filePath = mtlPath;
            type = AssetType::MTL;
        }
    case AssetType::MTL : 
        {
        std::unordered_map<std::string, AssetMaterial> assets = std::move(AssetMaterial::CreateAssetMaterials(type, filePath));
            if(assets.empty())
                return false;
            for (auto& [key, asset] : assets) {
				mMaterials.emplace(asset.GetAssetID(), asset);
                mMaterials.emplace(asset.GetAssetID(), std::move(asset));
                // std::pair<USHORT, AssetMaterial> pair(asset->mAssetID, asset.value())
                if (isObjFollowedByMtl) {
                    mMaterialObjects[objectId].materialAssetID = asset.GetAssetID();
                    mMaterialObjects[objectId].type = ColorType::material;
                    mMaterialObjects[objectId].hasTexture = true;
                }
			}

        }
        break;
    case AssetType::PPM : 
        {
            std::optional<AssetTexture> asset = AssetTexture::CreateAssetTexture(type, filePath);
            if(!asset.has_value())
                return false;
            mTextures[asset.value().GetAssetID()] = loadTexture(asset.value());
        }
        break;
    default:
        std::cout << "Invalid type:" << (int)type << std::endl;
        break;
    }
    return objectId;
}


GLuint AssetManager::InitializeTextures(std::filesystem::path& filePath)
{
    bool isLoadSuccessfull = false;
    unsigned int textureID = PPM::loadTextureToRender(filePath, isLoadSuccessfull);
    if (isLoadSuccessfull) {
        this->mTextures.push_back(textureID);
        return this->mTextures.size() - 1;
    }
    return -1;
}

void AssetManager::InitializeLights(GLuint shaderId){
    GLuint lightPositionLocation = glGetUniformLocation(shaderId, "u_lightPosition");
    GLuint lightAmbientLocation = glGetUniformLocation(shaderId, "u_light.ambientColor");
    GLuint lightDiffuseLocation = glGetUniformLocation(shaderId, "u_light.diffuseColor");
    GLuint lightSpecularLocation = glGetUniformLocation(shaderId, "u_light.specularColor");
    GLuint constant = glGetUniformLocation(shaderId, "u_light.attenConstant");
    GLuint linear = glGetUniformLocation(shaderId, "u_light.attenLinear");
    GLuint quadratic = glGetUniformLocation(shaderId, "u_light.attenQuadratic");
    glUniform1f(constant, gGameState.gLight->constant);
    glUniform1f(linear, gGameState.gLight->linear);
    glUniform1f(quadratic, gGameState.gLight->quadratic);
    glUniform3fv(lightPositionLocation, 1, &gGameState.gLight->position[0]);
    glUniform3f(lightAmbientLocation, gGameState.gLight->ambientColor.r, gGameState.gLight->ambientColor.g, gGameState.gLight->ambientColor.b);
    glUniform3f(lightDiffuseLocation, gGameState.gLight->diffuseColor.r, gGameState.gLight->diffuseColor.g, gGameState.gLight->diffuseColor.b);
    glUniform3f(lightSpecularLocation, gGameState.gLight->specularColor.r, gGameState.gLight->specularColor.g, gGameState.gLight->specularColor.b);

}

void AssetManager::Initialize(){
    std::cout << "Initializing AssetManager" << std::endl;
    std::string vertexShaderSource      = LoadShaderAsString("./shaders/vert.glsl");
    std::string fragmentShaderSource    = LoadShaderAsString("./shaders/frag.glsl");
    std::string plainVertexShaderSource = LoadShaderAsString("./shaders/plain_vert.glsl");
    std::string plainFragmentShaderSource = LoadShaderAsString("./shaders/plain_frag.glsl");

    mMaterialShaderID = CreateShaderProgram(vertexShaderSource,fragmentShaderSource);
    mPlainShaderID = CreateShaderProgram(plainVertexShaderSource, plainFragmentShaderSource);

    // Model transformation by translating our object into world space
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    // Projection matrix (in perspective) 
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
        (float)gGameState.gScreenWidth / (float)gGameState.gScreenHeight,
        0.1f,
        20.0f);

    // Initialize objects 
    std::cout << "Initializing objects" << std::endl;
    //InitializeObjects();

    // Initialize uniforms in material shader
    glUseProgram(mMaterialShaderID);
    // Retrieve our location of our Model Matrix
    GLint u_ModelMatrixLocation = glGetUniformLocation(mMaterialShaderID, "u_ModelMatrix");
    glUniformMatrix4fv(u_ModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
 

    // Retrieve our location of our perspective matrix uniform 
    GLint u_ProjectionLocation = glGetUniformLocation(mMaterialShaderID, "u_Projection");
    glUniformMatrix4fv(u_ProjectionLocation, 1, GL_FALSE, &perspective[0][0]);

    std::cout << "Initializing lights" << std::endl;
    //InitializeLights(mMaterialShaderID);
    std::cout << "Initializing materials" << std::endl;
    //InitializeMaterials();
    //std::cout << "Initializing textures" << std::endl;
    //for (auto &pair : this->mTextures) {
    //    this->InitializeTextures(pair.second);
    //}
    /*GLuint diffuseMapLoc = glGetUniformLocation(mMaterialShaderID, "u_diffuseMap");
    GLuint specMapLoc = glGetUniformLocation(mMaterialShaderID, "u_specularMap");
    GLuint normalMapLoc = glGetUniformLocation(mMaterialShaderID, "u_normalMap");
    glUniform1i(diffuseMapLoc, 0);
    glUniform1i(specMapLoc, 1);
    glUniform1i(normalMapLoc, 2);
    */
    glUseProgram(0);

    // Initialize shaders for non material program
    glUseProgram(mPlainShaderID);
    // Retrieve our location of our Model Matrix
    u_ModelMatrixLocation = glGetUniformLocation(mPlainShaderID, "u_ModelMatrix");
    glUniformMatrix4fv(u_ModelMatrixLocation, 1, GL_FALSE, &model[0][0]);


    // Retrieve our location of our perspective matrix uniform 
    u_ProjectionLocation = glGetUniformLocation(mPlainShaderID, "u_Projection");
    glUniformMatrix4fv(u_ProjectionLocation, 1, GL_FALSE, &perspective[0][0]);

    InitializeLights(mPlainShaderID);

    glUseProgram(0);
}

AssetManager::~AssetManager(){

    for (const GLuint &textureIds : mTextures) {
        glDeleteTextures(1, &textureIds);
    }
    // Delete our Graphics pipeline
    glDeleteProgram(mMaterialShaderID);
}


AssetID AssetManager::GenerateBrickWall()
{
    OBJFile obj = std::move(OBJFile::CreateBrick());
    AssetMaterial mtl = std::move(AssetMaterial::createBrickMTL());
    ObjectRenderInfo info;
    info.assetID = obj.GetAssetID();
    info.hasTexture = true;
    info.materialAssetID = mtl.GetAssetID();
    this->mMaterialObjects.emplace(obj.GetAssetID(),info);
    this->mMaterials.emplace(mtl.GetAssetID(), mtl);
    this->mObjects.emplace(obj.GetAssetID(), std::make_shared<OBJFile>(obj));
    return obj.GetAssetID();
}
