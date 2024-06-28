#version 410 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 instancePosition;
layout (location = 3) in float instanceRotation; 
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_Projection; // We'll use a perspective projection  // Model-View-Projection matrix

out vec2 UV;

void main() {
    float radians = instanceRotation * 3.14159 / 180.0; // Convert degrees to radians
    mat4 rotationMatrix = mat4(cos(radians), 0.0, sin(radians), 0.0,
                               0.0,         1.0, 0.0,          0.0,
                              -sin(radians), 0.0, cos(radians), 0.0,
                               0.0,         0.0, 0.0,          1.0);

    mat4 translationMatrix = mat4(1.0, 0.0, 0.0, 0.0,
                                  0.0, 1.0, 0.0, 0.0,
                                  0.0, 0.0, 1.0, 0.0,
                                  instancePosition.x, instancePosition.y, instancePosition.z, 1.0); // Translate according to instance position

    mat4 instanceModel = u_ModelMatrix *translationMatrix  * rotationMatrix;
    gl_Position = u_Projection *  u_ViewMatrix * u_ModelMatrix * vec4(vertexPosition + instancePosition, 1.0);
    UV = vertexUV;
}