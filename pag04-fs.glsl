#version 410 core

out vec4 colorFragmento;

in salidaVS{
    vec3 posicionV;
    vec3 normalV;
} entrada;

uniform vec3 uKd;

subroutine vec4 fCalcularColor();

subroutine uniform fCalcularColor uMetodoColorElegido;

subroutine (fCalcularColor)
vec4 colorRojo(){
    return vec4(1.0, 0.0, 0.0, 1.0);
}

subroutine (fCalcularColor)
vec4 colorSolido(){
    return vec4(uKd, 1.0);
}

void main(){
    colorFragmento = uMetodoColorElegido();
}