#version 330 core
layout(location = 0) in vec3 objectPos;

uniform mat4 depthMVP;
uniform mat4 modelMatrix;

void main() {
    gl_Position = depthMVP * modelMatrix * vec4(objectPos, 1.0);
}
