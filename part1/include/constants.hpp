#pragma once
#include <array>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <filesystem>

// Represents a triangle
struct Triangle {
    std::array<glm::vec3, 3> vertices = { glm::vec3(0.0,0.0,0.0) }; // 3 vertices per triangle
    std::array<glm::vec3, 3> normals = { glm::vec3(0.0,0.0,0.0) }; // 3 normals per triangle
    std::array<glm::vec2, 3> tex = { glm::vec2(0.0,0.0) };    // 2 texture co-ordinates per triangle
    glm::vec3 tangent = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 biTangent = glm::vec3(0.0, 0.0, 0.0);
};

// Asset types supported in the program.
enum class AssetType {
    UNKNOWN = 0,
    OBJ,
    STL,
    MTL,
    PPM
};

// Asset states.
enum class AssetState {
    BAD = 0,
    GOOD
};

// Asset color types supported.
enum class ColorType {
    color = 0,
    texture,
    material
};

struct global {
    const std::filesystem::path MediaDir = std::filesystem::path("./media");
    const std::filesystem::path SceneDir = std::filesystem::path("./media/scene");
    const std::filesystem::path ShaderDir = std::filesystem::path("./shaders");
};
struct Cell {
    float height;
    float water;
    float sediment;
    float flow;
};
typedef  std::vector<std::vector<Cell>> HeightMap;

extern global constants;
