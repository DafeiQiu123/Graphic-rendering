#version 330 core
layout(location = 0) in vec3 objectPos;

uniform mat4 depthMVP;

void main() {
    gl_Position = depthMVP * vec4(objectPos, 1.0);
}
