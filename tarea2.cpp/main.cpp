#include "estructuras.hpp"
#include "arbol.hpp"
#include "leer_mapa.hpp"

int main() {
    int total_habitaciones = 0;
    ArbolTernario arbol;
    LectorMapa lector;

    // Usar los métodos de las clases
    estacion** habitaciones = lector.leer_habitaciones("juego.map", total_habitaciones, arbol);
    lector.leer_y_conectar_arcos("juego.map", habitaciones, total_habitaciones, arbol);
    estacion* raiz = lector.obtener_raiz(habitaciones);
    arbol.set_raiz(raiz);

    // Visualiza el árbol
    arbol.mostrar_preorden();

    // Libera memoria (el destructor de ArbolTernario lo hace)

    return 0;
}