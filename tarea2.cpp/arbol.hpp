#ifndef ARBOL_HPP
#define ARBOL_HPP

#include "estructuras.hpp"
#include <string>
#include <iostream>
using namespace std;

class ArbolTernario {
private:
    estacion* raiz;
    void recorrer_preorden(estacion* nodo, int nivel);
    void liberar_arbol(estacion* nodo);
    estacion* buscar_estacion(estacion* nodo, int id);

public:
    ArbolTernario();
    ~ArbolTernario();
    void set_raiz(estacion* r);
    estacion* get_raiz();
    void mostrar_preorden();
    estacion* buscar(int id);
    estacion* crear_estacion(int id, string nombre, string tipo, string descripcion);
    bool agregar_hijo(estacion* padre, estacion* hijo);
};

#endif