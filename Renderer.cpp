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
        /*if ( idVS != 0 )
        { glDeleteShader ( idVS );
        }
        if ( idFS != 0 )
        { glDeleteShader ( idFS );
        }
        if ( idSP != 0 )
        { glDeleteProgram ( idSP );
        }*//*
        if ( idVBO != 0 )
        { glDeleteBuffers ( 1, &idVBO );
        }
        if ( idIBO != 0 )
        { glDeleteBuffers ( 1, &idIBO );
        }
        if ( idVAO != 0 )
        { glDeleteVertexArrays ( 1, &idVAO );
        }*/
        _modelos.clear(); // Se destruyen automáticamente todos los objetos Modelo al salir
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

    /*std::string Renderer::cargarArchivoTexto(const std::string& ruta) {
        std::ifstream archivo(ruta);
        if (!archivo.is_open()) {
            throw std::runtime_error("No se pudo abrir el archivo de shader: " + ruta);
        }
        std::stringstream buffer;
        buffer << archivo.rdbuf();
        return buffer.str();
    }*/

    /**
    * Método para hacer el refresco de la escena
    */
    void Renderer::refrescar (){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Alterna el modo de rasterización entre sólido y alambre (útil sin luces)
        if (_modoAlambre) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Modo alambre activo
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Modo sólido activo
        }

        _shaderProgram.activar();

        /*glm::mat4 mvp = _camara.getMatrizProyeccion() * _camara.getMatrizVision();
        _shaderProgram.setUniform("mModelViewProj", mvp);*/

        glm::mat4 mProyeccion = _camara.getMatrizProyeccion();
        glm::mat4 mVision = _camara.getMatrizVision();

        // Renderizado iterativo de todos los modelos presentes aplicando su matriz MVP individual
        for (const auto& modelo : _modelos) {
            glm::mat4 mModel = modelo->getMatrizModelado(); // Obtención de la matriz de modelado
            glm::mat4 mvp = mProyeccion * mVision * mModel; // Matriz MVP completa = P * V * M
            _shaderProgram.setUniform("mModelViewProj", mvp); // Paso de la matriz al Shader Program
            modelo->dibujar(); // AÑADIDO: Dibujado del objeto
        }
    }

    void Renderer::setViewport (int width, int height) {
        glViewport(0, 0, width, height);
        if (height > 0) {
            _camara.setViewportRatio((float)width / (float)height);
        }
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
        _shaderProgram.compilarYEnlazar(nombreBase);
    }

    // Implementación de la gestión dinámica de mallas
    void PAG::Renderer::anadirTrianguloProcedural(const std::string& nombre) {
        auto modelo = std::make_unique<Modelo>(nombre);
        modelo->creaTrianguloProcedural();
        _modelos.push_back(std::move(modelo));
    }

    void PAG::Renderer::cargarModeloOBJ(const std::string& rutaArchivo, const std::string& nombre) {
        auto modelo = std::make_unique<Modelo>(nombre);
        modelo->cargarOBJ(rutaArchivo);
        _modelos.push_back(std::move(modelo));
    }

    void PAG::Renderer::eliminarModelo(size_t indice) {
        if (indice < _modelos.size()) {
            _modelos.erase(_modelos.begin() + indice);
        }
    }

    void PAG::Renderer::vaciarEscena() {
        _modelos.clear();
    }


    /**
    * Método para crear el VAO para el modelo a renderizar
    * @note No se incluye ninguna comprobación de errores
    */
    /*
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

        glBindVertexArray ( 0 );
    }*/

    /**
    * Método para inicializar los parámetros globales de OpenGL
    */
    void PAG::Renderer::inicializaOpenGL ( ){
        glClearColor ( _colorBorrado[0], _colorBorrado[1], _colorBorrado[2], _colorBorrado[3] );
        glEnable ( GL_DEPTH_TEST );
        glEnable ( GL_MULTISAMPLE );
    }


}

