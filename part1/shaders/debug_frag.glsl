#version 410 core

// Materials or shaders
uniform sampler2D u_dirtMap;
uniform sampler2D u_rockMap;
uniform sampler2D u_grassMap;
uniform sampler2D u_noiseMap;
uniform float u_MaxHeight;

// Properties from vertex shader
in vec3 v_vertexNormal;
in vec3 v_fragmentPosition;
in vec2 v_textureCordinates;
in vec3 v_tangentViewPos;
out vec4 fragmentColor;

// Function to create pseudo-random number based on screen position
float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

// Entry point of program
void main()
{
    float noiseAmount = 0.1;
    float noise = texture(u_noiseMap, v_textureCordinates*10).r;
	vec3 v_grassColor = texture(u_grassMap, v_textureCordinates+ noise* 0.05).rgb;
    vec3 v_rockColor = texture(u_rockMap, v_textureCordinates+ noise* 0.05).rgb;
    vec3 v_dirtColor = texture(u_dirtMap, v_textureCordinates+ noise* 0.05).rgb;
    double height = v_fragmentPosition.y;
	double heightRatio = height / u_MaxHeight;

    float lowerBlend = float(smoothstep(0.2, 0.4, heightRatio));  // Between texture1 and texture2
    float upperBlend = float(smoothstep(0.6, 0.8, heightRatio));  // Between texture2 and texture3

    // Mix textures based on height and noise
    vec3 finalColor = mix(vec3(mix(v_grassColor, v_dirtColor, lowerBlend)), v_rockColor, upperBlend);
         // Add random variation
    noise = rand(v_fragmentPosition.xy);
    vec3 colorVariation = 0.05 * vec3(noise); // Small variation
    finalColor = clamp(finalColor + colorVariation, 0.0, 1.0); // Ensure color stays within valid range
    fragmentColor = vec4(finalColor, 1.0);
}