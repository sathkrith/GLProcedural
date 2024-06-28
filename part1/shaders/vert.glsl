#version 410 core
// From Vertex Buffer Object (VBO)
// The only thing that can come 'in', that is
// what our shader reads, the first part of the
// graphics pipeline.

layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexNormals;
layout(location=2) in vec2 vertexTextCords;
layout(location=3) in vec3 vertexTangent;
layout(location=4) in vec3 vertexBiTangent;

// Uniform variables
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Projection; 
uniform vec3 u_viewPosition;
uniform vec3 u_lightPosition;
// color types
uniform vec3 u_color; 

// Pass vertex colors into the fragment shader
out vec3 v_vertexNormal;
out vec3 v_fragmentPosition;
out vec2 v_textureCordinates;
out vec3 v_tangentLightPos;
out vec3 v_tangentViewPos;
out vec3 v_color;

void main()
{
    // Send to fragment shader the normals
    // and the vertex colors
    v_textureCordinates = vertexTextCords;
    // Compute TBN matrix
    vec3 T = normalize(vec3(u_ModelMatrix * vec4(vertexTangent,   0.0)));
    vec3 B = normalize(vec3(u_ModelMatrix * vec4(vertexBiTangent, 0.0)));
    vec3 N = normalize(vec3(u_ModelMatrix * vec4(vertexNormals,    0.0)));
    mat3 TBN = transpose(mat3(T, B, N));
    v_vertexNormal = TBN * vertexNormals;
    v_color = vec3(0.4, 0.0, 0.0);
    // Calculate in world space the position of the vertex
    v_tangentLightPos = TBN * u_lightPosition;
    v_tangentViewPos  = TBN * u_viewPosition;
    v_fragmentPosition  = TBN * vec3(u_ModelMatrix * vec4(position, 1.0));


    // Compute the MVP matrix
    gl_Position = u_Projection *  u_ViewMatrix * u_ModelMatrix * vec4(position,1.0f);
}