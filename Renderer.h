//
// Created by felip on 21/09/2026.
//

#ifndef PRACTICA_1_PAG_RENDERER_H
#define PRACTICA_1_PAG_RENDERER_H

#include <glad/glad.h>
#include <iostream>

#include "ShaderProgram.h"
#include "Camara.h"

/**
* Espacio de nombres para las prácticas de Programación de Aplicaciones Gráficas
*/
namespace PAG
{
    /**
    * @brief Clase encargada de encapsular la gestión del área de dibujo
    * OpenGL
    *
    * Esta clase coordina el renderizado de las escenas OpenGL. Se implementa
    * aplicando el patrón de diseño Singleton. Está pensada para que las
    * funciones callback hagan llamadas a sus métodos
    */
    class Renderer
    {
    private:
        static Renderer* instancia; ///< Puntero al único objeto
        Renderer ();

        ShaderProgram _shaderProgram;
        Camara _camara;

        /*
        GLuint idVS = 0; // Identificador del vertex shader
        GLuint idFS = 0; // Identificador del fragment shader
        GLuint idSP = 0; // Identificador del shader program
        */

        GLuint idVAO = 0; // Identificador del vertex array object
        GLuint idVBO = 0; // Identificador del vertex buffer object
        GLuint idIBO = 0; // Identificador del index buffer object

        float _colorBorrado[4]={0.2f,0.3f,0.3f,1.0f};
    public:
        virtual ~Renderer ();
        static Renderer& getInstancia ();

        void refrescar ();
        void setViewport(int width, int height);
        void colorear();
        float getbgRed ();
        float getbgGreen ();
        float getbgBlue ();
        void setbgRed (float bgRed);
        void setbgGreen (float bgGreen);
        void setbgBlue (float bgBlue);

        void activarTestProfundidad();
        void informePropiedades();

        void creaShaderProgram(const std::string& nombreBase="pag03");

        void creaModelo();

        void inicializaOpenGL();

        Camara& getCamara() { return _camara; }

        //std::string cargarArchivoTexto(const std::string& ruta);
    };
}


#endif //PRACTICA_1_PAG_RENDERER_H
