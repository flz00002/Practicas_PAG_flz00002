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
    void PAG::Renderer::creaShaderProgram( ){
        std::string miVertexShader =
            "#version 410\n"
            "layout (location = 0) in vec3 posicion;\n"
            "void main ()\n"
            "{ gl_Position = vec4 ( posicion, 1 );\n"
            "}\n";
        std::string miFragmentShader =
            "#version 410\n"
            "out vec4 colorFragmento;\n"
            "void main ()\n"
            "{ colorFragmento = vec4 ( 1.0, .4, .2, 1.0 );\n"
            "}\n";

        idVS = glCreateShader ( GL_VERTEX_SHADER );
        const GLchar* fuenteVS = miVertexShader.c_str ();
        glShaderSource ( idVS, 1, &fuenteVS, nullptr );
        glCompileShader ( idVS );

        idFS = glCreateShader ( GL_FRAGMENT_SHADER );
        const GLchar* fuenteFS = miFragmentShader.c_str ();
        glShaderSource ( idFS, 1, &fuenteFS, nullptr );
        glCompileShader ( idFS );

        idSP = glCreateProgram ();
        glAttachShader ( idSP, idVS );
        glAttachShader ( idSP, idFS );
        glLinkProgram ( idSP );
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



}

