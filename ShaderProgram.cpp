//
// Created by felip on 05/10/2026.
//

#include "ShaderProgram.h"
#include <fstream>
#include <sstream>

namespace PAG {

    ShaderProgram::ShaderProgram() : idSP(0) {}

    ShaderProgram::~ShaderProgram() {
        purgar();
    }

    void ShaderProgram::purgar() {
        if (idSP != 0) {
            glDeleteProgram(idSP);
            idSP = 0;
        }
    }

    std::string ShaderProgram::cargarArchivoTexto(const std::string& ruta) {
        std::ifstream archivo(ruta);
        if (!archivo.is_open()) {
            throw std::runtime_error("No se pudo abrir el archivo de shader: " + ruta);
        }
        std::stringstream buffer;
        buffer << archivo.rdbuf();
        return buffer.str();
    }

    void ShaderProgram::compilarYEnlazar(const std::string& nombreBase) {
        std::string rutaVS = nombreBase + "-vs.glsl";
        std::string rutaFS = nombreBase + "-fs.glsl";

        std::string codigoVS = cargarArchivoTexto(rutaVS);
        std::string codigoFS = cargarArchivoTexto(rutaFS);

        GLint exito = 0;
        GLchar infoLog[1024];

        GLuint idVS = glCreateShader(GL_VERTEX_SHADER);
        const GLchar* fuenteVS = codigoVS.c_str();
        glShaderSource(idVS, 1, &fuenteVS, nullptr);
        glCompileShader(idVS);

        glGetShaderiv(idVS, GL_COMPILE_STATUS, &exito);
        if (!exito) {
            glGetShaderInfoLog(idVS, 1024, nullptr, infoLog);
            glDeleteShader(idVS);
            throw std::runtime_error("Error Vertex Shader (" + rutaVS + "):\n" + std::string(infoLog));
        }

        GLuint idFS = glCreateShader(GL_FRAGMENT_SHADER);
        const GLchar* fuenteFS = codigoFS.c_str();
        glShaderSource(idFS, 1, &fuenteFS, nullptr);
        glCompileShader(idFS);

        glGetShaderiv(idFS, GL_COMPILE_STATUS, &exito);
        if (!exito) {
            glGetShaderInfoLog(idFS, 1024, nullptr, infoLog);
            glDeleteShader(idVS);
            glDeleteShader(idFS);
            throw std::runtime_error("Error Fragment Shader (" + rutaFS + "):\n" + std::string(infoLog));
        }

        GLuint nuevoSP = glCreateProgram();
        glAttachShader(nuevoSP, idVS);
        glAttachShader(nuevoSP, idFS);
        glLinkProgram(nuevoSP);

        glGetProgramiv(nuevoSP, GL_LINK_STATUS, &exito);
        if (!exito) {
            glGetProgramInfoLog(nuevoSP, 1024, nullptr, infoLog);
            glDetachShader(nuevoSP, idVS);
            glDetachShader(nuevoSP, idFS);
            glDeleteShader(idVS);
            glDeleteShader(idFS);
            glDeleteProgram(nuevoSP);
            throw std::runtime_error("Error al enlazar Shader Program:\n" + std::string(infoLog));
        }

        glDetachShader(nuevoSP, idVS);
        glDetachShader(nuevoSP, idFS);
        glDeleteShader(idVS);
        glDeleteShader(idFS);

        // Liberar el programa anterior si existía uno compilado
        purgar();
        idSP = nuevoSP;
    }

    void ShaderProgram::activar() const {
        if (idSP != 0) {
            glUseProgram(idSP);
        }
    }
}
