#version 330 core

layout(location = 0) in vec3 objectPos;
layout(location = 1) in vec3 objectNormal;
layout(location = 2) in vec2 texCoord;

out vec3 worldPos;
out vec3 worldNormal;
out vec2 fragTexCoord;
out vec4 FragPosLightSpace[8];

uniform mat4 modelMatrix;
uniform mat4 inverseModelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 lightSpaceMatrices[8];
uniform int numLightsV;

void main() {
    worldPos = vec3(modelMatrix * vec4(objectPos, 1.0f));
    worldNormal = normalize(vec3(transpose(inverseModelMatrix) * vec4(objectNormal,0.0)));
    fragTexCoord = texCoord;
    for (int i = 0; i < numLightsV; ++i) {
        FragPosLightSpace[i] = lightSpaceMatrices[i] * modelMatrix * vec4(objectPos, 1.0f);
    }
    gl_Position = projMatrix * viewMatrix * vec4(worldPos, 1.0f);
}
