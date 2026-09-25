#include <GL/gl.h>
#include "Renderer.h"
#include <iostream>
#include "glad/glad.h"
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
    Renderer::~Renderer (){ }
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
    /**
    * Método para hacer el refresco de la escena
    */
    void Renderer::refrescar ()
    { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::setViewport (int width, int height) {
        glViewport(0, 0, width, height);
    }

    float Renderer::getbgRed() {
        return bgRed;
    }

    float Renderer::getbgGreen() {
        return bgGreen;
    }

    float Renderer::getbgBlue() {
        return bgBlue;
    }

    void Renderer::setbgRed (float bgRed) {
        this->bgRed = bgRed;
    }
    void Renderer::setbgGreen (float bgGreen) {
        this->bgGreen = bgGreen;
    }
    void Renderer::setbgBlue (float bgBlue) {
        this->bgBlue = bgBlue;
    }

    void Renderer::colorear () {
        glClearColor(bgRed,bgGreen,bgBlue,1.0f);
        std::cout << "Color de fondo RGB: (" << bgRed << ", " << bgGreen << ", " << bgBlue << ")" << std::endl;
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
}

