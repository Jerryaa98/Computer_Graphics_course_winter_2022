#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

uniform mat4 model;			// model transformations
uniform mat4 camera;		// camera transformations
uniform mat4 projection;	// camera projection

// These outputs will be available in the fragment shader as inputs
out vec3 orig_fragPos;
out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoords;

void main()
{
	orig_fragPos = pos, 1.0f;
	fragPos = vec3(model * vec4(pos, 1.0f));
	fragNormal = vec3(model * vec4(normal, 1.0f));

	fragTexCoords = texCoords;

	gl_Position = projection * camera *  model * vec4(pos, 1.0f);
}