#version 410 core

#define SHINE = 0.1;

struct PointLight{
	float attenConstant;
	float attenLinear;
	float attenQuadratic;
	
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
};

// Uniform Light Variables
uniform PointLight u_light;

// Properties from vertex shader
in vec3 v_vertexNormal;
in vec3 v_fragmentPosition;
in vec3 v_lightPosition;
in vec3 v_color;

out vec4 fragmentColor;

// Entry point of program
void main()
{
	vec3 lightVec = v_lightPosition - v_fragmentPosition;

	// Calculate ambient component
    vec3 ambient = u_light.ambientColor * v_color;
    vec3 lightDirection = normalize(lightVec);

    // attenuation
	float distance = length(lightVec);
	float attenuation = 1.0/(u_light.attenConstant + u_light.attenLinear * distance + u_light.attenQuadratic * distance * distance);

        // Calculate diffuse component
    vec3 normal = normalize(v_vertexNormal);
    float diffuseIntensity = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = u_light.diffuseColor * diffuseIntensity * v_color * attenuation;

    // Calculate final color
    vec3 finalColor = ambient + diffuse;
    fragmentColor = vec4(finalColor, 1.0);	
}