#pragma once

#include <string>
#include <glad/glad.h>
#include "glm/matrix.hpp"
using namespace std;

struct Shader {
	// Use this shader in our pipeline.
	void Bind() const;
	// Remove shader from our pipeline
	void Unbind() const;

	// Create a Shader from a loaded vertex and fragment shader
	GLuint CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	// return the shader id
	GLuint GetID() const;
	// Set our uniforms for our shader.
	void SetMatrix(const std::string name, const glm::mat4 &value);
	void SetFloat3(const std::string name, float v0, float v1, float v2);
	void SetInt(const std::string name, int value);
	void SetBool(const std::string name, bool value);
	void SetFloatV(const std::string name, float value);
	void SetVec(const std::string name, const glm::vec3& vec);

	// Compiles loaded shaders
	unsigned int CompileShader(unsigned int type, const std::string& source);
	
	// The unique shaderID
	GLuint m_shaderID;
};
