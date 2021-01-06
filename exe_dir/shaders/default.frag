#version 330

in vec3 position;
in vec3 normal;
in vec2 texCoords;

out vec4 fColor;

uniform vec3 viewPos;

// Informacje o Ÿród³ach œwiat³a
struct DirectLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight { 
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic; 
};

uniform DirectLight directLight;
uniform PointLight pointLight;

// Informacje o materiale
struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

// Informacje o teksturach
uniform bool texturing;
uniform sampler2D diffuseTex;
uniform sampler2D specularTex;


vec3 diffuseColor;
vec3 specularColor;

vec3 CalcDirectLight(DirectLight light, vec3 norm, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 norm, vec3 viewDir);

void main()
{
    diffuseColor = material.diffuse;
    specularColor = material.specular;

    if (texturing)
    {
        diffuseColor *= vec3(texture(diffuseTex, texCoords));
        specularColor *= vec3(texture(specularTex, texCoords));
    }

    vec3 norm    = normalize(normal);
    vec3 viewDir = normalize(viewPos - position);

    vec3 result = vec3(0.0);
    result += CalcDirectLight(directLight, norm, viewDir);
    result += CalcPointLight(pointLight, norm, viewDir);
        
    fColor = vec4(clamp(result, 0.0, 1.0), 1.0);
}

vec3 CalcDirectLight(DirectLight light, vec3 norm, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    vec3 ambient  = diffuseColor * light.ambient;
    vec3 diffuse  = diff * diffuseColor * light.diffuse;
    vec3 specular = spec * specularColor * light.specular;
    
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - position);
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // Attenuation
    float distance    = length(light.position - position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			            light.quadratic * (distance * distance));    

    vec3 ambient  = diffuseColor * light.ambient;
    vec3 diffuse  = diff * diffuseColor * light.diffuse;
    vec3 specular = spec * specularColor * light.specular;

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}
