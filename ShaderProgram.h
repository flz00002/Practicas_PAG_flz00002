//
// Created by felip on 05/10/2026.
//

#ifndef PRACTICA_1_PAG_SHADERPROGRAM_H
#define PRACTICA_1_PAG_SHADERPROGRAM_H

#include <glad/glad.h>
#include <string>
#include <stdexcept>

namespace PAG {
    class ShaderProgram {
    private:
        GLuint idSP = 0; ///< Identificador del Shader Program de OpenGL

        std::string cargarArchivoTexto(const std::string& ruta);

    public:
        ShaderProgram();
        ~ShaderProgram();

        void compilarYEnlazar(const std::string& nombreBase);
        void activar() const;
        void purgar();

        GLuint getId() const { return idSP; }
    };
}

#endif // PRACTICA_1_PAG_SHADERPROGRAM_H
