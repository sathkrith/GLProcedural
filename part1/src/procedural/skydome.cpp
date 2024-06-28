#pragma once
#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp> 
#include "game_state.hpp"
#include <asset/ppm.hpp>
#include <thread>


void skyDome::initialize(int slices, int stacks) {
        std::string instvertexSource = LoadShaderAsString("./shaders/dome_vert.glsl");
        std::string instfragmentSource = LoadShaderAsString("./shaders/dome_shader.glsl");
        shaderProgram = CreateShaderProgram(instvertexSource, instfragmentSource);
        // Create VAO, VBO, and EBO
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLfloat skyboxVertices[] = {
            // Positions          
       -1.0f,  1.0f, -1.0f,
       -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
       -1.0f,  1.0f, -1.0f,

       -1.0f, -1.0f,  1.0f,
       -1.0f, -1.0f, -1.0f,
       -1.0f,  1.0f, -1.0f,
       -1.0f,  1.0f, -1.0f,
       -1.0f,  1.0f,  1.0f,
       -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

       -1.0f, -1.0f,  1.0f,
       -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
       -1.0f, -1.0f,  1.0f,

       -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
       -1.0f,  1.0f,  1.0f,
       -1.0f,  1.0f, -1.0f,

       -1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
        };


        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (void*)0);

        // Unbind VAO
        glBindVertexArray(0);

        glUseProgram(shaderProgram);

        // Projection matrix (in perspective) 
        glm::mat4 perspective = glm::perspective(glm::radians(90.0f),
            (float)gGameState.gScreenWidth / (float)gGameState.gScreenHeight,
            0.1f,
            100.0f);



        // Retrieve our location of our perspective matrix uniform 
        GLint u_ProjectionLocation = glGetUniformLocation(shaderProgram, "u_Projection");
        glUniformMatrix4fv(u_ProjectionLocation, 1, GL_FALSE, &perspective[0][0]);

        loadCubemap();
        GLuint textureLoc = glGetUniformLocation(shaderProgram, "skybox");
        glUniform1i(textureLoc, 0);
        
        glUseProgram(0);
}

void skyDome::loadCubemap()
{
    glGenTextures(1, &skyTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyTextureID);
    GLint error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error setting texture:" << std::hex << error << "\n";
    }
    int width, height, nrChannels;
    std::optional<PPM> ppm = PPM::LoadPPM(constants.MediaDir /  "objects"/"skybox"/"middle.ppm");
    PPM p = ppm.value();
    p.FlipImageVertically();
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error setting texture:" << std::hex << error << "\n";
    }
    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, p.width, p.height, GL_RGB, GL_UNSIGNED_BYTE, &ppm.value().pixels[0]);
    error =  glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error setting texture:" << std::hex << error << "\n";
    }
    ppm = PPM::LoadPPM(constants.MediaDir / "objects" / "skybox" / "left.ppm");
    p = ppm.value();
    p.FlipImageVertically();
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error setting texture:" << error << "\n";
    }
    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0, 0, p.width, p.height, GL_RGB, GL_UNSIGNED_BYTE, &ppm.value().pixels[0]);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error setting texture:" << std::hex << error << "\n";
    }
    ppm = PPM::LoadPPM(constants.MediaDir / "objects" / "skybox" / "top.ppm");
    p = ppm.value();
    p.FlipImageVertically();
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error setting texture:" << error << "\n";
    }
    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0, 0, p.width, p.height, GL_RGB, GL_UNSIGNED_BYTE, &ppm.value().pixels[0]);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error setting texture:" << std::hex << error << "\n";
    }
    ppm = PPM::LoadPPM(constants.MediaDir /  "objects" / "skybox" / "bot.ppm");
    p = ppm.value();
    p.FlipImageVertically();
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error setting texture:" << error << "\n";
    }
    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0, 0, p.width, p.height, GL_RGB, GL_UNSIGNED_BYTE, &ppm.value().pixels[0]);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error setting texture:" << std::hex << error << "\n";
    }
    ppm = PPM::LoadPPM(constants.MediaDir /"objects" / "skybox" / "right1.ppm");
    p = ppm.value();
    p.FlipImageVertically();
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error setting texture:" << error << "\n";
    }
    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, p.width, p.height, GL_RGB, GL_UNSIGNED_BYTE, &ppm.value().pixels[0]);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error setting texture:" << std::hex << error << "\n";
    }
    ppm = PPM::LoadPPM(constants.MediaDir /  "objects" / "skybox" / "right2.ppm");
    p = ppm.value();
    p.FlipImageVertically();
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0, 0, p.width, p.height, GL_RGB, GL_UNSIGNED_BYTE, &ppm.value().pixels[0]);
    error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error setting texture:" << std::hex << error << "\n";
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
 }

void skyDome::draw() {
    glDepthFunc(GL_LEQUAL);
    //glDisable(GL_DEPTH_TEST);
    glUseProgram(shaderProgram);
   // glDepthMask(GL_FALSE);  // Disable depth writing
    // Bind texture

    glBindVertexArray(vao);
    GLint u_ViewMatrixLocation = glGetUniformLocation(shaderProgram, "u_ViewMatrix");
    glm::mat4 viewMatrix = glm::mat4(glm::mat3(gGameState.gCamera->GetViewMatrix()));
    //glm::mat4 viewMatrix = gGameState.gCamera->GetViewMatrix();
    glUniformMatrix4fv(u_ViewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
   // glDepthMask(GL_TRUE);  // Re-enable depth writing
    glUseProgram(0);
    glDepthFunc(GL_LESS);
	glBindVertexArray(0);
    //glEnable(GL_DEPTH_TEST);
}
