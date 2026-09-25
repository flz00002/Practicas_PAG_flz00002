//
// Created by felip on 21/09/2026.
//

#ifndef PRACTICA_1_PAG_RENDERER_H
#define PRACTICA_1_PAG_RENDERER_H



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
        float bgRed=0.6f, bgGreen=0.6f, bgBlue=0.6f;
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
    };
}


#endif //PRACTICA_1_PAG_RENDERER_H
