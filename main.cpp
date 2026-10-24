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

// Variables globales para la gestión del ratón y el movimiento de la cámara
static bool botonIzquierdoPulsado = false;
static double ultimoX = 0.0, ultimoY = 0.0;
static PAG::TipoMovimiento movimientoActivo = PAG::TipoMovimiento::ZOOM;

// Buffer de caracteres estático para almacenar el nombre del shader en ImGui nativo
static char nombreShaderBuf[256] = "pag03";

// Buffers y estados globales para la ventana ImGui de Modelos 3D
static char rutaModeloBuf[256] = "modelos/t-rex.obj"; // Buffer para escribir la ruta del archivo OBJ
static int modeloSeleccionadoIndex = 0; // Índice del modelo activo para aplicar transformaciones

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
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS ){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
    //std::cout << "Key callback called" << std::endl;
    addLog("Key callback called");
}

// - Esta función callback será llamada cada vez que se pulse algún botón
// del ratón sobre el área de dibujo OpenGL.
void mouse_button_callback ( GLFWwindow *window, int button, int action, int mods ){
    if (button==GLFW_MOUSE_BUTTON_LEFT) {
        if (action==GLFW_PRESS) {
            botonIzquierdoPulsado = true;
            glfwGetCursorPos(window, &ultimoX, &ultimoY); // Guarda posición inicial del click
        } else if (action==GLFW_RELEASE) {
            botonIzquierdoPulsado=false;
        }
    }
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

// Callback para procesar el movimiento del cursor al hacer clic y arrastrar
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return; // Ignora el movimiento si se interactúa con ImGui
    }

    if (botonIzquierdoPulsado) {
        float deltaX = static_cast<float>(xpos - ultimoX);
        float deltaY = static_cast<float>(ypos - ultimoY);
        float sensibilidad = 0.2f;

        auto& camara = PAG::Renderer::getInstancia().getCamara();

        switch (movimientoActivo) {
            case PAG::TipoMovimiento::PAN:
                camara.pan(deltaX * sensibilidad);
                break;
            case PAG::TipoMovimiento::TILT:
                camara.tilt(deltaY * sensibilidad);
                break;
            case PAG::TipoMovimiento::DOLLY:
                camara.dolly(deltaX * 0.01f, deltaY * 0.01f);
                break;
            case PAG::TipoMovimiento::CRANE:
                camara.crane(-deltaY * 0.01f);
                break;
            case PAG::TipoMovimiento::ORBIT:
                camara.orbit(deltaY * sensibilidad, deltaX * sensibilidad);
                break;
            case PAG::TipoMovimiento::ZOOM:
                camara.zoom(deltaY * sensibilidad);
                break;
        }
    }

    ultimoX = xpos;
    ultimoY = ypos;
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
    glfwSetCursorPosCallback ( window, cursor_position_callback );
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

    try {
        PAG::Renderer::getInstancia().creaShaderProgram (nombreShaderBuf);
        addLog("Shaders 'pag03' cargados y compilados correctamente");
    }catch (const std::exception& e) {
        addLog("[ERROR]"+std::string(e.what()));
    }

    PAG::Renderer::getInstancia().anadirTrianguloProcedural("Triangulo_01");

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

        ImGui::SetNextWindowPos(ImVec2(430, 10), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(320, 340), ImGuiCond_Once);
        if (ImGui::Begin("Fondo")) {
            ImGui::SetWindowFontScale(1.0f);

            // Obtener el color de fondo actual del Renderer
            float colorFondo[3] = {
                PAG::Renderer::getInstancia().getbgRed(),
                PAG::Renderer::getInstancia().getbgGreen(),
                PAG::Renderer::getInstancia().getbgBlue()
            };

            // Control de Rueda/Paleta de Color de ImGui
            if (ImGui::ColorPicker3("##picker", colorFondo)) {
                PAG::Renderer::getInstancia().setbgRed(colorFondo[0]);
                PAG::Renderer::getInstancia().setbgGreen(colorFondo[1]);
                PAG::Renderer::getInstancia().setbgBlue(colorFondo[2]);
                PAG::Renderer::getInstancia().colorear();
            }
        }
        ImGui::End();

        // Ventana ImGui para la gestión interactiva de los Shader Programs
        ImGui::SetNextWindowPos(ImVec2(10, 270), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(400, 120), ImGuiCond_Once);
        if (ImGui::Begin("Shader Program")) {
            ImGui::SetWindowFontScale(1.0f);

            // MODIFICADO: Uso de la versión nativa de ImGui::InputText para arrays de char (requiere buffer y tamaño)
            ImGui::InputText("##nombreBaseShader", nombreShaderBuf, sizeof(nombreShaderBuf), ImGuiInputTextFlags_AutoSelectAll);

            // Botón "Load" para compilar y cargar el shader especificado
            if (ImGui::Button("Load")) {
                try {
                    PAG::Renderer::getInstancia().creaShaderProgram(nombreShaderBuf); // Se envía nombreShaderBuf
                    addLog("Shader Program '" + std::string(nombreShaderBuf) + "' cargado y compilado correctamente."); // Conversión a string para el log
                } catch (const std::exception& e) {
                    addLog("[ERROR EN SHADERS]: " + std::string(e.what()));
                }
            }
        }
        ImGui::End();

        // Ventana ImGui "Camera" para seleccionar el tipo de movimiento y aplicar controles directos
        ImGui::SetNextWindowPos(ImVec2(10, 400), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(400, 160), ImGuiCond_Once);
        if (ImGui::Begin("Camera")) {
            ImGui::SetWindowFontScale(1.0f);

            const char* opcionesMovimiento[] = { "Pan", "Tilt", "Dolly", "Crane", "Orbit", "Zoom" };
            int opcionActual = static_cast<int>(movimientoActivo);

            if (ImGui::Combo("Movement", &opcionActual, opcionesMovimiento, IM_ARRAYSIZE(opcionesMovimiento))) {
                movimientoActivo = static_cast<PAG::TipoMovimiento>(opcionActual);
            }

            auto& camara = PAG::Renderer::getInstancia().getCamara();

            // Interfaz adaptativa según el movimiento seleccionado
            if (movimientoActivo == PAG::TipoMovimiento::ZOOM) {
                float fovy = camara.getFovy();
                if (ImGui::SliderFloat("Angle", &fovy, 1.0f, 90.0f)) {
                    camara.setFovy(fovy);
                }
            } else if (movimientoActivo == PAG::TipoMovimiento::PAN) {
                if (ImGui::Button("<- Left"))  camara.pan(-5.0f);
                ImGui::SameLine();
                if (ImGui::Button("Right ->")) camara.pan(5.0f);
            } else if (movimientoActivo == PAG::TipoMovimiento::TILT) {
                if (ImGui::Button("^ Up ^"))   camara.tilt(-5.0f);
                ImGui::SameLine();
                if (ImGui::Button("v Down v")) camara.tilt(5.0f);
            } else if (movimientoActivo == PAG::TipoMovimiento::DOLLY) {
                if (ImGui::Button("<- Left"))  camara.dolly(-0.1f, 0.0f);
                ImGui::SameLine();
                if (ImGui::Button("Right ->")) camara.dolly(0.1f, 0.0f);
                if (ImGui::Button("^ Back ^"))  camara.dolly(0.0f, 0.1f);
                ImGui::SameLine();
                if (ImGui::Button("v Front v")) camara.dolly(0.0f, -0.1f);
            } else if (movimientoActivo == PAG::TipoMovimiento::CRANE) {
                if (ImGui::Button("^ Up ^"))   camara.crane(0.1f);
                ImGui::SameLine();
                if (ImGui::Button("v Down v")) camara.crane(-0.1f);
            } else if (movimientoActivo == PAG::TipoMovimiento::ORBIT) {
                if (ImGui::Button("North ^")) camara.orbit(5.0f, 0.0f);
                ImGui::SameLine();
                if (ImGui::Button("South v")) camara.orbit(-5.0f, 0.0f);
                if (ImGui::Button("West <-")) camara.orbit(0.0f, -5.0f);
                ImGui::SameLine();
                if (ImGui::Button("East ->")) camara.orbit(0.0f, 5.0f);
            }
        }
        ImGui::End();

        // Ventana ImGui "Modelos 3D" para cargar archivos OBJ, seleccionar objetos y aplicar transformaciones
        ImGui::SetNextWindowPos(ImVec2(430, 260), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(580, 290), ImGuiCond_Once);
        if (ImGui::Begin("Modelos 3D")) {
            ImGui::SetWindowFontScale(1.0f);

            // Casilla para alternar el modo de dibujado sólido o alambre
            bool modoAlambre = PAG::Renderer::getInstancia().getModoAlambre();
            if (ImGui::Checkbox("Modo Alambre (Wireframe)", &modoAlambre)) {
                PAG::Renderer::getInstancia().setModoAlambre(modoAlambre);
            }

            ImGui::Separator();

            // Carga interactiva de modelos OBJ mediante ruta de archivo
            ImGui::Text("Cargar Modelo OBJ:");
            ImGui::InputText("##rutaOBJ", rutaModeloBuf, sizeof(rutaModeloBuf));
            ImGui::SameLine();
            if (ImGui::Button("Cargar OBJ")) {
                try {
                    std::string rutaStr(rutaModeloBuf);
                    std::string nombreModelo = "Modelo_" + std::to_string(PAG::Renderer::getInstancia().getModelos().size() + 1);
                    PAG::Renderer::getInstancia().cargarModeloOBJ(rutaStr, nombreModelo);
                    addLog("Modelo OBJ cargado con éxito: " + rutaStr);
                } catch (const std::exception& e) {
                    addLog("[ERROR MODELO]: " + std::string(e.what()));
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("+ Triangulo")) {
                std::string nombreModelo = "Triangulo_" + std::to_string(PAG::Renderer::getInstancia().getModelos().size() + 1);
                PAG::Renderer::getInstancia().anadirTrianguloProcedural(nombreModelo);
                addLog("Triángulo procedural añadido.");
            }

            ImGui::Separator();

            // Selección de modelo y edición de transformaciones individuales
            auto& modelos = PAG::Renderer::getInstancia().getModelos();
            if (!modelos.empty()) {
                if (modeloSeleccionadoIndex >= static_cast<int>(modelos.size())) {
                    modeloSeleccionadoIndex = static_cast<int>(modelos.size()) - 1;
                }

                std::vector<std::string> nombresModelos;
                std::vector<const char*> itemsPtr;
                for (size_t i = 0; i < modelos.size(); ++i) {
                    nombresModelos.push_back("[" + std::to_string(i) + "] " + modelos[i]->getNombre());
                }
                for (const auto& n : nombresModelos) itemsPtr.push_back(n.c_str());

                ImGui::Combo("Modelo Seleccionado", &modeloSeleccionadoIndex, itemsPtr.data(), static_cast<int>(itemsPtr.size()));

                ImGui::SameLine();
                if (ImGui::Button("Eliminar")) {
                    PAG::Renderer::getInstancia().eliminarModelo(modeloSeleccionadoIndex);
                    addLog("Modelo eliminado de la escena.");
                    if (modeloSeleccionadoIndex > 0) modeloSeleccionadoIndex--;
                }

                if (!modelos.empty() && modeloSeleccionadoIndex < static_cast<int>(modelos.size())) {
                    auto* mod = modelos[modeloSeleccionadoIndex].get();

                    // Control de Posición (Traslación)
                    glm::vec3 pos = mod->getPosicion();
                    if (ImGui::DragFloat3("Posicion (X, Y, Z)", &pos.x, 0.05f)) {
                        mod->setPosicion(pos);
                    }

                    // Control de Rotación en grados
                    glm::vec3 rot = mod->getRotacion();
                    if (ImGui::DragFloat3("Rotacion (deg)", &rot.x, 1.0f)) {
                        mod->setRotacion(rot);
                    }

                    // Control de Escala
                    glm::vec3 esc = mod->getEscala();
                    if (ImGui::DragFloat3("Escala (X, Y, Z)", &esc.x, 0.05f, 0.01f, 10.0f)) {
                        mod->setEscala(esc);
                    }
                }
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No hay modelos en la escena.");
            }
        }
        ImGui::End();

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

