#include "estructuras.hpp"
#include <string>
#include <iostream>

using namespace std;

// crea una nueva estacion con sus datos correspondientes y retorna un puntero a la estacion creada
estacion* crear_estacion(int id, string nombre, string tipo, string descripcion) {
    estacion* nueva = new estacion;
    nueva->id = id;
    nueva->nombre = nombre;
    nueva->tipo = tipo;
    nueva->descripcion = descripcion;
    nueva->n1 = NULL;
    nueva->n2 = NULL;
    nueva->n3 = NULL;
    return nueva;
}

// agrega hijos a cada estacion para conectar estaciones
bool agregar_hijo(estacion* padre, estacion* hijo) {
    if (padre->n1 == NULL) { //verifica si el hijo n1 esta vacio (NULL)
        padre->n1 = hijo;
        return true;
    } else if (padre->n2 == NULL) {
        padre->n2 = hijo;
        return true;
    } else if (padre->n3 == NULL) {
        padre->n3 = hijo;
        return true;
    }
    // retorna falso si ya tiene 3 hijos no vacios
    return false;
}

// busca cualquier habitacion a partir de su id
estacion* buscar_estacion(estacion* nodo, int id) {
    if (nodo == NULL) return NULL; // en caso de buscar un id no existente
    if (nodo->id == id) return nodo;
    estacion* nodo_hijo = buscar_estacion(nodo->n1, id);
    if (nodo_hijo) return nodo_hijo;
    nodo_hijo = buscar_estacion(nodo->n2, id);
    if (nodo_hijo) return nodo_hijo;
    return buscar_estacion(nodo->n3, id);
}

// recorre y muestra el arbol en preorden (visita nodo actual y luego a sus hijos de izq a derecha)
void recorrer_preorden(estacion* nodo, int nivel = 0) {
    if (nodo == NULL) return;
    for (int i = 0; i < nivel; ++i){
        cout << "  ";
    }
    cout << nodo->nombre << " (" << nodo->tipo << ")\n";
    recorrer_preorden(nodo->n1, nivel + 1);
    recorrer_preorden(nodo->n2, nivel + 1);
    recorrer_preorden(nodo->n3, nivel + 1);
}

void liberar_arbol(estacion* nodo) {
    if (nodo == NULL) return;
    liberar_arbol(nodo->n1); // si un nodo no tiene hijos llama  liberar_arbol 3 veces y luego se borra
    liberar_arbol(nodo->n2);
    liberar_arbol(nodo->n3);
    delete nodo;
}