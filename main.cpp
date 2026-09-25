#include <iostream>
#include <vector>
// IMPORTANTE: El include de GLAD debe estar siempre ANTES de el de GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "renderer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


// Vector global para almacenar los mensajes de la consola de ImGui
std::vector<std::string> logMensajes;

// Función aux para añadir mensajes al log
void addLog(const std::string& msg) {
    logMensajes.push_back(msg);
}

// - Esta función callback será llamada cuando GLFW produzca algún error
void error_callback ( int errno, const char* desc ){
    std::string aux (desc);
    //std::cout << "Error de GLFW número " << errno << ": " << aux << std::endl;
    addLog("Error de GLFW numero "+std::to_string(errno)+": "+aux);
}

// - Esta función callback será llamada cada vez que el área de dibujo
// OpenGL deba ser redibujada.
void window_refresh_callback ( GLFWwindow *window ){
    PAG::Renderer::getInstancia().refrescar ();
    // - GLFW usa un doble buffer para que no haya parpadeo. Esta orden
    // intercambia el buffer back (que se ha estado dibujando) por el
    // que se mostraba hasta ahora front. Debe ser la última orden de
    // este callback
    glfwSwapBuffers ( window );
    //std::cout << "Refresh callback called" << std::endl;
    addLog("Refresh callback called");
}

// - Esta función callback será llamada cada vez que se cambie el tamaño
// del área de dibujo OpenGL.
void framebuffer_size_callback ( GLFWwindow *window, int width, int height ){
    PAG::Renderer::getInstancia().setViewport ( width, height );
    //std::cout << "Resize callback called" << std::endl;
    addLog("Resize callback called");
}

// - Esta función callback será llamada cada vez que se pulse una tecla
// dirigida al área de dibujo OpenGL.
void key_callback ( GLFWwindow *window, int key, int scancode, int action, int mods ){
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS ){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
    //std::cout << "Key callback called" << std::endl;
    addLog("Key callback called");
}

// - Esta función callback será llamada cada vez que se pulse algún botón
// del ratón sobre el área de dibujo OpenGL.
void mouse_button_callback ( GLFWwindow *window, int button, int action, int mods ){
    if ( action == GLFW_PRESS ){
        //std::cout << "Pulsado el boton: " << button << std::endl;
        addLog("Pulsado el boton "+std::to_string(button));
        ImGuiIO& io = ImGui::GetIO ();
        io.AddMouseButtonEvent ( button, true );
    }else if ( action == GLFW_RELEASE ){
        //std::cout << "Soltado el boton: " << button << std::endl;
        addLog("Soltado el boton "+std::to_string(button));
        ImGuiIO& io = ImGui::GetIO ();
        io.AddMouseButtonEvent ( button, false );
    }
}

// - Esta función callback será llamada cada vez que se mueva la rueda
// del ratón sobre el área de dibujo OpenGL.
void scroll_callback ( GLFWwindow *window, double xoffset, double yoffset ){
    float paso=0.05f;
    float auxR=0.6f, auxG=0.6f, auxB=0.6f;
    if (yoffset > 0) {
        auxR=std::min ( 1.0f,PAG::Renderer::getInstancia().getbgRed()+paso);
        auxG=std::min ( 1.0f,PAG::Renderer::getInstancia().getbgGreen()+paso);
        auxB=std::min ( 1.0f,PAG::Renderer::getInstancia().getbgBlue()+paso);
    }else if (yoffset < 0) {
        auxR=std::max(0.0f,PAG::Renderer::getInstancia().getbgRed()-paso);
        auxG=std::max(0.0f,PAG::Renderer::getInstancia().getbgGreen()-paso);
        auxB=std::max(0.0f,PAG::Renderer::getInstancia().getbgBlue()-paso);
    }
    PAG::Renderer::getInstancia().setbgRed(auxR);
    PAG::Renderer::getInstancia().setbgGreen(auxG);
    PAG::Renderer::getInstancia().setbgBlue(auxB);
    PAG::Renderer::getInstancia().colorear();
    addLog("Color de fondo RGB: ("+std::to_string(auxR)+","+std::to_string(auxG)+","+std::to_string(auxB)+")");
}

int main(){
    std::cout << "Starting Application PAG - Prueba 01" << std::endl;
    // - Este callback hay que registrarlo ANTES de llamar a glfwInit
    glfwSetErrorCallback ( (GLFWerrorfun) error_callback );
    // - Inicializa GLFW. Es un proceso que sólo debe realizarse una vez en la aplicación
    if ( glfwInit () != GLFW_TRUE )
    { std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // - Definimos las características que queremos que tenga el contexto gráfico
    // OpenGL de la ventana que vamos a crear. Por ejemplo, el número de muestras o el
    // modo Core Profile.
    glfwWindowHint ( GLFW_SAMPLES, 4 ); // - Activa antialiasing con 4 muestras.
    glfwWindowHint ( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE ); // - Esta y las 2
    glfwWindowHint ( GLFW_CONTEXT_VERSION_MAJOR, 4 ); // siguientes activan un contexto
    glfwWindowHint ( GLFW_CONTEXT_VERSION_MINOR, 3 ); // OpenGL Core Profile 4.3.
    // - Definimos el puntero para guardar la dirección de la ventana de la aplicación y
    // la creamos
    GLFWwindow *window;
    // - Tamaño, título de la ventana, en ventana y no en pantalla completa,
    // sin compartir recursos con otras ventanas.
    window = glfwCreateWindow ( 1024, 576, "PAG Introduction", nullptr, nullptr );
    // - Comprobamos si la creación de la ventana ha tenido éxito.
    if ( window == nullptr )
    { std::cout << "Failed to open GLFW window" << std::endl;
        glfwTerminate (); // - Liberamos los recursos que ocupaba GLFW.
        return -2;
    }
    // - Hace que el contexto OpenGL asociado a la ventana que acabamos de crear pase a
    // ser el contexto actual de OpenGL para las siguientes llamadas a la biblioteca
    glfwMakeContextCurrent ( window );
    // - Ahora inicializamos GLAD.
    if ( !gladLoadGLLoader ( (GLADloadproc) glfwGetProcAddress ) ){
        std::cout << "GLAD initialization failed" << std::endl;
        glfwDestroyWindow ( window ); // - Liberamos los recursos que ocupaba GLFW.
        window = nullptr;
        glfwTerminate ();
        return -3;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext ();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL ( window, true );
    ImGui_ImplOpenGL3_Init ();

    // - Interrogamos a OpenGL para que nos informe de las propiedades del contexto
    // 3D construido.
    addLog(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    addLog(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    addLog(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    addLog(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    //PAG::Renderer::getInstancia().informePropiedades();

    // - Registramos los callbacks que responderán a los eventos principales
    glfwSetWindowRefreshCallback ( window, window_refresh_callback );
    glfwSetFramebufferSizeCallback ( window, framebuffer_size_callback );
    glfwSetKeyCallback ( window, key_callback );
    glfwSetMouseButtonCallback ( window, mouse_button_callback );
    glfwSetScrollCallback ( window, scroll_callback );

    // - Establecemos un gris medio como color con el que se borrará el frame buffer.
    // No tiene por qué ejecutarse en cada paso por el ciclo de eventos.
    PAG::Renderer::getInstancia().colorear();
    // - Le decimos a OpenGL que tenga en cuenta la profundidad a la hora de dibujar.
    // No tiene por qué ejecutarse en cada paso por el ciclo de eventos.
    PAG::Renderer::getInstancia().activarTestProfundidad();
    // - Ciclo de eventos de la aplicación. La condición de parada es que la
    // ventana principal deba cerrarse. Por ejemplo, si el usuario pulsa el
    // botón de cerrar la ventana (la X).
    while ( !glfwWindowShouldClose ( window ) ){

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // Se dibujan los controles de Dear ImGui

        ImGui::SetNextWindowPos ( ImVec2 (10, 10), ImGuiCond_Once );
        ImGui::SetNextWindowSize(ImVec2(400, 250), ImGuiCond_Once);

        if ( ImGui::Begin ( "Mensajes" ) )
        { // La ventana está desplegada
            ImGui::SetWindowFontScale ( 1.0f ); // Escalamos el texto si fuera necesario
            for (const auto& msg : logMensajes) {
                ImGui::TextUnformatted(msg.c_str());
            }
            // Mantiene el scroll en la parte inferior si hay nuevos mensajes
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }
        }
        // Si la ventana no está desplegada, Begin devuelve false
        ImGui::End ();

        // - Borra los buffers (color y profundidad)
        PAG::Renderer::getInstancia().refrescar();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData ( ImGui::GetDrawData() );

        // - GLFW usa un doble buffer para que no haya parpadeo. Esta orden
        // intercambia el buffer back (en el que se ha estado dibujando) por el
        // que se mostraba hasta ahora (front).
        glfwSwapBuffers ( window );
        // - Obtiene y organiza los eventos pendientes, tales como pulsaciones de
        // teclas o de ratón, etc. Siempre al final de cada iteración del ciclo
        // de eventos y después de glfwSwapBuffers(window);
        glfwPollEvents ();
    }
    // - Una vez terminado el ciclo de eventos, liberar recursos, etc.
    std::cout << "Finishing application pag prueba" << std::endl;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext ();

    glfwDestroyWindow ( window ); // - Cerramos y destruimos la ventana de la aplicación.
    window = nullptr;
    glfwTerminate (); // - Liberamos los recursos que ocupaba GLFW.
}

