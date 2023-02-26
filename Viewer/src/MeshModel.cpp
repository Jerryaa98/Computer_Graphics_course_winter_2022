#include "MeshModel.h"
#include <iostream>
#include "Utils.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <glm/gtc/matrix_transform.hpp>


/*MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
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
*/

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> textureCoords, const std::string& modelName) :
    modelTransform(1),
    worldTransform(1),
    modelName(modelName)
{
    ambientColor = glm::vec3(((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)));
    diffuseColor = glm::vec3(((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)));
    specularColor = glm::vec3(((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)));

    worldTransform = glm::mat4x4(1);
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0, 1);
    color = glm::vec3(dist(mt), dist(mt), dist(mt));

    modelVertices.reserve(3 * faces.size());
    for (int i = 0; i < faces.size(); i++)
    {
        Face currentFace = faces.at(i);
        for (int j = 0; j < 3; j++)
        {
            int vertexIndex = currentFace.GetVertexIndex(j) - 1;

            Vertex vertex;
            vertex.position = vertices[vertexIndex];
            vertex.normal = normals[vertexIndex];

            if (textureCoords.size() > 0)
            {
                int textureCoordsIndex = currentFace.GetTextureIndex(j) - 1;
                vertex.textureCoords = textureCoords[textureCoordsIndex];
            }
            modelVertices.push_back(vertex);
        }
    }
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(Vertex), &modelVertices[0], GL_STATIC_DRAW);

    // Vertex Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Normals attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    // Vertex Texture Coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // unbind to make sure other code does not change it somewhere else
    glBindVertexArray(0);
}
    
    
MeshModel::~MeshModel(){
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
}

const glm::vec3& MeshModel::GetColor() const {
    return color;
}

void MeshModel::SetColor(const glm::vec3& color) {
    this->color = color;
}
const std::string& MeshModel::GetModelName()
{
    return modelName;
}

void MeshModel::SetWorldTransformation(const glm::mat4x4& worldTransform)
{
    this->worldTransform = worldTransform;
}

const glm::mat4x4& MeshModel::GetWorldTransformation() const
{
    return worldTransform;
}

void MeshModel::SetModelTransformation(const glm::mat4x4& worldTransform)
{
    this->modelTransform = worldTransform;
}

const glm::mat4x4& MeshModel::GetModelTransformation() const
{
    return modelTransform;
}

void MeshModel::TranslateModel(const glm::vec3& translationVector)
{
    modelTransform = Utils::TranslationMatrix(translationVector) * modelTransform;
}

void MeshModel::TranslateWorld(const glm::vec3& translationVector)
{
    worldTransform = Utils::TranslationMatrix(translationVector) * worldTransform;
}
void MeshModel::RotateXModel(double angle)
{
    modelTransform = Utils::XRotationMatrix(angle) * modelTransform;
}
void MeshModel::RotateYModel(double angle)
{
    modelTransform = Utils::YRotationMatrix(angle) * modelTransform;
}
void MeshModel::RotateZModel(double angle)
{
    modelTransform = Utils::ZRotationMatrix(angle) * modelTransform;
}

void MeshModel::ScaleXModel(double factor)
{
    modelTransform = Utils::XScalingMatrix(factor) * modelTransform;
}

void MeshModel::ScaleYModel(double factor)
{
    modelTransform = Utils::YScalingMatrix(factor) * modelTransform;
}

void MeshModel::ScaleZModel(double factor)
{
    modelTransform = Utils::ZScalingMatrix(factor) * modelTransform;
}
void MeshModel::ScaleModel(double factor)
{
    modelTransform = glm::scale(glm::mat4(1), glm::vec3(factor, factor, factor)) * modelTransform;
}
void MeshModel::RotateXWorld(double angle)
{
    worldTransform = Utils::XRotationMatrix(angle) * worldTransform;
}
void MeshModel::RotateYWorld(double angle)
{
    worldTransform = Utils::YRotationMatrix(angle) * worldTransform;
}
void MeshModel::RotateZWorld(double angle)
{
    worldTransform = Utils::ZRotationMatrix(angle) * worldTransform;
}
void MeshModel::ScaleXWorld(double factor)
{
    worldTransform = Utils::XScalingMatrix(factor) * worldTransform;
}

void MeshModel::ScaleYWorld(double factor)
{
    worldTransform = Utils::YScalingMatrix(factor) * worldTransform;
}
void MeshModel::ScaleZWorld(double factor)
{
    worldTransform = Utils::ZScalingMatrix(factor) * worldTransform;
}
void MeshModel::ScaleWorld(double factor)
{
    worldTransform = glm::scale(glm::mat4(1), glm::vec3(factor, factor, factor)) * worldTransform;
}
GLuint MeshModel::GetVAO() const
{
    return vao;
}
const std::vector<Vertex>& MeshModel::GetModelVertices()
{
    return modelVertices;
}