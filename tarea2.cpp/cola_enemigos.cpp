#include "cola_enemigos.hpp"

cola_enemigo* crear_cola() {
    return NULL;
}

void encolar(cola_enemigo*& frente, cola_enemigo*& fin, enemigo* e) {
    cola_enemigo* nuevo = new cola_enemigo;
    nuevo->dato = e;
    nuevo->sig = NULL;
    if (fin) {
        fin->sig = nuevo;
    } else {
        frente = nuevo;
    }
    fin = nuevo;
}

enemigo* desencolar(cola_enemigo*& frente, cola_enemigo*& fin) {
    if (!frente) return NULL;
    cola_enemigo* temp = frente;
    enemigo* e = temp->dato;
    frente = frente->sig;
    if (!frente) fin = NULL;
    delete temp;
    return e;
}

bool cola_vacia(cola_enemigo* frente) {
    return frente == NULL;
}

void liberar_cola(cola_enemigo*& frente) {
    while (frente) {
        cola_enemigo* temp = frente;
        frente = frente->sig;
        delete temp;
    }
}