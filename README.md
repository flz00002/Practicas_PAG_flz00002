# Ejercicio de reflexión Práctica 1
La incompatibilidad se debe a que los métodos de clase
no estáticos en C++ reciben implicitamente el puntero
"this", por lo que su firma no coincide con las
funciones en C que espera GLFW. Para resolverlo de forma
desacoplada y sin usar variables globales, se utiliza un
User Pointer junto con una Proxy.

1- Asociación: Tras crear la ventana, asociamos la 
instancia del renderer a GLFW mediante 
glfwSetWindowUserPointer(window, &PAG::Renderer).

2- Función Puente: Se registra como callback de GLFW 
una función de C pura (o un método static de la clase).

3- Invocación: Dentro de la función puente, recuperamos 
el objeto con
```
auto* renderer = static_cast<PAG::Renderer*>
(glfwGetWindowUserPointer(window));
```
y llamamos al método miembro correspondiente
(```renderer->refrescarVentana()```).

### Declaración de PAG::Renderer
Debe declararse como una variable local dentro de la función
main. Esto evita uso de variables globales, garantiza encapsulamiento
y asegura que su memoria se libere automáticamente al
finalizar el programa

### Módulo de inicialización
El módulo encargado de inicializarlo debe ser main.cpp.

La instanciación debe realizarse estrictamente después de
crear la ventana con GLFW y haber cargado el contexto con
GLAD, pero antes de entrar en el ciclo de eventos.

# Cambios realizados en la Práctica 3

### 1. Carga externa de Shaders GLSL
* **Separación de código:** Se han extraído los shaders del código fuente en C++ a ficheros de texto independientes: `pag03-vs.glsl` (Vertex Shader) y `pag03-fs.glsl` (Fragment Shader).
* **Lectura dinámica de archivos:** Se ha implementado el método auxiliar `PAG::Renderer::cargarArchivoTexto` mediante `std::ifstream` y `std::stringstream` para leer el contenido del código GLSL desde el disco.
* **Parametrización:** El método `PAG::Renderer::creaShaderProgram(const std::string& nombreBase)` ahora recibe como parámetro el nombre base del archivo (por defecto `"pag03"`), concatenando automáticamente los sufijos `-vs.glsl` y `-fs.glsl`.

### 2. Gestión de Errores y Excepciones
* **Comprobación en OpenGL:** Se han integrado las funciones `glGetShaderiv` (con `GL_COMPILE_STATUS`) y `glGetProgramiv` (con `GL_LINK_STATUS`) para detectar fallos sintácticos en la compilación y enlazado de shaders.
* **Lanzamiento de excepciones:** En caso de fallo, `Renderer` obtiene el log de error mediante `glGetShaderInfoLog` o `glGetProgramInfoLog` y lanza una excepción de tipo `std::runtime_error`.
* **Integración con la interfaz ImGui:** En `main.cpp`, la llamada a `creaShaderProgram` se ha envuelto dentro de un bloque `try-catch`. Los errores capturados se redirigen a la consola gráfica de la ventana "Mensajes" de ImGui a través de `addLog()`, evitando el cierre inesperado de la aplicación.

### 3. Gestión y Limpieza de Recursos
* **Liberación de Shaders:** Tras enlazar con éxito el *Shader Program* (`glLinkProgram`), se desvinculan (`glDetachShader`) y eliminan (`glDeleteShader`) los identificadores individuales del Vertex Shader y Fragment Shader para optimizar la memoria gráfica.
* **Desvinculación de VAOs:** Se ha añadido `glBindVertexArray(0)` al finalizar la configuración en `creaModelo()` y tras realizar las llamadas de dibujo en `refrescar()`.

## Pregunta sobre el triángulo Práctica 3
### Si redimensionas la ventana de la aplicación, verás que el triángulo no permanece igual, sino que se deforma al mismo tiempo que la ventana. ¿A qué crees que se debe este comportamiento?
Esto es debido a que los vértices del triángulo están definidos únicamente
en coordenadas normalizadas en el rango [-1,1], las cuales la GPU estira
directamente para adaptar el tamaño del Viewport (glViewport).

Al redimensionar la ventana cambia su relación de aspecto (ancho/alto),
por lo que el espacio [-1,1] se escala de forma desigual en cada eje. Esto
sucede porque no se está utilizando una matriz de proyección que compense
la proporción de la ventana y mantenga la geometría original del objeto.

# Práctica 4

## Desacoplamiento realizado
La gestión de código GLSL se ha extraído de `PAG::Renderer` hacia la clase
dedicada `PAG::ShaderProgram`:
* `PAG::ShaderProgram`: Encapsula el identificador de OpenGL (`idSP`), la 
lectura de archivos GLSL (`-vs.glsl` y `-fs.glsl`), la compilación individual
de *vertex* y *fragment shaders*, el enlazado, la captura de errores, la 
activación con `glUseProgram` y la liberación de recursos.
* `PAG::Renderer`: Mantiene un objeto miembro `_shaderProgram` de tipo 
`PAG::ShaderProgram` y delega en este tanto la compilación como la activación
durante la fase de refresco de la escena.

## Cambios realizados
1. **Creación de la clase `PAG::ShaderProgram` (`ShaderProgram.h` / `ShaderProgram.cpp`):**
    * Método `compilarYEnlazar(const std::string& nombreBase)` para leer, compilar, enlazar y verificar fallos en tiempo de ejecución.
    * Método `activar()` para vincular el programa activo en el pipeline con `glUseProgram`.
    * Método `purgar()` para eliminar el programa de la memoria gráfica con `glDeleteProgram`.
2. **Refactorización de `PAG::Renderer` (`renderer.h` / `renderer.cpp`):**
    * Eliminación de los atributos `idVS`, `idFS` e `idSP`, así como del método privado `cargarArchivoTexto`.
    * Adaptación de `creaShaderProgram(nombreBase)` y `refrescar()` para delegar la lógica en `_shaderProgram`.
3. **Interfaz Gráfica ImGui en `main.cpp`:**
    * Incorporación de la ventana "Shader Program" con un cuadro de texto (`ImGui::InputText`) respaldado por un buffer `char[256]` nativo y un botón `"Load"` para compilar shaders dinámicamente.
    * Captura de excepciones `std::runtime_error` durante el enlazado o compilación, redirigiendo los mensajes de error a la consola gráfica "Mensajes".
4. **Sincronización de Callbacks con ImGui:**
    * Invocación de `ImGui_ImplGlfw_KeyCallback`, `ImGui_ImplGlfw_MouseButtonCallback` y `ImGui_ImplGlfw_ScrollCallback` dentro de los callbacks de GLFW para permitir la edición interactiva del cuadro de texto (borrado con Backspace, selección y navegación con teclado).

## Diagrama UML
```mermaid
classDiagram
    namespace PAG {
        class Renderer {
            -static Renderer* instancia
            -_shaderProgram: ShaderProgram
            -GLuint idVAO
            -GLuint idVBO
            -GLuint idIBO
            -float _colorBorrado[4]
            -Renderer()
            +getInstancia() Renderer&
            +refrescar() void
            +setViewport(int width, int height) void
            +creaShaderProgram(string nombreBase) void
            +creaModelo() void
            +colorear() void
            +activarTestProfundidad() void
        }

        class ShaderProgram {
            -GLuint idSP
            -cargarArchivoTexto(string ruta) string
            +ShaderProgram()
            +~ShaderProgram()
            +compilarYEnlazar(string nombreBase) void
            +activar() void
            +purgar() void
            +getId() GLuint
        }
    }

    Renderer "1" *-- "1" ShaderProgram : encapsula
```

# Práctica 5
## Cambios realizados
### 1. Encapsulacion en la Clase `PAG::Camara`
* **Definición de parámetros:** La cámara almacena su posición en el espacio (`_posicion`), el punto al que mira (`_lookAt`), el vector de orientación vertical (`_up`), el ángulo de visión vertical (`_fovy`), la relación de aspecto del viewport (`_aspecto`) y los planos de corte cercano y lejano (`_zNear`, `_zFar`).
* **Generación de matrices:** Implementación de los métodos `getMatrizVision()` mediante `glm::lookAt` y `getMatrizProyeccion()` mediante `glm::perspective`.
* **Implementación de movimientos:**
   * **Pan / Tilt:** Rotaciones de la dirección de visión sobre el eje local vertical u horizontal respectivamente.
   * **Dolly / Crane:** Traslaciones sobre el plano local de la cámara o sobre el eje vertical $Y$ del mundo.
   * **Orbit:** Rotación en latitud y longitud de la posición de la cámara alrededor del punto de interés (`_lookAt`).
   * **Zoom:** Modificación restringida del ángulo de visión vertical `_fovy`.

### 2. Actualización del Pipeline y Vertex Shader
* **Vertex Shader (`pag03-vs.glsl`):** Incorporación de la variable `uniform mat4 mModelViewProj` para transformar las coordenadas de los vértices del modelo antes de pasar a la fase de rasterización.
* **`PAG::ShaderProgram`:** Método `setUniform(nombre, matriz)` para transferir la matriz `glm::mat4` a la GPU utilizando `glUniformMatrix4fv`.
* **`PAG::Renderer`:** En cada ciclo de refresco (`refrescar()`), el *Renderer* solicita las matrices a la cámara, calcula la matriz combinada $\text{MVP} = \text{Proyección} \times \text{Vista}$ y la envía al *shader program* antes de renderizar la geometría.

### 3. Interacción y Gestión de Eventos
* **Ventana ImGui "Camera":** Ventana dedicada con un selector desplegable (*Combo Box*) para alternar entre los tipos de movimiento (`PAG::TipoMovimiento`) y paneles adaptativos con botones y controles deslizantes (*Sliders*).
* **Arrastre con ratón (`cursor_position_callback`):** Registro de callback en GLFW que detecta cuando el botón izquierdo se mantiene pulsado sobre la ventana de dibujo y traslada la diferencia de píxeles recorridos ($\Delta X, \Delta Y$) al movimiento activo de la cámara.
* **Ajuste de Aspect Ratio:** Modificación de `setViewport` para actualizar dinámicamente la relación de aspecto de la cámara al redimensionar la ventana gráfica.

## Decisión de Diseño y Arquitectura

* **Desacoplamiento Matemático:** La cámara se diseñó como una clase independiente dentro del espacio de nombres `PAG`. De esta forma, el cálculo de trasformaciones matemáticas queda aislado tanto de OpenGL como de la interfaz gráfica.
* **Coordinación desde `PAG::Renderer`:** Se mantiene la arquitectura Singleton de `Renderer` como director principal. `Renderer` posee la instancia de `Camara` y garantiza que la matriz MVP se actualice correctamente en el pipeline en cada fotograma.
* **Prevención de Interferencias con la GUI (`WantCaptureMouse`):** En el callback del movimiento del ratón se comprueba la propiedad `ImGui::GetIO().WantCaptureMouse`. Si el usuario interactúa con controles dentro de una ventana de ImGui, el movimiento de la cámara se bloquea para evitar desplazamientos accidentales.

## Instrucciones de Uso

1. **Control mediante la Ventana "Camera" (ImGui):**
   * Selecciona el movimiento deseado en el menú desplegable **Movement** (*Zoom, Pan, Tilt, Dolly, Crane, Orbit*).
   * Utiliza los botones específicos que aparecen debajo para realizar desplazamientos finos en la dirección elegida.
   * En el modo **Zoom**, ajusta el slider **Angle** para modificar gradualmente el campo de visión.
2. **Control por Ratón (Clic y Arrastre):**
   * Haz clic con el **botón izquierdo del ratón** sobre el lienzo 3D y arrastra en cualquier dirección. La cámara se moverá fluidamente aplicando el tipo de movimiento que esté seleccionado en la ventana "Camera".
3. **Carga de Shaders y Fondo:**
   * Utiliza el panel **Fondo** para cambiar el color de borrado o usa la rueda del ratón.
   * Escribe el nombre base de cualquier shader en el panel **Shader Program** y pulsa **Load** para recompilar en caliente.

## Diagrama UML
```mermaid
classDiagram
namespace PAG {
class TipoMovimiento {
<<enumeration>>
PAN
TILT
DOLLY
CRANE
ORBIT
ZOOM
}

        class Camara {
            -vec3 _posicion
            -vec3 _lookAt
            -vec3 _up
            -float _fovy
            -float _aspecto
            -float _zNear
            -float _zFar
            +getMatrizVision() mat4
            +getMatrizProyeccion() mat4
            +setViewportRatio(float aspecto) void
            +setFovy(float fovy) void
            +pan(float deltaX) void
            +tilt(float deltaY) void
            +dolly(float deltaX, float deltaZ) void
            +crane(float deltaY) void
            +orbit(float deltaLat, float deltaLon) void
            +zoom(float deltaFov) void
        }

        class Renderer {
            -static Renderer* instancia
            -ShaderProgram _shaderProgram
            -Camara _camara
            +getInstancia() Renderer&
            +refrescar() void
            +setViewport(int width, int height) void
            +getCamara() Camara&
        }

        class ShaderProgram {
            -GLuint idSP
            +compilarYEnlazar(string nombreBase) void
            +activar() void
            +setUniform(string nombre, mat4 matriz) void
        }
    }

    Renderer "1" *-- "1" Camara : contiene
    Renderer "1" *-- "1" ShaderProgram : utiliza
```
# Práctica 6: Gestionando modelos 3D

En esta práctica se ha refactorizado la aplicación para desacoplar completamente la gestión de la geometría de la clase `PAG::Renderer`. 
Se introduce la clase `PAG::Modelo`, la integración de la biblioteca **Assimp** para la carga de modelos en formato OBJ, la aplicación 
de **transformaciones de modelado individuales** (traslación, rotación y escalado) y la posibilidad de gestionar múltiples objetos 
simultáneamente en la escena desde **Dear ImGui**.

---

## Decisiones de Diseño y Arquitectura

1. **Desacoplamiento de la Geometría (`PAG::Modelo`):**
    * Se extrajo de `PAG::Renderer` toda la responsabilidad de crear y destruir los objetos OpenGL encargados de la representación geométrica (`VAO`, `VBO`, `IBO`).
    * Cada instancia de `PAG::Modelo` almacena su propia geometría, número de índices, identificadores de buffers y transformaciones geométricas independientes.

2. **Integración de Assimp para la carga de archivos OBJ:**
    * Se utiliza `Assimp::Importer` procesando los flags `aiProcess_JoinIdenticalVertices`, `aiProcess_Triangulate` y `aiProcess_GenSmoothNormals`.
    * Esto permite fusionar vértices duplicados, garantizar la triangulación de polígonos complejos y generar normales suavizadas automáticamente si el archivo OBJ no las define.
    * Las mallas contenidas en la escena importada se condensan en un único conjunto de VBO e IBO por modelo para optimizar el rendimiento y mantener la simplicidad del pipeline.

3. **Gestión Dinámica de la Escena (`PAG::Renderer`):**
    * `PAG::Renderer` almacena la escena mediante una colección vectorial de punteros inteligentes `std::vector<std::unique_ptr<PAG::Modelo>>`.
    * Esto garantiza una gestión automática de la memoria (RAII), invocando los destructores y liberando los buffers de OpenGL en la GPU (`glDeleteVertexArrays`, `glDeleteBuffers`) al eliminar un objeto o al cerrar la aplicación.

4. **Transformaciones de Modelado e Integración MVP:**
    * Cada modelo calcula su matriz de modelado local $M$ en el método `getMatrizModelado()` componiendo las transformaciones en el orden estándar:
      $$M = \text{Translate}(\vec{pos}) \times \text{Rotate}_X(\theta_x) \times \text{Rotate}_Y(\theta_y) \times \text{Rotate}_Z(\theta_z) \times \text{Scale}(\vec{s})$$
    * En cada ciclo de refresco (`refrescar()`), el *Renderer* combina las matrices de proyección ($P$), vista ($V$) y modelado ($M$) de cada objeto para enviar la matriz $\text{MVP} = P \times V \times M$ individualizada al *Shader Program* mediante la uniform `mModelViewProj`.

5. **Modo de Visualización de Alambre (*Wireframe*):**
    * Al no disponer aún de modelos de iluminación, se ha incorporado la casilla "Modo Alambre (Wireframe)" que alterna globalmente entre `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)` y `glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)` para facilitar la inspección de la topología 3D de las mallas.

---

## Instrucciones de Uso (Ventana "Modelos 3D")

1. **Modo Alambre:**
    * Marca o desmarca la casilla **Modo Alambre (Wireframe)** para alternar entre la visualización en alambre y sólida de la escena.

2. **Carga de Modelos 3D:**
    * **Cargar OBJ:** Escribe la ruta de un archivo OBJ (ej. `modelos/coche.obj`) en el cuadro de texto y pulsa el botón **Cargar OBJ**. Si la ruta es válida, el modelo se incorporará a la escena.
    * **+ Triángulo:** Pulsa este botón para añadir un triángulo procedural de prueba en cualquier momento.

3. **Selección y Edición de Transformaciones:**
    * Utiliza el desplegable **Modelo Seleccionado** para elegir el objeto de la escena que deseas modificar.
    * Ajusta los controles de arrastre (*DragFloat3*) para modificar en tiempo real:
        * **Posición (X, Y, Z):** Traslada el objeto en el espacio 3D.
        * **Rotación (deg):** Rota el objeto en grados sobre los ejes X, Y y Z.
        * **Escala (X, Y, Z):** Escala el objeto de forma independiente o uniforme.
    * Pulsa el botón **Eliminar** para remover el objeto seleccionado de la escena gráfica y liberar su memoria GPU.

---

## Diagrama de Clases UML

### Sintaxis PlantUML

```plantuml
@startuml
namespace PAG {
    enum TipoMovimiento {
        PAN
        TILT
        DOLLY
        CRANE
        ORBIT
        ZOOM
    }

    class Camara {
        - glm::vec3 _posicion
        - glm::vec3 _lookAt
        - glm::vec3 _up
        - float _fovy
        - float _aspecto
        - float _zNear
        - float _zFar
        + Camara()
        + glm::mat4 getMatrizVision() const
        + glm::mat4 getMatrizProyeccion() const
        + void setViewportRatio(float aspecto)
        + void setFovy(float fovy)
        + void pan(float deltaX)
        + void tilt(float deltaY)
        + void dolly(float deltaX, float deltaZ)
        + void crane(float deltaY)
        + void orbit(float deltaLat, float deltaLon)
        + void zoom(float deltaFov)
    }

    class ShaderProgram {
        - GLuint idSP
        - std::string cargarArchivoTexto(const std::string& ruta)
        + ShaderProgram()
        + ~ShaderProgram()
        + void compilarYEnlazar(const std::string& nombreBase)
        + void activar() const
        + void purgar()
        + void setUniform(const std::string& nombre, const glm::mat4& matriz)
        + GLuint getId() const
    }

    class Modelo {
        - GLuint _idVAO
        - GLuint _idVBO
        - GLuint _idIBO
        - GLsizei _numIndices
        - std::string _nombre
        - glm::vec3 _posicion
        - glm::vec3 _rotacion
        - glm::vec3 _escala
        - void purgar()
        + Modelo(const std::string& nombre)
        + ~Modelo()
        + void creaTrianguloProcedural()
        + void cargarOBJ(const std::string& rutaArchivo)
        + void dibujar() const
        + glm::mat4 getMatrizModelado() const
        + const std::string& getNombre() const
        + glm::vec3 getPosicion() const
        + void setPosicion(const glm::vec3& pos)
        + glm::vec3 getRotacion() const
        + void setRotacion(const glm::vec3& rot)
        + glm::vec3 getEscala() const
        + void setEscala(const glm::vec3& esc)
    }

    class Renderer {
        - static Renderer* instancia
        - ShaderProgram _shaderProgram
        - Camara _camara
        - std::vector<std::unique_ptr<Modelo>> _modelos
        - bool _modoAlambre
        - float _colorBorrado[4]
        - Renderer()
        + static Renderer& getInstancia()
        + virtual ~Renderer()
        + void refrescar()
        + void setViewport(int width, int height)
        + void colorear()
        + void activarTestProfundidad()
        + void creaShaderProgram(const std::string& nombreBase)
        + void anadirTrianguloProcedural(const std::string& nombre)
        + void cargarModeloOBJ(const std::string& ruta, const std::string& nombre)
        + void eliminarModelo(size_t indice)
        + void vaciarEscena()
        + bool getModoAlambre() const
        + void setModoAlambre(bool alambre)
        + Camara& getCamara()
        + std::vector<std::unique_ptr<Modelo>>& getModelos()
    }

    Renderer "1" *-- "1" ShaderProgram : encapsula >
    Renderer "1" *-- "1" Camara : contiene >
    Renderer "1" *-- "*" Modelo : gestiona >
}
@enduml
```

# Práctica 7: Subrutinas GLSL y Gestión de Materiales

## Decisiones de Diseño

En esta práctica se ha extendido la aplicación gráfica para incorporar la gestión de materiales por objeto y la selección dinámica de modos de renderizado en la GPU sin recurrir a estructuras condicionales dentro de los shaders.

### 1. Estructura e Integración del Material (`struct Material`)
* **Encapsulación interna:** Se ha definido la estructura `Material` directamente dentro del espacio de nombres `PAG` (e integrada dentro de la clase `Modelo`). Esta almacena las propiedades ópticas fundamentales del objeto:
   * `Ka`: Color ambiente ($vec3$).
   * `Kd`: Color difuso ($vec3$).
   * `Ks`: Color especular ($vec3$).
   * `shininess`: Exponente especular / brillo ($float$).
* **Desacoplamiento:** Cada instancia de `Modelo` posee su propio objeto `Material`. Durante el bucle de renderizado en `Renderer::refrescar()`, los parámetros del material (como `uKd`) se envían de forma individual al shader mediante uniforms antes de emitir las órdenes de dibujo (`glDrawElements`).

### 2. Bloques de Interfaz GLSL (`interface blocks`)
* Para organizar el flujo de datos entre las etapas del pipeline programable, se ha sustituido la declaración de variables `out` / `in` independientes por un bloque de interfaz conector denominado `salidaVS`:
   * **Vertex Shader (`pag04-vs.glsl`):** Declara `out salidaVS { vec3 posicionV; vec3 normalV; } salida;`.
   * **Fragment Shader (`pag04-fs.glsl`):** Declara `in salidaVS { vec3 posicionV; vec3 normalV; } entrada;`.
* Esto previene colisiones de nombres en shaders complejos y mantiene una concordancia estructurada del flujo de datos inter-shader.

### 3. Subrutinas GLSL para Modos de Visualización
* **Evitación de condicionales en GPU:** Para cumplir con el objetivo de evitar bifurcaciones en el shader, se ha definido una firma de subrutina `subroutine vec4 fCalcularColor();` asociada al uniform `uMetodoColorElegido`.
* **Implementaciones provistas:**
   1. `colorRojo()`: Utilizada en el **Modo Alambre**, devuelve un color rojo fijo (`vec4(1.0, 0.0, 0.0, 1.0)`).
   2. `colorSolido()`: Utilizada en el **Modo Sólido**, devuelve el color difuso asignado al material del modelo (`vec4(uKd, 1.0)`).
* **Control C++:** En la clase `ShaderProgram`, se utiliza `glGetSubroutineIndex` para consultar los índices asignados por la GPU a cada función e `glUniformSubroutinesuiv` para activar la subrutina requerida antes del renderizado.

### 4. Robustez en la Gestión de Shaders
* Se ha implementado un mecanismo de seguridad en `ShaderProgram::activarSubrutina` que verifica la existencia de subrutinas activas (`GL_ACTIVE_SUBROUTINE_UNIFORMS`) antes de llamar a las funciones de OpenGL. Esto garantiza retrocompatibilidad total con shaders previos (como `pag03`) sin provocar fallos de ejecución.

---

## Manual de Usuario

La interfaz gráfica desarrollada con **Dear ImGui** está organizada en ventanas interactivas independientes:

### 1. Ventana de Mensajes (Log)
* Muestra información técnica en tiempo real sobre el estado del sistema: versión de OpenGL/GLSL, confirmación de carga de shaders, errores de compilación y llamadas a callbacks.

### 2. Ventana de Fondo
* Contiene un selector de color (paleta RGB) para modificar dinámicamente el color de borrado del área de dibujo OpenGL. También se puede ajustar progresivamente haciendo scroll con la rueda del ratón.

### 3. Ventana Shader Program
* Permite introducir el nombre base de un conjunto de shaders (por ejemplo, `pag03` o `pag04`) en un campo de texto y pulsar el botón **Cargar**. La aplicación recompilará y enlazará los shaders al vuelo.

### 4. Ventana Cámara
* **Selector de Movimiento:** Permite elegir la acción interactiva de la cámara: *Pan*, *Tilt*, *Dolly*, *Crane*, *Orbit* o *Zoom*.
* **Control por Ratón:** Haz clic izquierdo y arrastra sobre el viewport 3D para aplicar el movimiento seleccionado.
* **Control Manual:** Incluye botones dedicados e indicadores (como el *Slider* del ángulo FOV en el modo *Zoom*).

### 5. Ventana Modelos 3D y Materiales
* **Modo de Visualización (Checkbox):**
   * **Marcado (Modo Alambre):** Activa `glPolygonMode(..., GL_LINE)` y selecciona la subrutina `colorRojo`.
   * **Desmarcado (Modo Sólido):** Activa `glPolygonMode(..., GL_FILL)` y selecciona la subrutina `colorSolido`.
* **Carga de Modelos:**
   * Escribe la ruta de un archivo OBJ (ej. `modelos/t-rex.obj`) y pulsa **Cargar OBJ**.
   * Haz clic en **+ Triangulo** para añadir mallas procedurales básicas.
* **Gestión de Selección:** Desplegable (*Combo Box*) para elegir el modelo activo en la escena o eliminarlo.
* **Desplegable Transformaciones:** Deslizadores numéricos para modificar en tiempo real la **Posición** ($X, Y, Z$), **Rotación** (grados) y **Escala** del modelo seleccionado.
* **Desplegable Propiedades del Material:** Controles para ajustar los componentes ópticos del objeto activo:
   * **Color Difuso (Kd):** Paleta para cambiar el color base del modelo en modo sólido.
   * **Color Ambiente (Ka):** Ajuste de reflectividad ambiental.
   * **Color Especular (Ks):** Color de los destellos luminosos.
   * **Brillo (Shininess):** Exponente de especularidad (rango $1.0$ a $128.0$).

---

## Diagrama de Clases UML

```mermaid
classDiagram
    class Renderer {
        -static Renderer* instancia
        -ShaderProgram _shaderProgram
        -Camara _camara
        -vector~unique_ptr~Modelo~~ _modelos
        -bool _modoAlambre
        -float _colorBorrado[4]
        +static Renderer& getInstancia()
        +refrescar() void
        +setViewport(width, height) void
        +creaShaderProgram(nombreBase) void
        +cargarModeloOBJ(ruta, nombre) void
        +anadirTrianguloProcedural(nombre) void
        +eliminarModelo(indice) void
        +setModoAlambre(alambre) void
        +getModoAlambre() bool
        +getCamara() Camara&
    }

    class Material {
        +vec3 Ka
        +vec3 Kd
        +vec3 Ks
        +float shininess
    }

    class Modelo {
        -GLuint _idVAO
        -GLuint _idVBO
        -GLuint _idIBO
        -GLsizei _numIndices
        -string _nombre
        -Material _material
        -vec3 _posicion
        -vec3 _rotacion
        -vec3 _escala
        +cargarOBJ(ruta) void
        +creaTrianguloProcedural() void
        +dibujar() const void
        +getMatrizModelado() mat4
        +getMaterial() Material&
        +setMaterial(mat) void
        +setPosicion(pos) void
        +setRotacion(rot) void
        +setEscala(esc) void
    }

    class Camara {
        -vec3 _posicion
        -vec3 _centro
        -vec3 _arriba
        -float _fovy
        -float _aspectRatio
        +getMatrizVision() mat4
        +getMatrizProyeccion() mat4
        +pan(angulo) void
        +tilt(angulo) void
        +dolly(dx, dy) void
        +crane(dy) void
        +orbit(dPitch, dYaw) void
        +zoom(delta) void
    }

    class ShaderProgram {
        -GLuint idSP
        +compilarYEnlazar(nombreBase) void
        +activar() const void
        +purgar() void
        +setUniform(nombre, matriz) void
        +setUniform(nombre, vector) void
        +activarSubrutina(shaderType, nombreSubrutina) void
    }

    Renderer "1" *-- "1" ShaderProgram : gestiona
    Renderer "1" *-- "1" Camara : contiene
    Renderer "1" *-- "*" Modelo : contiene colección
    Modelo "1" *-- "1" Material : posee
```