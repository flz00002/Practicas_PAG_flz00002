//
// Created by felip on 24/10/2026.
//

#ifndef PRACTICA_1_PAG_MODELO_H
#define PRACTICA_1_PAG_MODELO_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

namespace PAG {
    /**
     * @brief Clase desacoplada encargada de gestionar el VAO, VBO, IBO y las
     * transformaciones de modelado de un objeto 3D.
     */
    class Modelo {
    private:
        GLuint _idVAO = 0; ///< Identificador del Vertex Array Object
        GLuint _idVBO = 0; ///< Identificador del Vertex Buffer Object
        GLuint _idIBO = 0; ///< Identificador del Index Buffer Object
        GLsizei _numIndices = 0; ///< Número total de índices para el renderizado

        std::string _nombre; ///< Nombre descriptivo del modelo

        // Atributos de transformación de modelado
        glm::vec3 _posicion = glm::vec3(0.0f); ///< Posición (traslación)
        glm::vec3 _rotacion = glm::vec3(0.0f); ///< Rotación en grados (ejes X, Y, Z)
        glm::vec3 _escala   = glm::vec3(1.0f); ///< Escala en los ejes X, Y, Z

        void purgar();

    public:
        Modelo(const std::string& nombre = "Modelo");
        ~Modelo();

        void creaTrianguloProcedural();
        void cargarOBJ(const std::string& rutaArchivo);
        void dibujar() const;

        glm::mat4 getMatrizModelado() const;

        // Getters y Setters
        const std::string& getNombre() const { return _nombre; }
        void setNombre(const std::string& nombre) { _nombre = nombre; }

        glm::vec3 getPosicion() const { return _posicion; }
        void setPosicion(const glm::vec3& pos) { _posicion = pos; }

        glm::vec3 getRotacion() const { return _rotacion; }
        void setRotacion(const glm::vec3& rot) { _rotacion = rot; }

        glm::vec3 getEscala() const { return _escala; }
        void setEscala(const glm::vec3& esc) { _escala = esc; }
    };
}

#endif // PRACTICA_1_PAG_MODELO_H
