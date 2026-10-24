//
// Created by felip on 24/10/2026.
//

#include "Modelo.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>
#include <iostream>

namespace PAG {

    Modelo::Modelo(const std::string& nombre) : _nombre(nombre) {}

    Modelo::~Modelo() {
        purgar();
    }

    void Modelo::purgar() {
        if (_idIBO != 0) { glDeleteBuffers(1, &_idIBO); _idIBO = 0; }
        if (_idVBO != 0) { glDeleteBuffers(1, &_idVBO); _idVBO = 0; }
        if (_idVAO != 0) { glDeleteVertexArrays(1, &_idVAO); _idVAO = 0; }
        _numIndices = 0;
    }

    void Modelo::creaTrianguloProcedural() {
        purgar();

        GLfloat vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };
        GLuint indices[] = { 0, 1, 2 };
        _numIndices = 3;

        glGenVertexArrays(1, &_idVAO);
        glBindVertexArray(_idVAO);

        glGenBuffers(1, &_idVBO);
        glBindBuffer(GL_ARRAY_BUFFER, _idVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &_idIBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _idIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void Modelo::cargarOBJ(const std::string& rutaArchivo) {
        purgar();

        Assimp::Importer importador;
        const aiScene* escena = importador.ReadFile(rutaArchivo,
            aiProcess_JoinIdenticalVertices |
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals);

        if (!escena || escena->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !escena->mRootNode) {
            throw std::runtime_error("Error al cargar archivo OBJ (" + rutaArchivo + "): " + importador.GetErrorString());
        }

        std::vector<GLfloat> vboData;
        std::vector<GLuint> indicesData;

        GLuint numVerticesAcumulados = 0;

        // Recorremos todas las mallas contenidas en la escena importada
        for (unsigned int m = 0; m < escena->mNumMeshes; ++m) {
            aiMesh* malla = escena->mMeshes[m];

            for (unsigned int i = 0; i < malla->mNumVertices; ++i) {
                vboData.push_back(malla->mVertices[i].x);
                vboData.push_back(malla->mVertices[i].y);
                vboData.push_back(malla->mVertices[i].z);
            }

            for (unsigned int f = 0; f < malla->mNumFaces; ++f) {
                aiFace cara = malla->mFaces[f];
                for (unsigned int j = 0; j < cara.mNumIndices; ++j) {
                    indicesData.push_back(numVerticesAcumulados + cara.mIndices[j]);
                }
            }
            numVerticesAcumulados += malla->mNumVertices;
        }

        _numIndices = static_cast<GLsizei>(indicesData.size());

        glGenVertexArrays(1, &_idVAO);
        glBindVertexArray(_idVAO);

        glGenBuffers(1, &_idVBO);
        glBindBuffer(GL_ARRAY_BUFFER, _idVBO);
        glBufferData(GL_ARRAY_BUFFER, vboData.size() * sizeof(GLfloat), vboData.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &_idIBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _idIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesData.size() * sizeof(GLuint), indicesData.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    glm::mat4 Modelo::getMatrizModelado() const {
        glm::mat4 m(1.0f);
        m = glm::translate(m, _posicion);
        m = glm::rotate(m, glm::radians(_rotacion.x), glm::vec3(1.0f, 0.0f, 0.0f));
        m = glm::rotate(m, glm::radians(_rotacion.y), glm::vec3(0.0f, 1.0f, 0.0f));
        m = glm::rotate(m, glm::radians(_rotacion.z), glm::vec3(0.0f, 0.0f, 1.0f));
        m = glm::scale(m, _escala);
        return m;
    }

    void Modelo::dibujar() const {
        if (_idVAO != 0 && _numIndices > 0) {
            glBindVertexArray(_idVAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _idIBO);
            glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    }
}
