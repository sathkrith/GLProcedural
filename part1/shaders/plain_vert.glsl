#version 410 core
// From Vertex Buffer Object (VBO)
// The only thing that can come 'in', that is
// what our shader reads, the first part of the
// graphics pipeline.

layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexNormals;

// Uniform variables
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Projection; // We'll use a perspective projection
uniform vec3 u_lightPosition;


uniform vec3 u_color; 

// Pass vertex colors into the fragment shader
out vec3 v_vertexNormal;
out vec3 v_fragmentPosition;
out vec3 v_lightPosition;    
out vec3 v_color;
void main()
{
    // Send to fragment shader the normals
    // and the vertex colors
    v_vertexNormal = vertexNormals;
    v_color = u_color;
    v_lightPosition = u_lightPosition;

    // Calculate in world space the position of the vertex
    v_fragmentPosition  = vec3(u_ModelMatrix * vec4(position, 1.0));


    // Compute the MVP matrix
    gl_Position = u_Projection *  u_ViewMatrix * u_ModelMatrix * vec4(position,1.0f);
}