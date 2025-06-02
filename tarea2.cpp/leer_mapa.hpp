#ifndef LEER_MAPA_HPP
#define LEER_MAPA_HPP

#include "arbol.hpp"
#include "estructuras.hpp"
#include <string>
using namespace std;

class LectorMapa {
public:
    // Lee las habitaciones y retorna un arreglo de punteros a estacion
    estacion** leer_habitaciones(string filename, int& total_habitaciones, ArbolTernario& arbol);

    // Lee los arcos y conecta las habitaciones
    void leer_y_conectar_arcos(string filename, estacion** habitaciones, int total_habitaciones, ArbolTernario& arbol);

    // Devuelve la raíz del árbol (habitaciones[0])
    estacion* obtener_raiz(estacion** habitaciones);
};

// Declaraciones de funciones auxiliares:
enemigo** leer_enemigos_globales(string filename, int& total_enemigos);
evento** leer_eventos_globales(string filename, int& total_eventos);

#endif