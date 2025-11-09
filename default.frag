#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;


// Inputs the texture coordinates from the Vertex Shader
in vec2 texCoord;
in vec3 Normal;
// Imports the current position from the Vertex Shader
in vec3 crntPos;

// Gets the Texture Unit from the main function
uniform sampler2D diffuse1;
uniform vec4 lightColor;
// Gets the position of the light from the main function
uniform vec3 lightPos;
uniform vec3 camPos;


void main()
{		
	float ambient = 0.2f;
	vec3 surfaceNormal = normalize(Normal);
	vec3 lightDir = normalize(lightPos - crntPos);
	vec3 viewDir = normalize(camPos - crntPos);
	vec3 reflectDir = reflect(-lightDir, surfaceNormal);
	float specularStrength = 0.5f;

	float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	float intensity = max(dot(surfaceNormal, lightDir), 0.0f);
	FragColor = texture(diffuse1, texCoord)  * lightColor * (intensity + ambient + specIntensity); 
}