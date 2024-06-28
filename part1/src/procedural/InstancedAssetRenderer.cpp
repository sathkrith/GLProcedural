#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "game_state.hpp"
#include <filesystem>
#include "asset/obj.hpp"
#include "asset/ppm.hpp"
#include <asset/asset_material.hpp>


#include "procedural/fault_formation.hpp"
#include <glm/gtc/matrix_transform.hpp> 
#include <asset/obj2.hpp>


void checkOpenGLError() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
        switch (err) {
        case GL_INVALID_ENUM:
            std::cerr << "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.\n";
            break;
        case GL_INVALID_VALUE:
            std::cerr << "GL_INVALID_VALUE: A numeric argument is out of range.\n";
            break;
        case GL_INVALID_OPERATION:
            std::cerr << "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.\n";
            break;
        case GL_STACK_OVERFLOW:
            std::cerr << "GL_STACK_OVERFLOW: This command would cause a stack overflow.\n";
            break;
        case GL_STACK_UNDERFLOW:
            std::cerr << "GL_STACK_UNDERFLOW: This command would cause a stack underflow.\n";
            break;
        case GL_OUT_OF_MEMORY:
            std::cerr << "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.\n";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.\n";
            break;
        default:
            std::cerr << "Unknown error.\n";
            break;
        }
    }
}


bool InstancedAssetRenderer::LoadObj(const std::filesystem::path& modelPath) {
        bool isSucc = false;
        std::optional<OBJFile2> obj = OBJFile2::LoadOBJ(modelPath);
        if (!obj.has_value()) {
            return false;
        }
        if(obj.value().mtlPath.has_value()) {
            std::filesystem::path mtlPath = modelPath.parent_path() / obj.value().mtlPath.value();
            materials = AssetMaterial::CreateAssetMaterials(AssetType::MTL, mtlPath);
            for (auto const& [key, material] : materials) {
                this->textures[key] = 1000;
                if (material.diffuseMapPath.has_value()) {
                    this->textures[key] = PPM::loadTextureToRender(material.diffuseMapPath.value(), isSucc);
                    if (!isSucc) {
                        std::cout << "Failed to load texture" << std::endl;
                        this->textures[key] = 1000;
                    }
                }
			}
		}
        for (auto const& [key, value] : obj.value().mMeshList) {
            vaos[key] = 0;
            // Vertex Arrays Object (VAO) Setup
            glGenVertexArrays(1, &vaos[key]);
            // We bind (i.e. select) to the Vertex Array Object (VAO) that we want to work withn.
            glBindVertexArray(vaos[key]);
            std::vector<GLfloat> mesh;
            int offset = 0;
            bool foundObject = false;

            //AssetObject *asset = &mAssets[assetInfo.assetID];
            std::vector<Triangle> tempMesh = value;
            meshSize[key] = tempMesh.size() * 3;
            for (Triangle t : tempMesh) {
                for (int i = 0; i < 3; i++) {
                    mesh.push_back(t.vertices[i].x);
                    mesh.push_back(t.vertices[i].y);
                    mesh.push_back(t.vertices[i].z);
                    mesh.push_back(t.tex[i].x);
                    mesh.push_back(t.tex[i].y);
                    if (t.vertices[i].x < minx) {
                        minx = t.vertices[i].x;
                    }
                    else if (t.vertices[i].x > maxx) {
                        maxx = t.vertices[i].x;
                    }
                    if (t.vertices[i].y < miny) {
                        miny = t.vertices[i].y;
                    }
                    else if (t.vertices[i].y > maxy) {
                        maxy = t.vertices[i].y;
                    }
                    if (t.vertices[i].z < minz) {
                        minz = t.vertices[i].z;
                    }
                    else if (t.vertices[i].z > maxz) {
                        maxz = t.vertices[i].z;
                    }
                }
            }
            width = maxx - minx;
            height = maxy - miny;
            depth = maxz - minz;

            // Handle materials
            // Vertex Buffer Object (VBO) creation
            GLuint buffer;
            glGenBuffers(1, &buffer);
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(GLfloat), mesh.data(), GL_STATIC_DRAW);

            //vertices
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)0);
            // Tex cords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)(sizeof(GLfloat) * 3));

            glBindVertexArray(0);
        }
        return true;
}

void InstancedAssetRenderer::setup(const std::filesystem::path& modelPath) {
		bool isSuccessful = false;
        isSuccessful = LoadObj(modelPath);

        if (!isSuccessful) {
            std::cout << "Failed to load object" << std::endl;
        }

}

void InstancedAssetRenderer::updateInstances() {
    if (instancePositions.size() == 0) {
		return;
	}
    std::vector<GLfloat> instancePositionsUpdated;
    for(int i = 0; i < instancePositions.size(); i++) {
        //float pushUp = instancePositions[i].y - miny;
		instancePositionsUpdated.push_back(instancePositions[i].x);
		instancePositionsUpdated.push_back(instancePositions[i].y);
		instancePositionsUpdated.push_back(instancePositions[i].z);
	}

    std::vector<float> instanceRotations(instancePositions.size());
    for (auto& angle : instanceRotations) {
        angle = ((float)rand() / (float)RAND_MAX) * 360.0f; // Random rotation angle in degrees, ranging from 0 to 360
    }

    for (auto const& [key, modelVAO] : this->vaos) {
        glBindVertexArray(modelVAO);
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instancePositionsUpdated.size() * sizeof(GLfloat), &instancePositionsUpdated[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(2, 1);  // Tell OpenGL this is an instanced vertex attribute.

        GLuint rotationVBO;
        glGenBuffers(1, &rotationVBO);
        glBindBuffer(GL_ARRAY_BUFFER, rotationVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceRotations.size() * sizeof(float), instanceRotations.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, rotationVBO);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
        glVertexAttribDivisor(3, 1); 

        glBindVertexArray(0);
    }
}

void InstancedAssetRenderer::updateShader(GLuint shaderID, std::string key){
    GLuint materialColorLocation = glGetUniformLocation(shaderID, "u_material.diffuseColor");
    GLuint materialHasDiffLocation = glGetUniformLocation(shaderID, "u_material.bDiffMap");
    glUniform3f(materialColorLocation, materials[key].diffuseColor.r, materials[key].diffuseColor.g, materials[key].diffuseColor.b);
    if (textures[key] != 0 && textures[key] < 100) {
        glUniform1i(materialHasDiffLocation, true);
        GLuint textureLoc = glGetUniformLocation(shaderID, "u_texture");
		glUniform1i(textureLoc, 0);
    }
   	else {
		glUniform1i(materialHasDiffLocation, false);
	}

}

void InstancedAssetRenderer::render(GLuint shaderID) {
    if (instancePositions.size() == 0) {
        return;
    }
    glUseProgram(shaderID);

    GLint u_ViewMatrixLocation = glGetUniformLocation(shaderID, "u_ViewMatrix");
    glm::mat4 viewMatrix = gGameState.gCamera->GetViewMatrix();
    glUniformMatrix4fv(u_ViewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

    for (auto const& [key, modelVAO] : this->vaos) {
		updateShader(shaderID, key);
		glBindVertexArray(modelVAO);
        if (textures[key] != 0 && textures[key] < 100) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[key]);
		}
		checkOpenGLError();
        glDrawArraysInstanced(GL_TRIANGLES, 0, meshSize[key], instancePositions.size());
        glBindVertexArray(0);
    }
    glUseProgram(0);
}

