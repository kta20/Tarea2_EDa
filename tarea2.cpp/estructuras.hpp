#ifndef ESTRUCTURAS_HPP
#define ESTRUCTURAS_HPP

#include <string>
using namespace std;

struct estacion {
  int id;
  string nombre;
  string tipo; // (INICIO, COMBATE, EVENTO, FIN, ESPECIAL)
  string descripcion;
  estacion* n1; // puntero a nodos hijos
  estacion* n2;
  estacion* n3; 
};

struct enemigo {
  string nombre; 
  int vida;
  int ataque;
  float precision;
  float prob_spawn;
};

struct evento {
  string nombre;
  float prob_ev;
  string desc_ev;
  string opcionA;
  string consecA; //muestra consecuencia de A
  string efectoA;
  string opcionB;
  string consecB;
  string efectoB;
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