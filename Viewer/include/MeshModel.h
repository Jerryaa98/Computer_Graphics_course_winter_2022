#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>
#include "Face.h"
//#include <math.h>
#include <memory>
#include "MeshModel.h"

//class MeshModel

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoords;
};
   
class MeshModel
{
    protected:
        std::vector<Face> faces;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> textureCoords;
        std::vector<Vertex> modelVertices;
        glm::mat4x4 modelTransform;
        glm::mat4x4 worldTransform;
        std::string modelName;
        glm::vec3 color;
        GLuint vbo;
        GLuint vao;


    public:
        MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> textureCoords, const std::string& modelName = "");	    
        virtual ~MeshModel();
        const glm::mat4x4& GetWorldTransformation() const;
        const glm::mat4x4& GetModelTransformation() const;
        void SetWorldTransformation(const glm::mat4x4& worldTransform);
        void SetModelTransformation(const glm::mat4x4& modelTransform);
        const glm::vec3& GetColor() const;
        void SetColor(const glm::vec3& color);
        const std::string& GetModelName();
        const std::vector<Vertex>& GetModelVertices();
        void TranslateModel(const glm::vec3& translationVector);
        void TranslateWorld(const glm::vec3& translationVector);
        
        void RotateXModel(double angle);
        void RotateYModel(double angle);
        void RotateZModel(double angle);
        
        void ScaleXModel(double factor);
        void ScaleYModel(double factor);
        void ScaleZModel(double factor);
        void ScaleModel(double factor);
        
        void RotateXWorld(double angle);
        void RotateYWorld(double angle);
        void RotateZWorld(double angle);
        
        void ScaleXWorld(double factor);
        void ScaleYWorld(double factor);
        void ScaleZWorld(double factor);
        void ScaleWorld(double factor);
        
        GLuint GetVAO() const;
       
        glm::vec3 ambientColor = glm::vec3(0.2f, 0.7f, 1.0f);
        glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
        /*
        * const Face& GetFace(int index) const;
	    int GetFacesCount() const;
	    const std::string& GetModelName() const;
	    glm::vec2 MeshModel::GetVertixPoint(int i);

        void GetTransform();
        std::vector<glm::vec3> Draw(glm::mat4x4 transformMat);

        float localTranslateVector[3] = { 0, 0, 0 };
        float localRotateVector[3] = { 0, 180, 0 };
        float localScaleVector[3] = { 1, 1, 1 };
        bool uniformLocalScale = true;
        float localScale = 0.011;

        float worldTranslateVector[3] = { 0, 0, 0 };
        float worldRotateVector[3] = { 0, 0, 0 };
        float worldScaleVector[3] = { 1, 1, 1 };
        bool uniformWorldScale = true;
        float worldScale = 1;

        int maxScale = 0;

        float maxZ = -1.0f * FLT_MAX;
        float minZ = FLT_MAX;

        bool drawAxis = false;
        bool drawBoundingBox = false;
        bool drawVertexNormals = false;
        bool drawFaceNormals = false;

        float vertexNormalsScale = 0.05;
        float faceNormalsScale = 0.001;

        int LocalWorldEditObject = 1;

        bool trianglesBoundingBoxes = false;
        bool coloredTriangles = true;
        bool trianglesOutlines = false;

        float maxX = -1.0f * FLT_MAX;
        float minX = FLT_MAX;

        float maxY = -1.0f * FLT_MAX;
        float minY = FLT_MAX;


        std::vector<glm::vec3> boundingBox;
        std::vector<glm::vec3> axis;

        std::vector<glm::vec3> transformedBoundingBox;
        std::vector<glm::vec3> transformedAxis;
        std::vector<std::vector<glm::vec3>> transformedVertexNormals;
        std::vector<std::vector<glm::vec3>> transformedFaceNormals;

        std::vector<Face> faces;
        std::vector<glm::vec3> vertices;

        glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);

        glm::mat4x4 objectTransform;
        glm::mat4x4 objectTransformation = glm::mat4x4(1.0f);

        glm::vec3 color = glm::vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);

        std::vector<std::vector<int>> verticesToNormals;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> transformedNormals;

        float Z_Min =  static_cast<float>(INFINITY);
        float Z_Max = -1 * static_cast<float>(INFINITY);


        float maxCoordinates[3] = { -1.0f * FLT_MAX, -1.0f * FLT_MAX, -1.0f * FLT_MAX };
        float minCoordinates[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
    private:
        // local matrices (init with identity matrix)
        glm::mat4x4 localTranslateMat = glm::mat4x4(1.0f);
        glm::mat4x4 localRotateXMat = glm::mat4x4(1.0f);
        glm::mat4x4 localRotateYMat = glm::mat4x4(1.0f);
        glm::mat4x4 localRotateZMat = glm::mat4x4(1.0f);
        glm::mat4x4 localScaleMat = glm::mat4x4(1.0f);

        // world matrices (init with identity matrix)
        glm::mat4x4 worldTranslateMat = glm::mat4x4(1.0f);
        glm::mat4x4 worldRotateXMat = glm::mat4x4(1.0f);
        glm::mat4x4 worldRotateYMat = glm::mat4x4(1.0f);
        glm::mat4x4 worldRotateZMat = glm::mat4x4(1.0f);
        glm::mat4x4 worldScaleMat = glm::mat4x4(1.0f);
        glm::mat4x4 axisTransformation = glm::mat4x4(1.0f);

	    std::string model_name;
        glm::mat4x4 localTransform;
        glm::mat4x4 worldTransform;
        */
};
