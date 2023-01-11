#include "MeshModel.h"
#include <iostream>
#include <glm\gtx\string_cast.hpp>
#include <algorithm>

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
	faces(faces),
	vertices(vertices),
	normals(normals),
	model_name(model_name)
{ 
    // init matrices
    this->objectTransform = glm::mat4x4(1.0f);
    this->worldTransform = glm::mat4x4(1.0f);

    float x, y, z;

    float max = -1.0f * FLT_MAX;
    float min = FLT_MAX;

    
    for (int i = 0; i < vertices.size(); i++) {
        x = vertices[i].x;
        y = vertices[i].y;
        z = vertices[i].z;

        // get maximum
        if (x > maxCoordinates[0]) maxCoordinates[0] = x;
        if (y > maxCoordinates[1]) maxCoordinates[1] = y;
        if (z > maxCoordinates[2]) maxCoordinates[2] = z;
        
        Z_Max = glm::max(Z_Max, z);
        Z_Min = glm::max(Z_Min, z);



        // get minimum
        if (x < minCoordinates[0]) minCoordinates[0] = x;
        if (y < minCoordinates[1]) minCoordinates[1] = y;
        if (z < minCoordinates[2]) minCoordinates[2] = z;
        
    }

    boundingBox.push_back(glm::vec3(minCoordinates[0], minCoordinates[1], maxCoordinates[2]));
    boundingBox.push_back(glm::vec3(minCoordinates[0], maxCoordinates[1], maxCoordinates[2]));
    boundingBox.push_back(glm::vec3(maxCoordinates[0], maxCoordinates[1], maxCoordinates[2]));
    boundingBox.push_back(glm::vec3(maxCoordinates[0], minCoordinates[1], maxCoordinates[2]));

    boundingBox.push_back(glm::vec3(minCoordinates[0], minCoordinates[1], minCoordinates[2]));
    boundingBox.push_back(glm::vec3(minCoordinates[0], maxCoordinates[1], minCoordinates[2]));
    boundingBox.push_back(glm::vec3(maxCoordinates[0], maxCoordinates[1], minCoordinates[2]));
    boundingBox.push_back(glm::vec3(maxCoordinates[0], minCoordinates[1], minCoordinates[2]));

    // X axis
    axis.push_back(glm::vec3(0, 0, 0));
    axis.push_back(glm::vec3(maxCoordinates[0], 0, 0));

    // Y axis
    axis.push_back(glm::vec3(0, 0, 0));
    axis.push_back(glm::vec3(0, maxCoordinates[1], 0));

    // Z axis
    axis.push_back(glm::vec3(0, 0, 0));
    axis.push_back(glm::vec3(0, 0, maxCoordinates[2]));

    maxScale = maxCoordinates[0];
    if (maxScale < maxCoordinates[1]) maxScale = maxCoordinates[1];
    if (maxScale < maxCoordinates[2]) maxScale = maxCoordinates[2];

    verticesToNormals = std::vector<std::vector<int>>((vertices.size()));

    int facesCount = this->GetFacesCount();
    for (int j = 0; j < facesCount; j++) {
        Face face = GetFace(j);

        // VERTICES
        int v1Index = face.GetVertexIndex(0) - 1;
        int v2Index = face.GetVertexIndex(1) - 1;
        int v3Index = face.GetVertexIndex(2) - 1;

        int n1Index = face.GetNormalIndex(0) - 1;
        int n2Index = face.GetNormalIndex(1) - 1;
        int n3Index = face.GetNormalIndex(2) - 1;

        verticesToNormals[v1Index].push_back(n1Index);
        verticesToNormals[v2Index].push_back(n2Index);
        verticesToNormals[v3Index].push_back(n3Index);
    }
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

//glm::mat4x4& MeshModel::GetRotation()
//{
//    glm::mat4x4 Local = localRotateXMat * localRotateYMat * localRotateZMat;
//    glm::mat4x4 World = worldRotateXMat * worldRotateYMat * worldRotateZMat;
//
//    return Local * World;
//}

void MeshModel::GetTransform() {
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


    objectTransform = localTranslateMat * localRotateXMat * localRotateYMat * localRotateZMat * localScaleMat;
    worldTransform = worldTranslateMat * worldRotateXMat * worldRotateYMat * worldRotateZMat * worldScaleMat;
    this->objectTransform = worldTransform * objectTransform;
    this->axisTransformation = worldTranslateMat * worldScaleMat * localTranslateMat * localScaleMat;

}

std::vector<glm::vec3> MeshModel::Draw(glm::mat4x4 cameraTransform) {
    this->GetTransform();
    std::vector<glm::vec3> newVertices;
    glm::mat4x4 matrix = cameraTransform * this->objectTransform;
    maxZ = -1.0f * FLT_MAX;
    minZ = FLT_MAX;
    float maxX = -1.0f * FLT_MAX;
    float minX = FLT_MAX;
    float maxY = -1.0f * FLT_MAX;
    float minY = FLT_MAX;
    int verticesCount = this->vertices.size();
    for (int i = 0; i < verticesCount; i++) {
        glm::vec4 vector = matrix * glm::vec4(this->vertices.at(i), 1.0f);

        // cut the w coordinate
        newVertices.push_back(glm::vec3(vector.x / vector.w, vector.y / vector.w, vector.z / vector.w));
        minZ = std::min(minZ, (vector.z / vector.w));
        maxZ = std::max(maxZ, vector.z / vector.w);

        minX = std::min(minX, (vector.x / vector.w));
        maxX = std::max(maxX, vector.x / vector.w);

        minY = std::min(minY, (vector.y / vector.w));
        maxY = std::max(maxY, vector.y / vector.w);
    }

    int normalsCount = this->vertices.size();
    std::vector<glm::vec3> transformedNormals1;
    for (int i = 0; i < normalsCount; i++) {
        glm::vec4 vector = this->objectTransform * glm::vec4(this->normals.at(i), 1.0f);

        // cut the w coordinate
        transformedNormals1.push_back(glm::vec3(vector.x / vector.w, vector.y / vector.w, vector.z / vector.w));
    }
    transformedNormals = transformedNormals1;

    int facesCount = this->GetFacesCount();
    int counter = 0;

    std::vector<std::vector<glm::vec3>> vertexNormalsPairs;
    std::vector<std::vector<glm::vec3>> faceNormalsPairs;
    if (drawVertexNormals || drawFaceNormals) {
        for (int j = 0; j < facesCount; j++) {
            Face face = GetFace(j);

            // VERTICES
            int v1Index = face.GetVertexIndex(0) - 1;
            int v2Index = face.GetVertexIndex(1) - 1;
            int v3Index = face.GetVertexIndex(2) - 1;

            glm::vec3 v1 = newVertices.at(v1Index);
            glm::vec3 v2 = newVertices.at(v2Index);
            glm::vec3 v3 = newVertices.at(v3Index);

            if (drawVertexNormals) {
                // NORMALS
                int n1Index = face.GetNormalIndex(0) - 1;
                int n2Index = face.GetNormalIndex(1) - 1;
                int n3Index = face.GetNormalIndex(2) - 1;

                glm::vec3 n1 = this->normals.at(n1Index);
                glm::vec3 n2 = this->normals.at(n2Index);
                glm::vec3 n3 = this->normals.at(n3Index);

                std::vector<glm::vec3> pairN1;
                pairN1.push_back(v1);
                pairN1.push_back(v1 + (vertexNormalsScale * n1));

                std::vector<glm::vec3> pairN2;
                pairN2.push_back(v2);
                pairN2.push_back(v2 + (vertexNormalsScale * n2));

                std::vector<glm::vec3> pairN3;
                pairN3.push_back(v3);
                pairN3.push_back(v3 + (vertexNormalsScale * n3));

                vertexNormalsPairs.push_back(pairN1);
                vertexNormalsPairs.push_back(pairN2);
                vertexNormalsPairs.push_back(pairN3);

                this->transformedVertexNormals = vertexNormalsPairs;
            }

            if (drawFaceNormals) {
                glm::vec3 p1 = newVertices.at(v1Index);
                glm::vec3 p2 = newVertices.at(v2Index);
                glm::vec3 p3 = newVertices.at(v3Index);

                glm::vec3 U = p2 - p1;
                glm::vec3 V = p3 - p1;

                float x = (U.y * V.z) - (U.z * V.y);
                float y = (U.z * V.x) - (U.x * V.z);
                float z = (U.x * V.y) - (U.y * V.x);

                glm::vec3 normal = glm::vec3(x, y, z);

                glm::vec3 faceCenter = glm::vec3((v1.x + v2.x + v3.x) / 3, (v1.y + v2.y + v3.y) / 3, 1);

                std::vector<glm::vec3> pairFN;
                pairFN.push_back(faceCenter);
                pairFN.push_back(faceCenter + glm::vec3(-1 * faceNormalsScale * normal));

                faceNormalsPairs.push_back(pairFN);

                this->transformedFaceNormals = faceNormalsPairs;
            }
        }
    }



    if (drawBoundingBox) {
        int count = this->boundingBox.size();
        std::vector<glm::vec3> newTransformed;
        for (int i = 0; i < count; i++) {
            glm::vec4 vector = matrix * glm::vec4(this->boundingBox.at(i), 1.0f);

            // cut the w coordinate
            newTransformed.push_back(glm::vec3(vector.x / vector.w, vector.y / vector.w, 1));
            transformedBoundingBox = newTransformed;
        }
    }

    if (!this->LocalWorldEditObject) {
        matrix = cameraTransform * this->axisTransformation;
    }

    if (drawAxis) {
        int count = this->axis.size();
        std::vector<glm::vec3> newTransformed;
        for (int i = 0; i < count; i++) {
            glm::vec4 vector = matrix * glm::vec4(this->axis.at(i), 1.0f);

            // cut the w coordinate
            newTransformed.push_back(glm::vec3(vector.x / vector.w, vector.y / vector.w, 1));
            transformedAxis = newTransformed;
        }
    }

    return newVertices;
}

glm::vec2 MeshModel::GetVertixPoint(int i)
{
	return vertices[i];
}

//glm::vec3& MeshModel::GetVertex(int i, int j) {
//    return vertices[this->GetFace(i).GetVertexIndex(j) - float(1)];
//}