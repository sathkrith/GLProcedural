#ifndef ASSET_MANAGER_HPP
#define ASSET_MANAGER_HPP

#include "asset/asset_object.hpp"
#include "asset/asset_material.hpp"
#include "asset/asset_texture.hpp"
#include <unordered_set>
#include <unordered_map>

/*
Manages assets in the program.
*/
struct AssetManager {
    private:
        // Map of asset id - 3d object.
        std::unordered_map<AssetID, std::shared_ptr<AssetObject>> mObjects;
        // Map of asset id - meterial.
        std::unordered_map<AssetID, AssetMaterial> mMaterials;
        // List of texture ID returned from GlGenTextures.
        std::vector<GLuint> mTextures;

        // Activated asset IDs of objects with texture.
        std::unordered_set<AssetID> mActivatedTexturedAssets;
        // Activated asset IDs of objects without texture .
        std::unordered_set<AssetID> mActivatedPlainAssets;

        // Plain and material asset ID vs asset information.
        std::unordered_map<AssetID, ObjectRenderInfo> mMaterialObjects;
        std::unordered_map<AssetID, ObjectRenderInfo> mPlainObjects;

        // IDs
        int mLastMaterialIndex = 0;
        // Material shader.
        GLuint mMaterialShaderID = 0;
        // Plain shader  - for objects with no texture.
        GLuint mPlainShaderID = 0;

        /*
        * Initialize all objects.
        */
        void InitializeObjects();
        /*
        * Initialize all material uniforms.
        */
        void InitializeMaterials();
        /*
        * Load a texture from image file and Initialize.
        */
        GLuint InitializeTextures(std::filesystem::path& filePath);
        /*
        * Initialize all light uniforms.
        */
        void InitializeLights(GLuint shaderId);

    public:

        //Initialize arrays, uniforms and buffers.
        void Initialize();
        AssetManager() = default;
        ~AssetManager();
        // Get object of asset id
        std::optional<std::shared_ptr<AssetObject>> getObject(AssetID id);
        // Get material of asset id
        std::optional<AssetMaterial> getMaterial(AssetID id);
        // Get texture id of texture index
        std::optional<GLuint> getTexture(size_t id);


        /**
        * Load an asset from the provided path.
        */
        unsigned short LoadAsset(std::filesystem::path filePath, AssetType type, bool isStatic);

        /*
        Generate a Brick Wall object.
        */
        unsigned short GenerateBrickWall();
};
#endif