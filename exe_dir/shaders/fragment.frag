#version 330

// informacje o zrodle swiatla
uniform vec4 lightPosition; // we wspolrzednych oka
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

// informacje o materiale
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;

uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform bool texturing;

in vec3 position; // interpolowana pozycja
in vec3 normal; // interpolowany normal
in vec2 texCoords;

out vec4 fColor; // wyjsciowy kolor fragmentu
 
void main()
{
	vec3 norm = normalize( normal );
	vec3 lightDir = normalize( vec3( lightPosition ) - position );
	vec3 viewDir = normalize( vec3( 0.0, 0.0, 0.0 ) - position );
	vec3 halfDir = normalize( lightDir + viewDir );

	vec3 ambient = lightAmbient * materialAmbient;
	//vec3 ambient = lightAmbient * vec3(texture(diffuseTex, texCoords));

	vec3 diffuse = lightDiffuse * max( dot( lightDir, norm ), 0.0 );

	if (texturing)
		diffuse *= vec3(texture(diffuseTex, texCoords));
	else
		diffuse *= materialDiffuse;

	vec3 specular = vec3( 0.0, 0.0, 0.0 );
	if( dot( lightDir, viewDir ) > 0.0 ) 
	{
		specular = pow( max( 0.0, dot( norm, halfDir ) ), materialShininess ) * lightSpecular;
			
		if (texturing)
			specular *= vec3(texture(specularTex, texCoords));
		else
			specular *= materialSpecular;
	}

	fColor = vec4( clamp( ambient + diffuse + specular, 0.0, 1.0 ), 1.0 );
}