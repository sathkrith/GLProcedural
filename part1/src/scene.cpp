#include "scene.hpp"
#include "game_state.hpp"
#include <asset/ppm.hpp>

Scene::Scene(std::filesystem::path sceneFile) {}

void Scene::InitializeTexturedObjects() {
    /*
    // Vertex Arrays Object (VAO) Setup
    glGenVertexArrays(1, &mMaterialVertexArrayObject);
    // We bind (i.e. select) to the Vertex Array Object (VAO) that we want to work withn.
    glBindVertexArray(mMaterialVertexArrayObject);
    std::vector<GLfloat> mesh;
    int offset = 0;
    bool foundObject = false;
    for (const std::pair<AssetID, ObjectRenderInfo>& info : mMaterialObjects) {
        if (!info.second.hasTexture || !gGameState.gAssetManager->getObject(info.first).has_value())
            continue;
        //AssetObject *asset = &mAssets[assetInfo.assetID];
        std::vector<Triangle> tempMesh = gGameState.gAssetManager->getObject(info.first).value()->GetMesh();
        mMaterialObjects[info.first].meshSize = tempMesh.size() * 3;
        mMaterialObjects[info.first].vertexBufferOffset = offset;
        offset += mMaterialObjects[info.first].meshSize;
        for (Triangle t : tempMesh) {
            for (int i = 0; i < 3; i++) {
                mesh.push_back(t.vertices[i].x);
                mesh.push_back(t.vertices[i].y);
                mesh.push_back(t.vertices[i].z);
                mesh.push_back(t.normals[i].x);
                mesh.push_back(t.normals[i].y);
                mesh.push_back(t.normals[i].z);
                mesh.push_back(t.tex[i].x);
                mesh.push_back(t.tex[i].y);
                mesh.push_back(t.tangent.x);
                mesh.push_back(t.tangent.y);
                mesh.push_back(t.tangent.z);
                mesh.push_back(t.biTangent.x);
                mesh.push_back(t.biTangent.y);
                mesh.push_back(t.biTangent.z);
            }
        }
    }

    // Handle materials
    // Vertex Buffer Object (VBO) creation
    glGenBuffers(1, &mMaterialVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mMaterialVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(GLfloat), mesh.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (GLvoid*)0);
    // Color information (r,g,b)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (GLvoid*)(sizeof(GLfloat) * 3));
    // Normal information (nx,ny,nz)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (GLvoid*)(sizeof(GLfloat) * 6));
    // Tangents
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (GLvoid*)(sizeof(GLfloat) * 8));
    // BiTangents
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (GLvoid*)(sizeof(GLfloat) * 11));

    glBindVertexArray(0);
    */
}

void Scene::InitializePlainObjects() {
    /*
    // Vertex Arrays Object (VAO) Setup
    glGenVertexArrays(1, &mPlainVertexArrayObject);
    // We bind (i.e. select) to the Vertex Array Object (VAO) that we want to work withn.
    glBindVertexArray(mPlainVertexArrayObject);
    std::vector<GLfloat> plainMesh;
    int offset = 0;
    bool foundObject = false;
    for (const std::pair<AssetID, ObjectRenderInfo>& info : mPlainObjects) {
        if (info.second.hasTexture || !gGameState.gAssetManager->getObject(info.first).has_value())
            continue;
        //AssetObject *asset = &mAssets[assetInfo.assetID];
        std::vector<Triangle> tempMesh = gGameState.gAssetManager->getObject(info.first).value()->GetMesh();
        mPlainObjects[info.first].meshSize = tempMesh.size() * 3;
        mPlainObjects[info.first].vertexBufferOffset = offset;
        offset += mPlainObjects[info.first].meshSize;
        for (Triangle t : tempMesh) {
            for (int i = 0; i < 3; i++) {
                plainMesh.push_back(t.vertices[i].x);
                plainMesh.push_back(t.vertices[i].y);
                plainMesh.push_back(t.vertices[i].z);
                plainMesh.push_back(t.normals[i].x);
                plainMesh.push_back(t.normals[i].y);
                plainMesh.push_back(t.normals[i].z);
            }
        }
    }

    glGenBuffers(1, &mPlainVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mPlainVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, plainMesh.size() * sizeof(GLfloat), plainMesh.data(), GL_STATIC_DRAW);


    // vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)0);
    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)(sizeof(GL_FLOAT) * 3));

    glBindVertexArray(0);

    */
}

void Scene::Initialize()
{
    this->viewer.Initialize(300, 300,100, 0,1, 0.5);
    sky.initialize(10, 5);
}

void Scene::DrawPlainObjects() {
    /*
    if (mActivatedPlainAssets.empty())
        return;
    glUseProgram(gGameState.gPlainObjectShader);
    glBindVertexArray(mPlainVertexArrayObject);
    updateEnvironment(gGameState.gPlainObjectShader);
    for (AssetID ID : mActivatedPlainAssets) {
        ObjectRenderInfo info = mPlainObjects[ID];
        GLuint colorLocation = glGetUniformLocation(gGameState.gPlainObjectShader, "u_color");
        glUniform3f(colorLocation, info.color.r, info.color.g, info.color.b);
        glDrawArrays(GL_TRIANGLES, info.vertexBufferOffset, info.meshSize);
    }
    glBindVertexArray(0);
    glUseProgram(0);
    */
}


void Scene::DrawMaterialObjects() {
    /*
    if (mActivatedTexturedAssets.empty())
        return;
    glUseProgram(gGameState.gTexturedObjectShader);
    glBindVertexArray(mMaterialVertexArrayObject);

    // Update light and camera
    updateEnvironment(gGameState.gTexturedObjectShader);

    for (AssetID ID : mActivatedTexturedAssets) {
        ObjectRenderInfo info = mMaterialObjects[ID];
        AssetMaterial& material = mMaterialObjects[info.materialAssetID];
        if (!material.materialShaderIndex.has_value()) {
            std::cout << "Material has not been initialized for:" << ID << "\n";
            continue;
        }
        // Update color
        GLuint colorLocation = glGetUniformLocation(gGameState.gTexturedObjectShader, "u_color");
        glUniform3f(colorLocation, info.color.r, info.color.g, info.color.b);

        GLuint materialIDLocation = glGetUniformLocation(gGameState.gTexturedObjectShader, "u_materialID");
        glUniform1i(materialIDLocation, 0);

        // bind diffuse map
        if (material.diffuseMapIndex.has_value()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mTextures[material.diffuseMapIndex.value()]);
        }

        // bind specular map
        if (material.specularMapIndex.has_value()) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mTextures[material.specularMapIndex.value()]);
        }
        // bind normal map
        if (material.normalMapIndex.has_value()) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, mTextures[material.normalMapIndex.value()]);
        }
        glDrawArrays(GL_TRIANGLES, info.vertexBufferOffset, info.meshSize);
    }
    glBindVertexArray(0);
    glUseProgram(0);
    */
}

void Scene::PreDraw() {
    // Disable depth test and face culling.
    glEnable(GL_DEPTH_TEST);                    // NOTE: Need to enable DEPTH Test
    //glDisable(GL_CULL_FACE);

    // Set the polygon fill mode
    glPolygonMode(GL_FRONT_AND_BACK, gGameState.gPolygonMode);

    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, gGameState.gScreenWidth, gGameState.gScreenHeight);
    glClearColor(0.1f, 4.f, 7.f, 1.f);

    //Clear color buffer and Depth Buffer
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

}

void Scene::Draw() {
    // Enable our attributes
    //Render data
    //DrawPlainObjects();
    //DrawMaterialObjects();
    //sky.draw();
    this->viewer.Draw();
}

