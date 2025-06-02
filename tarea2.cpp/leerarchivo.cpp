using namespace std;
#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "estructuras.hpp"



struct estacion {
    int id;
    string nombre;
    string descripcion;
    string tipo; 
    estacion* n1;
    estacion* n2;
    estacion* n3;

    estacion(int id, string nombre, string descripcion, string tipo)
        : id(id), nombre(nombre), descripcion(descripcion), tipo(tipo),
        n1(nullptr), n2(nullptr), n3(nullptr) 
    

    {

    }

};

estacion** listaEstaciones(ifstream& archivo) {
    string inicio;
    int total_estaciones = 0;

    while (getline(archivo, inicio)) {
        if (inicio.find("HABITACIONES|") == 0) {
            size_t pos = inicio.find('|');
            total_estaciones = stoi(inicio.substr(pos + 1));
            estacion** nodos = new estacion*[total_estaciones];

            for (int i = 0; i < total_estaciones; ++i) {
                if (!getline(archivo, inicio)) {
                    break;
                }
                if (inicio.find("ARCOS|19") == 0) {
                    break;
                }
                estacion* actual = new estacion(    0, "", "", "");


                size_t pos1 = inicio.find('|');
                size_t pos2 = inicio.find('|', pos1 + 1);
                size_t pos3 = inicio.find('|', pos2 + 1);
                int id = stoi(inicio.substr(0, pos1));


                string nombre = inicio.substr(pos1 + 1, pos2 - pos1 - 1);
                string tipo = inicio.substr(pos2 + 1, pos3 - pos2 - 1);
                string descripcion = inicio.substr(pos3 + 1);

                actual -> id = id;
                actual -> nombre = nombre;
                actual -> descripcion = descripcion;
                actual -> tipo = tipo;
                actual -> n1 = NULL;
                actual -> n2 = NULL;
                actual -> n3 = NULL;
                nodos[i] = actual;
            }
            return nodos;
        }
    }
    return NULL;
}




void leerArcos(ifstream& archivo, estacion** nodos) {
    string inicio;
    while (getline(archivo, inicio)) {
        if (inicio.find("ARCOS|19") == 0) {
            size_t pos = inicio.find('|');
            int total_arcos = stoi(inicio.substr(pos + 1));

            for (int i = 0; i < total_arcos; ++i) {
                if (!getline(archivo, inicio)) {
                    break;
                }
                size_t pos1 = inicio.find('|');
                int id1 = stoi(inicio.substr(0, pos1));
                int id2 = stoi(inicio.substr(pos1 + 1));
                nodos[id1]->n1 = nodos[id2];  
            }
        }
    }
}

enemigo** leerEnemigos(ifstream& archivo) {
    string inicio;
    int total_enemigos = 0;
    while (getline(archivo, inicio)) {
        if (inicio.find("ENEMIGOS|") == 0) {
            size_t pos = inicio.find('|');
            total_enemigos = stoi(inicio.substr(pos + 1));
            enemigo** enemigos = new enemigo*[total_enemigos];
            for (int i = 0; i < total_enemigos; ++i) {
                if (!getline(archivo, inicio)) {
                    break;
                }
                size_t pos1 = inicio.find('|');
                size_t pos2 = inicio.find('|', pos1 + 1);
                size_t pos3 = inicio.find('|', pos2 + 1);
                size_t pos4 = inicio.find('|', pos3 + 1);

                string nombre = inicio.substr(0, pos1);
                int vida = stoi(inicio.substr(pos1 + 1, pos2 - pos1 - 1));
                int ataque = stoi(inicio.substr(pos2 + 1, pos3 - pos2 - 1));
                float precision = stof(inicio.substr(pos3 + 1, pos4 - pos3 - 1));
                float prob_spawn = stof(inicio.substr(pos4 + 1));

                enemigos[i] = new enemigo{nombre, vida, ataque, precision, prob_spawn};
            }
            return enemigos;
        }
    }
    return nullptr;
}

evento** leerEventos(ifstream& archivo, int& total_eventos) {
    string linea;
    while (getline(archivo, linea)) {
        if (linea.find("EVENTOS|") == 0) {
            size_t pos = linea.find('|');
            total_eventos = stoi(linea.substr(pos + 1));
            evento** eventos = new evento*[total_eventos];

            for (int i = 0; i < total_eventos; ++i) {
                if (!getline(archivo, linea)) break;
                
                size_t p1 = linea.find('|');
                size_t p2 = linea.find('|', p1 + 1);
                size_t p3 = linea.find('|', p2 + 1);
                size_t p4 = linea.find('|', p3 + 1);
                size_t p5 = linea.find('|', p4 + 1);
                size_t p6 = linea.find('|', p5 + 1);
                size_t p7 = linea.find('|', p6 + 1);
                size_t p8 = linea.find('|', p7 + 1);

                string nombre = linea.substr(0, p1);
                float prob_ev = stof(linea.substr(p1 + 1, p2 - p1 - 1));
                string desc_ev = linea.substr(p2 + 1, p3 - p2 - 1);
                string opcionA = linea.substr(p3 + 1, p4 - p3 - 1);
                string consecA = linea.substr(p4 + 1, p5 - p4 - 1);
                string efectoA = linea.substr(p5 + 1, p6 - p5 - 1);
                string opcionB = linea.substr(p6 + 1, p7 - p6 - 1);
                string consecB = linea.substr(p7 + 1, p8 - p7 - 1);
                string efectoB = linea.substr(p8 + 1);

                eventos[i] = new evento{nombre,prob_ev,desc_ev,opcionA,consecA,efectoA,opcionB,consecB,efectoB};
            }
            return eventos;
        }
    }
    total_eventos = 0;
    return nullptr;
}
mejora** leerMejoras(ifstream& archivo, int& total_mejoras) {
    string linea;
    while (getline(archivo, linea)) {
        if (linea.find("MEJORAS DE COMBATE") == 0) {
            size_t pos = linea.find('|');
            total_mejoras = stoi(linea.substr(pos + 1));
            mejora** mejoras = new mejora*[total_mejoras];

            for (int i = 0; i < total_mejoras; ++i) {
                if (!getline(archivo, linea)) break;

                size_t p1 = linea.find('|');
                size_t p2 = linea.find('|', p1 + 1);
                size_t p3 = linea.find('|', p2 + 1);
                size_t p4 = linea.find('|', p3 + 1);

                int vida = stoi(linea.substr(0, p1));
                float precision = stof(linea.substr(p1 + 1, p2 - p1 - 1));
                int ataque = stoi(linea.substr(p2 + 1, p3 - p2 - 1));
                int recuperacion = stoi(linea.substr(p3 + 1, p4 - p3 - 1));

                mejoras[i] = new mejora{vida, precision, ataque, recuperacion};
            }
            return mejoras;
        }
    }
    total_mejoras = 0;
    return nullptr;
}

int main() {
    ifstream archivo("juego.txt");  // abrir archivo en modo lectura

    if (!archivo.is_open()) {
        cout << "Error al abrir el archivo." << endl;
        return 1;  // termina el programa con error
    }

    estacion** estaciones = listaEstaciones(archivo);
    leerArcos(archivo, estaciones);


    if (estaciones == nullptr) {
        cout << "No se encontró la sección HABITACIONES|" << endl;
    }

    archivo.close();

    return 0;
}
