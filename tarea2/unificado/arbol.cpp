#include "arbol.hpp"
#include <set>
#include <iostream>
using namespace std;
//constructor
ArbolTernario::ArbolTernario() : raiz(NULL) {}
//destructor
ArbolTernario::~ArbolTernario() {
    std::set<estacion*> visitados;
    liberar_arbol(raiz, visitados);
}
// asigna raiz
void ArbolTernario::set_raiz(estacion* r) {
    raiz = r;
}
//retorna raiz
estacion* ArbolTernario::get_raiz() {
    return raiz;
}
//muestra arbol en preoden 
void ArbolTernario::mostrar_preorden() {
    recorrer_preorden(raiz, 0);
}
//busca estacion por id
estacion* ArbolTernario::buscar(int id) {
    return buscar_estacion(raiz, id);
}

// crea una nueva estacion con sus datos correspondientes y retorna un puntero a la estacion creada
estacion* ArbolTernario::crear_estacion(int id, string nombre, string tipo, string descripcion) {
    estacion* nueva = new estacion;
    nueva->id = id;
    nueva->nombre = nombre;
    nueva->tipo = tipo;
    nueva->descripcion = descripcion;
    nueva->n1 = NULL;
    nueva->n2 = NULL;
    nueva->n3 = NULL;
    nueva->enemigos = NULL;
    nueva->cantidad_enemigos = 0;
    nueva->evento_asociado = NULL;
    nueva->evento_dinamico = false;
    return nueva;
}

// agrega hijos a cada estacion para conectar estaciones
bool ArbolTernario::agregar_hijo(estacion* padre, estacion* hijo) {
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

// recorre y muestra el arbol en preorden (visita nodo actual y luego a sus hijos de izq a derecha)
void ArbolTernario::recorrer_preorden(estacion* nodo, int nivel) {
    if (nodo == NULL) return;
    for (int i = 0; i < nivel; ++i) cout << "  ";
    cout << nodo->nombre << " (" << nodo->tipo << ")\n";
    recorrer_preorden(nodo->n1, nivel + 1);
    recorrer_preorden(nodo->n2, nivel + 1);
    recorrer_preorden(nodo->n3, nivel + 1);
}
//libera recursivamente memoria de estaciones
void ArbolTernario::liberar_arbol(estacion* nodo, std::set<estacion*>& visitados) {
    if (nodo == NULL) return;
    if (visitados.count(nodo)) return; 
    visitados.insert(nodo);

    liberar_arbol(nodo->n1, visitados);
    liberar_arbol(nodo->n2, visitados);
    liberar_arbol(nodo->n3, visitados);
//libera enemigos 
    if (nodo->enemigos != NULL) {
        for (int i = 0; i < nodo->cantidad_enemigos; ++i) {
            if (nodo->enemigos[i]) delete nodo->enemigos[i];
        }
        delete[] nodo->enemigos;
        nodo->enemigos = NULL;
    }
//libera evento si correspond
    if (nodo->evento_dinamico && nodo->evento_asociado != NULL) {
        delete[] nodo->evento_asociado->opciones;
        delete nodo->evento_asociado;
        nodo->evento_asociado = NULL;
    }

    delete nodo;
}

void ArbolTernario::liberar_arbol(estacion* nodo) {
    std::set<estacion*> visitados;
    liberar_arbol(nodo, visitados);
}

// busca cualquier habitacion a partir de su id
estacion* ArbolTernario::buscar_estacion(estacion* nodo, int id) {
    if (nodo == NULL) return NULL;
    if (nodo->id == id) return nodo;
    estacion* nodo_hijo = buscar_estacion(nodo->n1, id);
    if (nodo_hijo) return nodo_hijo;
    nodo_hijo = buscar_estacion(nodo->n2, id);
    if (nodo_hijo) return nodo_hijo;
    return buscar_estacion(nodo->n3, id);
}

