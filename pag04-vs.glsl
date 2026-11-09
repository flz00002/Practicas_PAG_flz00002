#version 410
layout (location = 0) in vec3 posicion;

uniform mat4 mModelViewProj

out salidaVS {
    vec3 posicionV;
    vec3 normalV;
} salida;

void main () {
    salida.posicionV = posicion;
    salida.normalV   = vec3(0.0, 0.0, 1.0); // Normal por defecto por el momento
    gl_Position      = mModelViewProj * vec4(posicion, 1.0);
}