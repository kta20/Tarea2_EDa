#ifndef ESTRUCTURAS_HPP
#define ESTRUCTURAS_HPP

#include <string>
using namespace std;


// Declaraciones adelantadas
struct enemigo;
struct evento;

struct estacion {
  int id;
  string nombre;
  string tipo; // (INICIO, COMBATE, EVENTO, FIN, ESPECIAL)
  string descripcion;
  estacion* n1; // puntero a nodos hijos
  estacion* n2;
  estacion* n3;
  enemigo** enemigos;         // NUEVO: arreglo de punteros a enemigos
  int cantidad_enemigos;      // NUEVO: cantidad de enemigos en la habitación
  evento* evento_asociado; // NUEVO: puntero al evento de la estación (si es EVENTO)
};

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

struct jugador{
  int vida;
  int ataque;
  float precision;
  int recuperacion;
};

struct mejora{
  int vida;
  float precision;
  int ataque;
  int recuperacion;
};

struct bonus{
  string nombre;
  int id_origen;
  string accion;
  string desc_post;
};

struct cola_enemigo{
  enemigo* dato;
  cola_enemigo* sig;
};

#endif