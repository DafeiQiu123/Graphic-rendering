#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

bool Mesh::loadOBJ(const std::string &filepath)
{
    m_vertexData.clear();
    std::ifstream objFile(filepath);
    if (!objFile.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filepath << std::endl;
        return false;
    }
    std::cout << "Storing the mesh obj" << filepath << " into vertexdata" << std::endl;

    std::vector<glm::vec3> temp_vertexs;
    std::vector<glm::vec3> temp_normals;
    std::vector<std::vector<std::string>> temp_face;

    std::string line;
    while (std::getline(objFile, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        if (prefix == "v") {
            glm::vec3 position;
            iss >> position.x >> position.y >> position.z;
            temp_vertexs.push_back(position);
        } else if (prefix == "f") {
            std::string s0,s1,s2;
            iss >> s0 >> s1 >> s2;
            std::vector<std::string> oneFace{s0,s1,s2};
            temp_face.push_back(oneFace);
        } else if (prefix == "vn"){
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
    }
    objFile.close();
    std::cout << temp_face[0][0] << " " << temp_face[0][1] << " " << temp_face[0][2];
    int count = 0;
    // Compute normals
    for (int i = 0; i < temp_face.size(); i++){
        glm::vec2 i0 = parseFace(temp_face[i][0]);
        glm::vec2 i1 = parseFace(temp_face[i][1]);
        glm::vec2 i2 = parseFace(temp_face[i][2]);

        glm::vec3 v0 = temp_vertexs[i0[0]];
        glm::vec3 v1 = temp_vertexs[i1[0]];
        glm::vec3 v2 = temp_vertexs[i2[0]];
        glm::vec3 normal_defaulf = glm::normalize(glm::cross(v1-v0,v2-v0));
        // glm::vec3 n0 = (i0[1] == -1) ? normal_defaulf : glm::normalize(temp_normals[i0[1]]);
        // glm::vec3 n1 = (i1[1] == -1) ? normal_defaulf : glm::normalize(temp_normals[i1[1]]);
        // glm::vec3 n2 = (i2[1] == -1) ? normal_defaulf : glm::normalize(temp_normals[i2[1]]);
        glm::vec3 n0 = normal_defaulf;
        glm::vec3 n1 = normal_defaulf;
        glm::vec3 n2 = normal_defaulf;
        insertVec3(m_vertexData,v0);
        insertVec3(m_vertexData,n0);
        m_vertexData.push_back(0);
        m_vertexData.push_back(1);
        insertVec3(m_vertexData,v1);
        insertVec3(m_vertexData,n1);
        m_vertexData.push_back(0);
        m_vertexData.push_back(0);
        insertVec3(m_vertexData,v2);
        insertVec3(m_vertexData,n2);
        m_vertexData.push_back(1);
        m_vertexData.push_back(0);
        count ++;
    }

    return true;
}

glm::vec2 Mesh::parseFace(const std::string &vertexStr){
    size_t pos = 0;
    size_t firstSlashPos = vertexStr.find('/', pos);
    size_t secondSlashPos = vertexStr.find('/', firstSlashPos + 1);
    glm::vec2 result(-1,-1);
    if (firstSlashPos != std::string::npos && secondSlashPos != std::string::npos) {
        std::string vIndexStr = vertexStr.substr(pos, firstSlashPos - pos);
        std::string vnIndexStr = vertexStr.substr(secondSlashPos + 1);
        result[0] = std::stoi(vIndexStr) - 1;
        result[1] = std::stoi(vnIndexStr) - 1;
    } else {
        result[0] = std::stoi(vertexStr) - 1;
        result[1] = -1;
    }
    return result;
}

const std::vector<float>& Mesh::getVertexData() const
{
    return m_vertexData;
}

void Mesh::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
