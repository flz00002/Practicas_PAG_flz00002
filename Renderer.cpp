#include "glad/glad.h"
#include <GL/gl.h>
#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace PAG
{
    Renderer* Renderer::instancia = nullptr;
    /**
    * Constructor por defecto
    */
    Renderer::Renderer (){ }
    /**
    * Destructor
    */
    Renderer::~Renderer () {
        if ( idVS != 0 )
        { glDeleteShader ( idVS );
        }
        if ( idFS != 0 )
        { glDeleteShader ( idFS );
        }
        if ( idSP != 0 )
        { glDeleteProgram ( idSP );
        }
        if ( idVBO != 0 )
        { glDeleteBuffers ( 1, &idVBO );
        }
        if ( idIBO != 0 )
        { glDeleteBuffers ( 1, &idIBO );
        }
        if ( idVAO != 0 )
        { glDeleteVertexArrays ( 1, &idVAO );
        }
    }
    /**
    * Consulta del objeto único de la clase
    * @return La dirección de memoria del objeto
    */
    Renderer& Renderer::getInstancia ()
    {
        if (!instancia){
            instancia = new Renderer;
    }
        return *instancia;
    }

    std::string Renderer::cargarArchivoTexto(const std::string& ruta) {
        std::ifstream archivo(ruta);
        if (!archivo.is_open()) {
            throw std::runtime_error("No se pudo abrir el archivo de shader: " + ruta);
        }
        std::stringstream buffer;
        buffer << archivo.rdbuf();
        return buffer.str();
    }

    /**
    * Método para hacer el refresco de la escena
    */
    void Renderer::refrescar (){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
        glUseProgram ( idSP );
        glBindVertexArray ( idVAO );
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, idIBO );
        glDrawElements ( GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr );
    }

    void Renderer::setViewport (int width, int height) {
        glViewport(0, 0, width, height);
    }

    float Renderer::getbgRed() {
        return _colorBorrado[0];
    }

    float Renderer::getbgGreen() {
        return _colorBorrado[1];
    }

    float Renderer::getbgBlue() {
        return _colorBorrado[2];
    }

    void Renderer::setbgRed (float bgRed) {
        _colorBorrado[0] = bgRed;
    }
    void Renderer::setbgGreen (float bgGreen) {
        _colorBorrado[1] = bgGreen;
    }
    void Renderer::setbgBlue (float bgBlue) {
        _colorBorrado[2] = bgBlue;
    }

    void Renderer::colorear () {
        glClearColor(_colorBorrado[0],_colorBorrado[1],_colorBorrado[2],_colorBorrado[3]);
    }

    void Renderer::activarTestProfundidad() {
        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::informePropiedades() {
        std::cout << glGetString ( GL_RENDERER ) << std::endl
    << glGetString ( GL_VENDOR ) << std::endl
    << glGetString ( GL_VERSION ) << std::endl
    << glGetString ( GL_SHADING_LANGUAGE_VERSION ) << std::endl;
    }

    /**
    * Método para crear, compilar y enlazar el shader program
    * @note No se incluye ninguna comprobación de errores
    */
    void PAG::Renderer::creaShaderProgram(const std::string& nombreBase) {
        std::string rutaVS = nombreBase + "-vs.glsl";
        std::string rutaFS = nombreBase + "-fs.glsl";

        std::string codigoVS = cargarArchivoTexto(rutaVS);
        std::string codigoFS = cargarArchivoTexto(rutaFS);

        GLint exito = 0;
        GLchar infoLog[1024];

        idVS = glCreateShader(GL_VERTEX_SHADER);
        const GLchar* fuenteVS = codigoVS.c_str();
        glShaderSource(idVS, 1, &fuenteVS, nullptr);
        glCompileShader(idVS);

        glGetShaderiv(idVS, GL_COMPILE_STATUS, &exito);
        if (!exito) {
            glGetShaderInfoLog(idVS, 1024, nullptr, infoLog);
            throw std::runtime_error("Error Vertex Shader (" + rutaVS + "):\n" + std::string(infoLog));
        }

        idFS = glCreateShader(GL_FRAGMENT_SHADER);
        const GLchar* fuenteFS = codigoFS.c_str();
        glShaderSource(idFS, 1, &fuenteFS, nullptr);
        glCompileShader(idFS);

        glGetShaderiv(idFS, GL_COMPILE_STATUS, &exito);
        if (!exito) {
            glGetShaderInfoLog(idFS, 1024, nullptr, infoLog);
            throw std::runtime_error("Error Fragment Shader (" + rutaFS + "):\n" + std::string(infoLog));
        }

        idSP = glCreateProgram();
        glAttachShader(idSP, idVS);
        glAttachShader(idSP, idFS);
        glLinkProgram(idSP);

        glGetProgramiv(idSP, GL_LINK_STATUS, &exito);
        if (!exito) {
            glGetProgramInfoLog(idSP, 1024, nullptr, infoLog);
            throw std::runtime_error("Error al enlazar el Shader Program:\n" + std::string(infoLog));
        }
    }

    /**
    * Método para crear el VAO para el modelo a renderizar
    * @note No se incluye ninguna comprobación de errores
    */
    void PAG::Renderer::creaModelo ( ){
        GLfloat vertices[] = { -.5, -.5, 0,
                                .5, -.5, 0,
                                .0, .5, 0 };
        GLuint indices[] = { 0, 1, 2 };

        glGenVertexArrays ( 1, &idVAO );
        glBindVertexArray ( idVAO );
        glGenBuffers ( 1, &idVBO );
        glBindBuffer ( GL_ARRAY_BUFFER, idVBO );
        glBufferData ( GL_ARRAY_BUFFER, 9*sizeof(GLfloat), vertices, GL_STATIC_DRAW );
        glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), nullptr );
        glEnableVertexAttribArray ( 0 );
        glGenBuffers ( 1, &idIBO );
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, idIBO );
        glBufferData ( GL_ELEMENT_ARRAY_BUFFER, 3*sizeof(GLuint), indices, GL_STATIC_DRAW );
    }

    /**
    * Método para inicializar los parámetros globales de OpenGL
    */
    void PAG::Renderer::inicializaOpenGL ( ){
        glClearColor ( _colorBorrado[0], _colorBorrado[1], _colorBorrado[2], _colorBorrado[3] );
        glEnable ( GL_DEPTH_TEST );
        glEnable ( GL_MULTISAMPLE );
    }


}

