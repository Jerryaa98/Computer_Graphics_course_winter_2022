#include "MeshModel.h"
#include <iostream>

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
	faces(faces),
	vertices(vertices),
	normals(normals),
	model_name(model_name)
{
    // init matrices
    this->objectTransform = glm::mat4x4(1.0f);
    this->worldTransform = glm::mat4x4(1.0f);

    float max = -1.0f * FLT_MAX;
    float min = FLT_MAX;

    float x, y, z;

    for (int i = 0; i < vertices.size(); i++) {
        x = vertices[i].x;
        y = vertices[i].y;
        z = vertices[i].z;

        // get maximum
        if (x > max) {
            max = x;
        }
        if (y > max) {
            max = y;
        }
        if (z > max) {
            max = z;
        }

        // get minimum
        if (x < min) {
            min = x;
        }
        if (y < min) {
            min = y;
        }
        if (z < min) {
            min = z;
        }
    }

    max = abs(max);
    min = abs(min);

    if (min > max)
        max = min;

    float scaleFactor = 500.0f / max;
    localScaleArray[0] = scaleFactor;
    localScaleArray[1] = scaleFactor;
    localScaleArray[2] = scaleFactor;
    this->maxScale = 2 * scaleFactor;
}

MeshModel::~MeshModel()
{
}

const Face& MeshModel::GetFace(int index) const
{
	return faces[index];
}

int MeshModel::GetFacesCount() const
{
	return faces.size();
}

const std::string& MeshModel::GetModelName() const
{
	return model_name;
}

glm::mat4x4 MeshModel::GetTransform() {
    // calculate local transformations
    if (lockLocalScale) {
        localScaleMat[0][0] = localScaleLocked;
        localScaleMat[1][1] = localScaleLocked;
        localScaleMat[2][2] = localScaleLocked;
    }
    else {
        localScaleMat[0][0] = localScaleArray[0];
        localScaleMat[1][1] = localScaleArray[1];
        localScaleMat[2][2] = localScaleArray[2];
    }

    localTranslateMat[3][0] = localTranslateArray[0];
    localTranslateMat[3][1] = localTranslateArray[1];
    localTranslateMat[3][2] = localTranslateArray[2];

    localRotateXMat[1][1] = cos(glm::radians(localRotateArray[0]));
    localRotateXMat[1][2] = sin(glm::radians(localRotateArray[0]));
    localRotateXMat[2][1] = -sin(glm::radians(localRotateArray[0]));
    localRotateXMat[2][2] = cos(glm::radians(localRotateArray[0]));

    localRotateYMat[0][0] = cos(glm::radians(localRotateArray[1]));
    localRotateYMat[0][2] = sin(glm::radians(localRotateArray[1]));
    localRotateYMat[2][0] = -sin(glm::radians(localRotateArray[1]));
    localRotateYMat[2][2] = cos(glm::radians(localRotateArray[1]));

    localRotateZMat[0][0] = cos(glm::radians(localRotateArray[2]));
    localRotateZMat[0][1] = sin(glm::radians(localRotateArray[2]));
    localRotateZMat[1][0] = -sin(glm::radians(localRotateArray[2]));
    localRotateZMat[1][1] = cos(glm::radians(localRotateArray[2]));


    // calculate world transformations
    if (lockWorldScale) {
        worldScaleMat[0][0] = worldScaleLocked;
        worldScaleMat[1][1] = worldScaleLocked;
        worldScaleMat[2][2] = worldScaleLocked;
    }
    else {
        worldScaleMat[0][0] = worldScaleArray[0];
        worldScaleMat[1][1] = worldScaleArray[1];
        worldScaleMat[2][2] = worldScaleArray[2];
    }

    worldTranslateMat[3][0] = worldTranslateArray[0];
    worldTranslateMat[3][1] = worldTranslateArray[1];
    worldTranslateMat[3][2] = worldTranslateArray[2];

    worldRotateXMat[1][1] = cos(glm::radians(worldRotateArray[0]));
    worldRotateXMat[1][2] = sin(glm::radians(worldRotateArray[0]));
    worldRotateXMat[2][1] = -sin(glm::radians(worldRotateArray[0]));
    worldRotateXMat[2][2] = cos(glm::radians(worldRotateArray[0]));

    worldRotateYMat[0][0] = cos(glm::radians(worldRotateArray[1]));
    worldRotateYMat[0][2] = sin(glm::radians(worldRotateArray[1]));
    worldRotateYMat[2][0] = -sin(glm::radians(worldRotateArray[1]));
    worldRotateYMat[2][2] = cos(glm::radians(worldRotateArray[1]));

    worldRotateZMat[0][0] = cos(glm::radians(worldRotateArray[2]));
    worldRotateZMat[0][1] = sin(glm::radians(worldRotateArray[2]));
    worldRotateZMat[1][0] = -sin(glm::radians(worldRotateArray[2]));
    worldRotateZMat[1][1] = cos(glm::radians(worldRotateArray[2]));


    objectTransform = localTranslateMat * localRotateXMat * localRotateYMat * localRotateZMat * localScaleMat;
    worldTransform = worldTranslateMat * worldScaleMat * worldRotateXMat * worldRotateYMat * worldRotateZMat;
    glm::mat4x4 transform = worldTransform * objectTransform;
    return transform;

}

const std::vector<glm::vec3> MeshModel::getTransformedVertices() {
    std::vector<glm::vec3> newVertices;
    glm::mat4x4 matrix = this->GetTransform();
    int verticesCount = this->vertices.size();
    for (int i = 0; i < verticesCount; i++) {
        glm::vec4 vector = matrix * glm::vec4(this->vertices.at(i), 1.0f);

        // cut the w coordinate
        //newVertices.push_back(glm::vec3(vector.x, vector.y, vector.z));
        newVertices.push_back(glm::vec3(vector.x / vector.w, vector.y / vector.w, vector.z / vector.w));
    }
    return newVertices;
}

glm::vec2 MeshModel::GetVertixPoint(int i)
{
	return vertices[i];
}