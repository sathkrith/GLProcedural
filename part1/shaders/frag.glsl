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

struct Material {
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;

    bool bSpecMap;
    bool bDiffMap;
    bool bNormMap;
};

// Materials or shaders
uniform Material u_materials[8];
uniform sampler2D u_diffuseMap;
uniform sampler2D u_specularMap;
uniform sampler2D u_normalMap;

// Uniform Light Variables
uniform PointLight u_light;
uniform int u_materialID;

// Properties from vertex shader
in vec3 v_vertexNormal;
in vec3 v_fragmentPosition;
in vec2 v_textureCordinates;
in vec3 v_tangentViewPos;
in vec3 v_tangentLightPos;
in vec3 v_color;

out vec4 fragmentColor;


// Entry point of program
void main()
{
    
	// Sample diffuse and specular maps
    vec3 diffuseColor = v_color;
    vec3 specularColor = vec3(1.0,1.0,1.0);
    vec3 normalMapValue = v_vertexNormal;
    int v_materialID = u_materialID;
    
    if(u_materials[v_materialID].bDiffMap)
        diffuseColor = texture(u_diffuseMap, v_textureCordinates).rgb;
    if(u_materials[v_materialID].bSpecMap)
        specularColor = texture(u_specularMap, v_textureCordinates).rgb;
    if(u_materials[v_materialID].bNormMap)
        normalMapValue = normalize(texture(u_normalMap, v_textureCordinates).rgb * 2 - 1.0f);
    vec3 lightDirection = normalize(v_tangentLightPos - v_fragmentPosition);

    
    // Calculate ambient component
    vec3 ambient = u_materials[v_materialID].ambientColor * u_light.ambientColor ;

    // attenuation
	float distance = length(v_tangentLightPos - v_fragmentPosition);
	float attenuation = 1.0/(u_light.attenConstant + u_light.attenLinear * distance + u_light.attenQuadratic * distance * distance);

	
    // Calculate diffuse component
    vec3 normal = normalize(normalMapValue);
    float diffuseIntensity = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = u_light.diffuseColor * diffuseIntensity * diffuseColor * attenuation;

    // Calculate specular component
    vec3 viewDirection = normalize(v_tangentViewPos-v_fragmentPosition);
    vec3 reflectDirection = reflect(-lightDirection, normalMapValue);
    float specularIntensity = pow(max(dot(viewDirection, reflectDirection), 0.0), u_materials[v_materialID].shininess);
    vec3 specular = u_light.specularColor * specularIntensity * specularColor * u_materials[v_materialID].specularColor * attenuation;

    // Calculate final color
    vec3 finalColor = ambient + diffuse + specular;
    
    fragmentColor = vec4(finalColor, 1.0);
}