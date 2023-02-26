## 1

it creates a special identifier for an opengl texture and for a vertex array object (VAO).
it creates a buffer, and copies the vertex and texture coordinates data to it.
it creates a shader program, and tells the shader where to look for the vertex position data, and the data dimensions.


## 2

we started by loading the mesh to the GPU.


## 3

this is our code for the vShader:

```c++
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

// The model/view/projection matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// These outputs will be available in the fragment shader as inputs
out vec3 orig_fragPos;
out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoords;

void main()
{
	// Apply the model transformation to the 'position' and 'normal' properties of the vertex,
	// so the interpolated values of these properties will be available for usi n the fragment shader
	mat4 MainMatrix = projection * view * model;
	orig_fragPos = vec3(vec4(pos, 1.0f));
	fragPos = vec3(MainMatrix * vec4(pos, 1.0f));
	fragNormal = vec3(MainMatrix * vec4(normal,1.0f));

	// Pass the vertex texture coordinates property as it is. Its interpolated value
	// will be avilable for us in the fragment shader
	fragTexCoords = texCoords;

	// This is an internal OpenGL variable, we must set a value to this variable
	gl_Position = MainMatrix * vec4(pos, 1.0f);
}

```

## 4
We implemented a simple fragment shader that outputs a constant color.
```c++
#version 330 core

struct Material
{
	sampler2D textureMap;
	// You can add more fields here...
	// Such as:
	//		1. diffuse/specular relections constants
	//		2. specular alpha constant
	//		3. Anything else you find relevant
};

// We set this field's properties from the C++ code
uniform Material material;
uniform vec3 MeshModelColor;

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
	vec3 textureColor = vec3(texture(material.textureMap, fragTexCoords));

	frag_color = vec4(MeshModelColor,1.0f);
}
```

## 5
we loaded all the variables and parameters to the GPU.

```c++
colorShader.setUniform("model", currentModel->GetWorldTransformation() *currentModel->GetModelTransformation());
colorShader.setUniform("camera", camera.GetViewTransformation());
colorShader.setUniform("projection", camera.GetProjectionTransformation());
colorShader.setUniform("material.textureMap", 0);

colorShader.setUniform("material.objAmbientColor", currentModel->ambientColor);
colorShader.setUniform("material.objDiffuseColor", currentModel->diffuseColor);
colorShader.setUniform("material.objSpecularColor",currentModel->specularColor);

colorShader.setUniform("material.lightAmbientColor", scene->GetLigh(0->ambientColor);
colorShader.setUniform("material.lightDiffuseColor", scene->GetLigh(0->diffuseColor);
colorShader.setUniform("material.lightSpecularColor", scene->GetLigh(0->specularColor);
colorShader.setUniform("material.alpha", scene->GetLight(0)->alpha);

colorShader.setUniform("material.lightPos", scene->GetLight(0)->GetPosition());
colorShader.setUniform("material.eye", camera.GetEye());
```

## 6

## 7

## 8

#### Environment Mapping

we used 6 pictures that we found online to construct this environment.

![environmentMapping1](pics/environmentMapping1.png)

![environmentMapping2](pics/environmentMapping2.png)

![environmentMapping3](pics/environmentMapping3.png)

#### Toon Shading

we can see the levels of brightness on the ball very clearly.

![ballToonShading](pics/ballToonShading.jpg)