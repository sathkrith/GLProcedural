#include "shader.hpp"
#include <iostream>
using namespace std;

// Use this shader in our pipeline.
void Shader::Bind() const {
    glUseProgram(m_shaderID);
}

// Remove shader from our pipeline
void Shader::Unbind() const {
    glDeleteProgram(m_shaderID);
}

// Create a Shader from a loaded vertex and fragment shader
GLuint Shader::CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
	// Create a new program object
	GLuint programObject = glCreateProgram();

	// Compile our shaders
	GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	// Link our two shader programs together.
	// Consider this the equivalent of taking two .cpp files, and linking them into
	// one executable file.
	glAttachShader(programObject, myVertexShader);
	glAttachShader(programObject, myFragmentShader);
	glLinkProgram(programObject);

	// Validate our program
	glValidateProgram(programObject);

	// Once our final program Object has been created, we can
	// detach and then delete our individual shaders.
	glDetachShader(programObject, myVertexShader);
	glDetachShader(programObject, myFragmentShader);
	// Delete the individual shaders once we are done
	glDeleteShader(myVertexShader);
	glDeleteShader(myFragmentShader);
	m_shaderID = programObject;
	return programObject;
}
    // return the shader id
GLuint Shader::GetID() const {
    return m_shaderID;
}
// Set our uniforms for our shader.
void Shader::SetMatrix(const std::string name, const glm::mat4& value) {
    GLint u_ViewMatrixLocation = glGetUniformLocation(m_shaderID, name.c_str());
    glUniformMatrix4fv(u_ViewMatrixLocation, 1, GL_FALSE, &value[0][0]);
}

void Shader::SetFloat3(const std::string name, float v0, float v1, float v2) {
    GLint location = glGetUniformLocation(m_shaderID, name.c_str());
    glUniform3f(location, v0, v1, v2);
}

void Shader::SetInt(const std::string name, int value) {
    GLint location = glGetUniformLocation(m_shaderID, name.c_str());
    glUniform1i(location, value);
}

void Shader::SetFloatV(const std::string name, float value) {
    GLint location = glGetUniformLocation(m_shaderID, name.c_str());
    glUniform1f(location, value);
}

void Shader::SetBool(const std::string name, bool value) {
    GLint location = glGetUniformLocation(m_shaderID, name.c_str());
    glUniform1i(location, value);
}

void Shader::SetVec(const std::string name, const glm::vec3& vec) {
    GLint location = glGetUniformLocation(m_shaderID, name.c_str());
    glUniform3fv(location, 1, &vec[0]);
}
// Compiles loaded shaders
unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
	// Compile our shaders
	GLuint shaderObject;

	// Based on the type passed in, we create a shader object specifically for that
	// type.
	if (type == GL_VERTEX_SHADER) {
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}
	else if (type == GL_FRAGMENT_SHADER) {
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}

	const char* src = source.c_str();
	// The source of our shader
	glShaderSource(shaderObject, 1, &src, nullptr);
	// Now compile our shader
	glCompileShader(shaderObject);

	// Retrieve the result of our compilation
	int result;
	// Our goal with glGetShaderiv is to retrieve the compilation status
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		char* errorMessages = new char[length]; // Could also use alloca here.
		glGetShaderInfoLog(shaderObject, length, &length, errorMessages);

		if (type == GL_VERTEX_SHADER) {
			std::cout << "ERROR: GL_VERTEX_SHADER compilation failed!\n" << errorMessages << "\n";
		}
		else if (type == GL_FRAGMENT_SHADER) {
			std::cout << "ERROR: GL_FRAGMENT_SHADER compilation failed!\n" << errorMessages << "\n";
		}
		// Reclaim our memory
		delete[] errorMessages;

		// Delete our broken shader
		glDeleteShader(shaderObject);

		return 0;
	}
	return shaderObject;
}

