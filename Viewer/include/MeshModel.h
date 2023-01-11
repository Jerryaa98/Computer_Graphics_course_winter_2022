#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Face.h"
#include <math.h>

class MeshModel
{
    public:
	    MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name);
	    virtual ~MeshModel();
	    const Face& GetFace(int index) const;
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

        std::vector<glm::vec3> boundingBox;
        std::vector<glm::vec3> axis;

        std::vector<glm::vec3> transformedBoundingBox;
        std::vector<glm::vec3> transformedAxis;
        std::vector<std::vector<glm::vec3>> transformedVertexNormals;
        std::vector<std::vector<glm::vec3>> transformedFaceNormals;

        std::vector<Face> faces;
        std::vector<glm::vec3> vertices;

        glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
        //glm::vec3 diffuseColor = glm::vec3(95.0f / 256.0f, 59.0f / 256.0f, 59.0f / 256.0f);
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
};
