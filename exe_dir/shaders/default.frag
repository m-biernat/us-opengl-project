#version 330

// Informacje o Ÿród³ach œwiat³a
struct DirectLight {
    vec3 direction;
    vec3 color;
};

struct PointLight { 
    vec3 position;
    vec3 color;
};

uniform DirectLight directLight;
uniform PointLight pointLight;
uniform vec3 ambientLight;

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

uniform vec3 viewPos;

in vec3 position;
in vec3 normal;
in vec2 texCoords;

out vec4 fColor;
 
void main()
{
    vec3 diffuse = vec3(1.0);
    vec3 specular = vec3(1.0);

    if (texturing)
    {
    	diffuse = texture(diffuseTex, texCoords).rgb;
        specular = texture(specularTex, texCoords).rgb; 
    }

    diffuse *= material.diffuse;
    specular *= material.specular;

    // Ambient
    vec3 ambient = diffuse * ambientLight;
  	
    // Diffuse 
    vec3 norm = normalize(normal);
	//vec3 lightDir = normalize(vec3(-directLight.direction));
    vec3 lightDir = normalize(vec3(pointLight.position) - position);
    diffuse *= max(dot(norm, lightDir), 0.0) * pointLight.color; 
    
    // Specular
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    specular *=  spec * pointLight.color;
        
    vec3 result = clamp(ambient + diffuse + specular, 0.0, 1.0);
    fColor = vec4(result, 1.0);
}
