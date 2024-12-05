#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

class Mesh {
public:
    Mesh();
    ~Mesh();
    bool loadOBJ(const std::string &filepath);
    glm::vec2 parseFace(const std::string &vertexStr);

    const std::vector<float>& getVertexData() const;
private:
    std::vector<float> m_vertexData; // Interleaved positions and normals
    void insertVec3(std::vector<float> &data, glm::vec3 v);
};
