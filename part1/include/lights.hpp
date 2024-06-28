#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include "utils.hpp"

struct Light{
    glm::vec3 position;
    glm::vec3 diffuseColor = { 1.0f, 1.0f, 1.0f };
    glm::vec3 ambientColor = { 0.5f, 0.5f, 0.5f };
    glm::vec3 specularColor = { 1.0f, 1.0f, 1.0f };
    float ambientStrength = 1.0f;
    float specularStrength = 0.5f;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

	GLuint mShaderID;	
    GLuint mVAO;
    GLuint mVBO;

    /// Constructor
	Light(glm::vec3 position);
    Light();
    // Initialization function that can be called after
    // OpenGL has been setup
    void Initialize();

    // Operations that happen before drawing
    void PreDraw();
	// Draw a light
	void Draw();

    ~Light();
};


#endif