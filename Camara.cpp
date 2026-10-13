//
// Created by felip on 13/10/2026.
//

#include "Camara.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

namespace PAG {

    glm::mat4 Camara::getMatrizVision() const {
        return glm::lookAt(_posicion, _lookAt, _up); // Genera la matriz de vista
    }

    glm::mat4 Camara::getMatrizProyeccion() const {
        return glm::perspective(glm::radians(_fovy), _aspecto, _zNear, _zFar); // Genera la matriz de proyección
    }

    void Camara::setViewportRatio(float aspecto) {
        _aspecto = aspecto;
    }

    void Camara::setFovy(float fovy) {
        _fovy = glm::clamp(fovy, 1.0f, 90.0f); // Restringe el campo de visión
    }

    void Camara::pan(float deltaX) {
        // Rotación de la dirección de visión alrededor del eje v (Up)
        glm::vec3 dir = _lookAt - _posicion;
        dir = glm::rotate(dir, glm::radians(-deltaX), _up);
        _lookAt = _posicion + dir;
    }

    void Camara::tilt(float deltaY) {
        // Rotación alrededor del eje u local (perpendicular a dir y up)
        glm::vec3 dir = _lookAt - _posicion;
        glm::vec3 u = glm::normalize(glm::cross(dir, _up));
        dir = glm::rotate(dir, glm::radians(-deltaY), u);
        _lookAt = _posicion + dir;
    }

    void Camara::dolly(float deltaX, float deltaZ) {
        // Traslación sobre el plano local de la cámara
        glm::vec3 n = glm::normalize(_posicion - _lookAt);
        glm::vec3 u = glm::normalize(glm::cross(_up, n));
        glm::vec3 desplazamiento = u * deltaX + n * deltaZ;
        _posicion += desplazamiento;
        _lookAt += desplazamiento;
    }

    void Camara::crane(float deltaY) {
        // Traslación vertical pura en el eje Y del mundo
        glm::vec3 desplazamiento = _up * deltaY;
        _posicion += desplazamiento;
        _lookAt += desplazamiento;
    }

    void Camara::orbit(float deltaLat, float deltaLon) {
        // Rotación en latitud y longitud de la posición respecto al punto lookAt
        glm::vec3 posRel = _posicion - _lookAt;
        glm::vec3 u = glm::normalize(glm::cross(posRel, _up));

        posRel = glm::rotate(posRel, glm::radians(-deltaLon), _up);
        posRel = glm::rotate(posRel, glm::radians(-deltaLat), u);

        _posicion = _lookAt + posRel;
    }

    void Camara::zoom(float deltaFov) {
        setFovy(_fovy - deltaFov);
    }
}
