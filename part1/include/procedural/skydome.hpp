#pragma once
#include <vector>


struct Vertex {
    float x, y, z;
    float u, v;  // Texture coordinates
};

class skyDome {
    GLuint vao, vbo, ebo;
    GLuint shaderProgram;
    GLuint skyTextureID;
    void loadCubemap();
public:
    skyDome() {}
    void initialize(int slices, int stacks);

    void draw();
};