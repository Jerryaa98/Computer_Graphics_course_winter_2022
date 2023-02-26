#version 330 core

struct Material
{
	sampler2D textureMap;

	vec3 vertexNormal;

	vec3 objAmbientColor;
	vec3 objDiffuseColor;
	vec3 objSpecularColor;

	vec3 lightAmbientColor;
	vec3 lightDiffuseColor;
	vec3 lightSpecularColor;

	float alpha;

	vec3 lightPos;
	vec3 eye;
};

// We set this field's properties from the C++ code
uniform Material material;

float pi = 3.14159;
int toonLevels = 5;

// Inputs from vertex shader (after interpolation was applied)
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec3 orig_fragPos;

// The final color of the fragment (pixel)
out vec4 frag_color;

void main()
{
	// Sample the texture-map at the UV coordinates given by 'fragTexCoords'

	// these mappings were found on the internet to map xy coordinates to cylindrical and spherical coordinates
	vec2 planeCoords = vec2(orig_fragPos.x, orig_fragPos.y);
	vec2 cylinderCoords = vec2((atan(orig_fragPos.y , 1.5 * orig_fragPos.x) + pi * 0.5) / pi, -(-orig_fragPos.z + 0.5));
	vec2 sphericalCoords = vec2(atan(orig_fragPos.x, orig_fragPos.z) / (2 * pi), asin(orig_fragPos.y / 3) / pi);

	vec3 textureColor = vec3(texture(material.textureMap, fragTexCoords));


	// Ambient
	vec3 Ia = material.lightAmbientColor * material.objAmbientColor;


	// Diffuse
	vec3 lightVector = normalize(material.lightPos - fragPos);
	vec3 Id = material.lightDiffuseColor * material.objDiffuseColor * max(float(dot(fragNormal, lightVector)), 0.0f);

	// normal mapping
	//vec3 Id = material.lightDiffuseColor * material.objDiffuseColor * max(float(dot(normalize(textureColor), lightVector)), 0.0f);


	// Specular
	//vec3 R = normalize((2 * dot(lightVector, fragNormal) * fragNormal) - lightVector);
	//vec3 cameraLocation = material.eye;
	//vec3 V = normalize(cameraLocation - fragPos);
	//vec3 Is = material.lightSpecularColor * material.objSpecularColor * pow(max(float(dot(R, V)), 0.0f), material.alpha);

	float toonLevel = floor(pow(max(float(dot(fragNormal, lightVector)), 0.0f), material.alpha) * toonLevels);
	vec3 Is = material.lightSpecularColor * material.objSpecularColor * (toonLevel / toonLevels);


	//frag_color = normalize(vec4(Ia, material.alpha) + vec4(Id, 0.0f) + vec4(Is, 0.0f));

	frag_color = (toonLevel / toonLevels) * normalize(vec4(Ia, material.alpha) + vec4(Id, 0.0f) + vec4(Is, 0.0f));

	//frag_color = vec4(textureColor, 1.0f);

}