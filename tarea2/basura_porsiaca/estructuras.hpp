#ifndef ESTRUCTURAS_HPP
#define ESTRUCTURAS_HPP
#include <string>
using namespace std;

struct enemigo {
    string nombre; 
    int vida;
    int ataque;
    float precision;
    float prob_spawn;
};

struct opcion_evento {
    string descripcion;
    int cambio_vida;
    int cambio_ataque;
    float cambio_precision;
    int cambio_recuperacion;
};

struct evento {
    string nombre;
    string descripcion;
    float probabilidad;
    opcion_evento* opciones;
    int cantidad_opciones;
};

struct jugador {
    int vida;
    int ataque;
    float precision;
    int recuperacion;
};

struct mejora {
    int vida;
    float precision;
    int ataque;
    int recuperacion;
};

struct bonus {
    string nombre;
    int id_origen;
    string accion;
    string desc_post;
};

struct cola_enemigo {
    enemigo* dato;
    cola_enemigo* sig;
};

struct estacion {
    int id;
    string nombre;
    string tipo; // (INICIO, COMBATE, EVENTO, FIN, ESPECIAL)
    string descripcion;
    estacion* n1;
    estacion* n2;
    estacion* n3;
    enemigo** enemigos;
    int cantidad_enemigos;
    evento* evento_asociado;
};

struct golpe {
    string atacante;
    string objetivo;
    int fuerza;
    bool acierto;
    int vida_objetivo;
    golpe* sig; // Para la pila
};

// Pila de golpes
struct pila_golpes {
    golpe* tope;
};


#endif