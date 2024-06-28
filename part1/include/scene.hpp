#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "asset/asset_manager.hpp"
#include "procedural/terrain_renderer.hpp"
#include <procedural/skydome.hpp>

// Set of static objects drawn on the screen.
struct Scene {
private:
    // List of texture indices in asset manager.
    std::vector<AssetID> mTextures;
    // translation, rotation and scaling info
    std::unordered_map<AssetID, glm::mat4> mObjectTransformations;
    // Activated asset IDs of objects with texture.
    std::unordered_set<AssetID> mActivatedTexturedAssets;
    // Activated asset IDs of objects without texture .
    std::unordered_set<AssetID> mActivatedPlainAssets;
    std::filesystem::path mFilePath;

    // Plain and material asset ID vs asset information.
    std::unordered_map<AssetID, ObjectRenderInfo> mMaterialObjects;
    std::unordered_map<AssetID, ObjectRenderInfo> mPlainObjects;


    // Material objects vertex array.
    GLuint mMaterialVertexArrayObject = 0;
    // material objects buffer.
    GLuint mMaterialVertexBufferObject = 0;
    // solid color objects vertex array.
    GLuint mPlainVertexArrayObject = 0;
    // solid color objects buffer.
    GLuint mPlainVertexBufferObject = 0;

    skyDome sky;
    TerrainRenderer viewer;
    /*
    * Initialize all objects with texture co ordinates.
    */
    void InitializeTexturedObjects();
    /*
    * Initialize all objects without texture co ordinates.
    */
    void InitializePlainObjects();
    /*
    * Initialize all light uniforms.
    */
    void InitializeLights(GLuint shaderId);


    /*
    * Set light based uniforms before drawing objects.
    */
    void updateEnvironment(GLuint shaderId);
    /**
    * Draw all objects that does not have texture co-ordinates defined.
    */
    void DrawPlainObjects();
    /**
    * Draw all objects that have texture co-ordinates defined.
    */
    void DrawMaterialObjects();

    void DrawSkyDome();

public:
    void Initialize();

    /**
    * Steps to take before drawing objects.
    */
    void PreDraw();
    /*
    * Draw all activated assets on the screen.
    */
    void Draw();
    Scene(std::filesystem::path sceneFile);
};