#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;         // Keep these to match your VAO layout
layout (location = 2) in vec3 normal;     // even if we don't use them

uniform mat4 depthMVP;    // Light's view-projection matrix
uniform mat4 modelMatrix; // Model matrix for the current object

void main() {
    // Transform vertex position to light's perspective
    gl_Position = depthMVP * modelMatrix * vec4(position, 1.0);
}
