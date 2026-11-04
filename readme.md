# Tarea 2 - Sistemas Operativos: Torneo de Komikan Concurrente

Ignacio Espinoza - Ingeniería Civil en Informática y Telecomunicaciones, UDP - sección 3

## Qué hace
Simula un torneo concurrente del juego de mesa mapuche Komikan. A diferencia de un juego de mesa tradicional, al ser concurrente, las fichas compiten simultáneamente por las casillas mediante hilos. El sistema empareja a los dos jugadores en mesas (respetando un límite máximo de M mesas), crea un juez por cada partida, verifica los movimientos de las 13 fichas mediante exclusión mutua, escribe un archivo de registro por mesa y responde correctamente a Ctrl+C para abortar y liberar todos los recursos.

### Reglas de cierre de juego
* **Gana el puma** si se come 6 o más perros.
* **Ganan los perros** si el puma queda acorralado.
* **Empate** si se llega a 500 movimientos sin ganador.
* **Abandono**: si una ficha "se cae", gana el otro bando.

### El tablero
Grid de 5 × 5 con casillas numeradas del 0 al 24, donde `id = y * 5 + x` permite ubicarse mediante coordenadas cartesianas. Además, hay tres casillas de trono: izquierda (`id 25`), derecha (`id 26`) y arriba (`id 27`). Los perros solo caminan, mientras que el puma puede caminar o saltar sobre un perro para comérselo.

## Archivos
El código está dividido en varios módulos para mantener el orden:
* `torneo.cpp`: El `main`, emparejamiento de mesas, semáforo global y manejo de la señal.
* `mesa.cpp` / `mesa.hpp`: Define la estructura de la partida, inicializa/destruye mutexes y contiene la lógica de cerrar/abrir casillas.
* `juez.cpp` / `juez.hpp`: El hilo árbitro que evalúa las reglas, detecta victorias y escribe los logs.
* `puma.cpp` / `puma.hpp`: Lógica del hilo del puma.
* `can.cpp` / `can.hpp`: Lógica de los 12 hilos de los perros.
* `livritoReglas.cpp` / `livritoReglas.hpp`: Funciones de reglas: validar paso y salto, aplicarlos, y detectar si el puma ganó o quedó acorralado.
* `tablita.cpp` / `tablita.hpp`: Construye el grafo de adyacencia del tablero o sea el grid 5x5 + 3 casillas de trono y las rutas de captura.
* `estado.cpp` / `estado.hpp`: Estructura del estado compartido o sea quién está en cada casilla y contador de capturas.
* `config.cpp` / `config.hpp`: Lector del archivo `.txt` de configuración.
* `azar.hpp`: Generador de números aleatorios para cada ficha.
* `apagado.hpp`: Hilo dedicado a esperar el Ctrl+C y la lista de mesas activas que ese hilo necesita para cerrarlas.
* `synchro.hpp`: Creación del `guarda` y plantillas del semaforo.
* `consola.hpp`: Mutex global para que los hilos no impriman texto mezclado en la terminal.
* `dibujadorDlogs.hpp`: Dibuja el tablero en texto en los archivos mesa_*.txt, `P` = puma, `D3` = perro 3, `x` = vacía.

## Hilos y sincronización
Por cada partida se crea un hilo de mesa que lanza 14 hilos: un juez, un puma y 12 perros.
* **Un mutex por casilla** (28 en total): para moverse, una ficha toma con `trylock` los mutex de las casillas de origen y destino. En caso de captura, también toma el de la casilla intermedia. Si alguno está ocupado, libera los mutex que alcanzó a tomar y vuelve a intentarlo, hasta un máximo de tres intentos. La casilla se bloquea solo durante el movimiento, después de verificar que esté libre.
* **`mutexAviso` + variable de condición**: protegen el contador de movimientos, el indicador de término, el resultado y la lista de jugadas. Las fichas avisan al juez cada vez que se mueven.
* **Semáforo `cupos`**: limita a `M` las mesas jugando a la vez, las demás esperan su turno.
* **`mutexConsola`**: permite imprimir una línea a la vez.
* **`mutexTorneo`**: protege la lista de mesas activas y el indicador `apagando`.

## Compilar y ejecutar
**Compilar:**

```bash
g++ -Wall -Wextra -std=c++17 *.cpp -o torneo -pthread
```

**Ejecutar:**

```bash
./torneo N M [config.txt]
```

* `N`: número total de jugadores que se procesarán.
* `M`: número máximo de mesas activas simultáneamente.
* `[config.txt]`: ruta opcional al archivo de configuración. Si no se indica, se usan los valores por defecto.
* `N` y `M` deben ser mayores que 0.
* Ctrl+C detiene el ingreso de nuevas mesas y cierra ordenadamente las partidas en curso. Las mesas que estaban esperando cupo no llegan a jugar.

## Logs
Cada mesa escribe su propio archivo `mesa_N.txt`, con las jugadas numeradas y un dibujo del tablero después de cada tanda de movimientos. Además, cada jugada y el resultado final se imprimen en la terminal.

##Visualización del tablero
- 0-24 -> grid 5x5
- 25 -> trono izquierda
- 26 -> trono derecha
- 27 -> trono arriba
```text
        27
  25          26
20  21  22  23  24
15  16  17  18  19
10  11  12  13  14
 5   6   7   8   9
 0   1   2   3   4
```
## Formato de config.txt
Permite fijar la semilla, los retardos en milisegundos y la posición inicial de las fichas. Si no se indica un archivo, se usan los valores por defecto, que son los mismos del ejemplo siguiente.
```text
SEED 42
DELAY_MIN 50
DELAY_MAX 300
PUMA_INIT TRONO_TOP
#  id|x|y
DOG 0 0 0
DOG 1 1 0
...
```
* `PUMA_INIT` acepta `TRONO_TOP`, `TRONO_LEFT`, `TRONO_RIGHT` o coordenadas `x y` del grid; por ejemplo: `PUMA_INIT 0 0`.
* `DOG id x y`: `id` va de 0 a 11, y `x` e `y` van de 0 a 4.

* Si una línea no es válida o `DELAY_MIN` es mayor que `DELAY_MAX`, el programa avisa por `cerr` y no se inicia.

### Configs de prueba incluidos
* `config.txt`: configuración por defecto.
* `config2.txt`: retardo fijo de 1 ms para partidas rápidas.
* `config_encerrado_forzado.txt`: el puma parte en la esquina `0 0`, rodeado de perros, así que queda acorralado de inmediato y ganan los perros.

## Probar el abandono
En `mesa.hpp`, `mesaQueSeCae` y `fichaQueSeCae` están en `-1`; es decir, nadie abandona. Para probarlo, se puede indicar el id de una mesa y el id de una ficha (de 0 a 11 para los perros y 12 para el puma), y luego recompilar. Esa ficha abandonará y ganará el otro bando.

## Referencias
Para las reglas del juego se tomó como base el enunciado de la tarea, junto con las siguientes fuentes:
- https://es.wikipedia.org/wiki/Komikan
- https://www.chileparaninos.gob.cl/639/articles-639005_archivo_01.pdf