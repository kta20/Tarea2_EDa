#ifndef COLA_ENEMIGOS_HPP
#define COLA_ENEMIGOS_HPP

#include "estructuras.hpp"

// Inicializa una cola vacía (devuelve nullptr)
cola_enemigo* crear_cola();

// Encola un enemigo al final
void encolar(cola_enemigo*& frente, cola_enemigo*& fin, enemigo* e);

// Desencola y retorna el enemigo al frente (o nullptr si vacía)
enemigo* desencolar(cola_enemigo*& frente, cola_enemigo*& fin);

// Retorna true si la cola está vacía
bool cola_vacia(cola_enemigo* frente);

// Libera toda la memoria de la cola
void liberar_cola(cola_enemigo*& frente);

#endif