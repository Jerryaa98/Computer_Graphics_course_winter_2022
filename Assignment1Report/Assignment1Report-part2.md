# Assignment1Report_part2

Using std we added a printing function to the code which prints the vectors of the vertices and faces, like in the image below :-

object used in the demo: bunny

```c++
if (lineType == "v")
		{
     vertices.push_back(Utils::Vec3fFromStream(issLine));
		 std::cout << issLine.str() << std::endl;
		}
		else if (lineType == "vn")
		{
			normals.push_back(Utils::Vec3fFromStream(issLine));
		}
		else if (lineType == "vt")
		{
			// TODO: Handle texture coordinates
		}
		else if (lineType == "f")
		{
			faces.push_back(Face(issLine));
			std::cout << issLine.str() << std::endl;

		}
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
		}
		else
		{
			// std::cout << "Found unknown line Type \"" << lineType << "\"";
		}
```

![VerticesAndFaces](part2Pics/VerticesAndFaces.jpg)

We added a scaling factor and a transaltion to the object in order for it to fit the screen properly like so:-
```c++
point1 = model.getTransformedVertices().at(model.GetFace(j).GetVertexIndex(0) - 1);
			point2 = model.getTransformedVertices().at(model.GetFace(j).GetVertexIndex(1) - 1);
			point3 = model.getTransformedVertices().at(model.GetFace(j).GetVertexIndex(2) - 1);

			point1 += glm::vec2(half_width, half_height);
			point2 += glm::vec2(half_width, half_height);
			point3 += glm::vec2(half_width, half_height);
			
			DrawLine(point1, point2, glm::vec3(1.0f));
			DrawLine(point1, point3, glm::vec3(1.0f));
			DrawLine(point2, point3, glm::vec3(1.0f));
```

we sort the value of the vectors in the GUI code. 

now in getTransformedVertices() we cut the w coordinate and divide each coordinate by it, then we drew it using the DrawLine() function we implimented the previous task.

this is the result, object used in the demonstration: bishop and teapot

![bishop](part2Pics/bishop.jpg)
![teapot](part2Pics/teapot.jpg)

### Transformation Gui
we can control each transformation individually by this window
![tranformation_gui](part2Pics/transformation_gui.jpg)


### Scaling:
first we calculated the scale factor by this code:
``` c++
for (int i = 0; i < vertices.size(); i++) {
        x = vertices[i].x;
        y = vertices[i].y;
        z = vertices[i].z;

        // get maximum
        if (x > y) 
            max = x;
        else 
            max = y;
        if (z > max) 
            max = z;
        

        // get minimum
        if (x < y) 
            min = x;
        else
            min = y;

        if (z < min) 
            min = z;
        
    }

    max = abs(max);
    min = abs(min);

    if (min > max)
        max = min;

    float scaleFactor = 500.0f / max;

```
and then we saved the scalingfactor as the value of the local scaling vector.

```C++

    localScaleVector[0] = scaleFactor;
    localScaleVector[1] = scaleFactor;
    localScaleVector[2] = scaleFactor;
    this->maxScale = 2 * scaleFactor;
```

we calculate the transform matrix by mutiplying the local transfrom matrix with the world transform matrix, the code does not need any explaination of how we calculate the world transform matrix nd the local transform matrix:
```c++
    localTransform = localTranslateMat * localRotateXMat * localRotateYMat *     localRotateZMat * localScaleMat;
    worldTransform = worldTranslateMat * worldScaleMat * worldRotateXMat *       worldRotateYMat * worldRotateZMat;
    glm::mat4x4 transform = worldTransform * localTransform;
    return transform;
 ```   
we used the function GetTransform to calculate the local scale matrix, the rotate matrix.....

```c++
glm::mat4x4 MeshModel::GetTransform() {
    // calculate local transformations
    if (uniformLocalScale) {
        localScaleMat[0][0] = localScale;
        localScaleMat[1][1] = localScale;
        localScaleMat[2][2] = localScale;
    }
    else {
        localScaleMat[0][0] = localScaleVector[0];
        localScaleMat[1][1] = localScaleVector[1];
        localScaleMat[2][2] = localScaleVector[2];
    }

    localTranslateMat[3][0] = localTranslateVector[0];
    localTranslateMat[3][1] = localTranslateVector[1];
    localTranslateMat[3][2] = localTranslateVector[2];

    localRotateXMat[1][1] = cos(glm::radians(localRotateVector[0]));
    localRotateXMat[1][2] = sin(glm::radians(localRotateVector[0]));
    localRotateXMat[2][1] = -sin(glm::radians(localRotateVector[0]));
    localRotateXMat[2][2] = cos(glm::radians(localRotateVector[0]));

    localRotateYMat[0][0] = cos(glm::radians(localRotateVector[1]));
    localRotateYMat[0][2] = sin(glm::radians(localRotateVector[1]));
    localRotateYMat[2][0] = -sin(glm::radians(localRotateVector[1]));
    localRotateYMat[2][2] = cos(glm::radians(localRotateVector[1]));

    localRotateZMat[0][0] = cos(glm::radians(localRotateVector[2]));
    localRotateZMat[0][1] = sin(glm::radians(localRotateVector[2]));
    localRotateZMat[1][0] = -sin(glm::radians(localRotateVector[2]));
    localRotateZMat[1][1] = cos(glm::radians(localRotateVector[2]));


    // calculate world transformations
    if (uniformWorldScale) {
        worldScaleMat[0][0] = worldScale;
        worldScaleMat[1][1] = worldScale;
        worldScaleMat[2][2] = worldScale;
    }
    else {
        worldScaleMat[0][0] = worldScaleVector[0];
        worldScaleMat[1][1] = worldScaleVector[1];
        worldScaleMat[2][2] = worldScaleVector[2];
    }

    worldTranslateMat[3][0] = worldTranslateVector[0];
    worldTranslateMat[3][1] = worldTranslateVector[1];
    worldTranslateMat[3][2] = worldTranslateVector[2];

    worldRotateXMat[1][1] = cos(glm::radians(worldRotateVector[0]));
    worldRotateXMat[1][2] = sin(glm::radians(worldRotateVector[0]));
    worldRotateXMat[2][1] = -sin(glm::radians(worldRotateVector[0]));
    worldRotateXMat[2][2] = cos(glm::radians(worldRotateVector[0]));

    worldRotateYMat[0][0] = cos(glm::radians(worldRotateVector[1]));
    worldRotateYMat[0][2] = sin(glm::radians(worldRotateVector[1]));
    worldRotateYMat[2][0] = -sin(glm::radians(worldRotateVector[1]));
    worldRotateYMat[2][2] = cos(glm::radians(worldRotateVector[1]));

    worldRotateZMat[0][0] = cos(glm::radians(worldRotateVector[2]));
    worldRotateZMat[0][1] = sin(glm::radians(worldRotateVector[2]));
    worldRotateZMat[1][0] = -sin(glm::radians(worldRotateVector[2]));
    worldRotateZMat[1][1] = cos(glm::radians(worldRotateVector[2]));

```


