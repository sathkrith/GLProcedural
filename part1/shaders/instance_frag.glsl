#version 410 core
struct Material {
    vec3 diffuseColor;
    bool bDiffMap;
};
uniform Material u_material;

in vec2 UV;
out vec3 color;
uniform sampler2D u_texture;

void main() {
    if(u_material.bDiffMap)
        color = texture(u_texture, UV).rgb;
    else
        color = u_material.diffuseColor;
}