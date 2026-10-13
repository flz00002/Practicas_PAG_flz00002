#version 410
layout (location = 0) in vec3 posicion;

uniform mat4 mModelViewProj;

void main () {
    gl_Position = mModelViewProj * vec4(posicion, 1.0);
}