#version 410 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Projection; // We'll use a perspective projection

void main() {
    TexCoords = aPos;
    vec4 pos = u_ViewMatrix* vec4(aPos, 1.0);
    gl_Position = pos.xyww; // This ensures the skydome is always at maximum depth
}