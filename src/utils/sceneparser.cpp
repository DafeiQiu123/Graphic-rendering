#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>
#include <QImage>
#include <chrono>
#include <iostream>

void traverseSceneGraph(SceneNode* node, glm::mat4 parentCTM, RenderData& renderData){
    glm::mat4 currentCTM = parentCTM;
    for (SceneTransformation* transformation : node->transformations) {
        switch (transformation->type) {
        case TransformationType::TRANSFORMATION_TRANSLATE:
            currentCTM = currentCTM * glm::translate(glm::mat4(1.0f), transformation->translate);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            currentCTM = currentCTM * glm::rotate(glm::mat4(1.0f), transformation->angle, transformation->rotate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            currentCTM = currentCTM * glm::scale(glm::mat4(1.0f), transformation->scale);
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            currentCTM = currentCTM * transformation->matrix;
            break;
        }
    }
    for(ScenePrimitive* primitive: node->primitives){
        RenderShapeData shapeData;
        shapeData.primitive = *primitive;
        if (primitive->material.textureMap.isUsed) {
            QString filepath = QString(primitive->material.textureMap.filename.c_str());
            std::cout << "update texture " << filepath.toStdString() << std::endl;
            shapeData.primitive.material.textureMap.textureImage = QImage(filepath).convertToFormat(QImage::Format_RGBA8888).mirrored();
        }
        shapeData.inverseCTM = glm::inverse(currentCTM);
        shapeData.ctm = currentCTM;
        renderData.shapes.push_back(shapeData);
    }
    for(SceneLight* light: node->lights){
        SceneLightData lightData;
        lightData.id = light->id;
        lightData.type = light->type;
        lightData.color = light->color;
        lightData.function = light->function;
        switch(light->type){
        case LightType::LIGHT_POINT:
            lightData.pos = currentCTM * glm::vec4(0.0f,0.0f,0.0f,1.0f);
            break;
        case LightType::LIGHT_DIRECTIONAL:
            lightData.dir = glm::normalize(currentCTM * (light->dir));
            break;
        case LightType::LIGHT_SPOT:
            lightData.pos = currentCTM * glm::vec4(0.0f,0.0f,0.0f,1.0f);
            lightData.dir = glm::normalize(currentCTM * (light->dir));
            lightData.penumbra = light->penumbra;
            lightData.angle = light->angle;
            break;
        }
        renderData.lights.push_back(lightData);
    }
    for (SceneNode* child : node->children) {
        traverseSceneGraph(child, currentCTM, renderData);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // TODO: Use your Lab 5 code here
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();
    SceneNode *root = fileReader.getRootNode();
    renderData.shapes.clear();
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    traverseSceneGraph(root, identityMatrix, renderData);
    return true;
}
