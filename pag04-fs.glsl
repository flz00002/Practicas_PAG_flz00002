#version 410
out vec4 colorFragmento;

// Bloque de interfaz de entrada desde el Vertex Shader
in salidaVS {
    vec3 posicionV;
    vec3 normalV;
} entrada;

// Uniform para el color difuso del material del modelo
uniform vec3 uKd;

// Firma de la subrutina
subroutine vec4 fCalcularColor();

// Uniform asociado a la subrutina activa
subroutine uniform fCalcularColor uMetodoColorElegido;

// Implementación para modo alambre (color fijo rojo)
subroutine (fCalcularColor)
vec4 colorRojo () {
    return vec4 (1.0, 0.0, 0.0, 1.0);
}

// Implementación para modo sólido (utiliza el color difuso del material)
subroutine (fCalcularColor)
vec4 colorSolido () {
    return vec4 (uKd, 1.0);
}

void main () {
    // Evaluación de la subrutina elegida desde C++
    colorFragmento = uMetodoColorElegido();
}