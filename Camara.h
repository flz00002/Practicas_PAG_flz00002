//
// Created by felip on 13/10/2026.
//

#ifndef PRACTICA_1_PAG_CAMARA_H
#define PRACTICA_1_PAG_CAMARA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace PAG {
    // Enum para los distintos tipos de movimiento de la cámara
    enum class TipoMovimiento {
        PAN,
        TILT,
        DOLLY,
        CRANE,
        ORBIT,
        ZOOM
    };

    class Camara {
    private:
        glm::vec3 _posicion = glm::vec3(0.0f, 0.0f, 3.0f); ///< Posición de la cámara en el espacio del mundo
        glm::vec3 _lookAt   = glm::vec3(0.0f, 0.0f, 0.0f); ///< Punto al que mira la cámara
        glm::vec3 _up       = glm::vec3(0.0f, 1.0f, 0.0f); ///< Vector Up global

        float _fovy   = 45.0f;           ///< Ángulo de visión vertical en grados
        float _aspecto = 1024.0f / 576.0f; ///< Relación de aspecto (ancho / alto)
        float _zNear  = 0.1f;            ///< Plano cercano
        float _zFar   = 100.0f;          ///< Plano lejano

    public:
        Camara() = default;
        ~Camara() = default;

        // Métodos de consulta de matrices
        glm::mat4 getMatrizVision() const;
        glm::mat4 getMatrizProyeccion() const;

        // Modificadores de parámetros de cámara
        void setViewportRatio(float aspecto);
        float getFovy() const { return _fovy; }
        void setFovy(float fovy);

        // Métodos para aplicar movimientos a la cámara
        void pan(float deltaX);
        void tilt(float deltaY);
        void dolly(float deltaX, float deltaZ);
        void crane(float deltaY);
        void orbit(float deltaLat, float deltaLon);
        void zoom(float deltaFov);
    };
}


#endif //PRACTICA_1_PAG_CAMARA_H
