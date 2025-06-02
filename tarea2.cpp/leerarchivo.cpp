#include <iostream>
#include <fstream>
#include <string>
#include "estructuras.hpp"
#include "arbol.hpp"
using namespace std;

// Lee las estaciones y devuelve un arreglo de punteros, y el máximo id encontrado
estacion** leerEstaciones(ifstream& archivo, int& max_id, int& total_estaciones) {
    string linea;
    max_id = -1;
    total_estaciones = 0;
    // Primero, busca la sección y el total
    streampos pos_archivo = archivo.tellg();
    while (getline(archivo, linea)) {
        if (linea.find("HABITACIONES|") == 0) {
            size_t pos = linea.find('|');
            total_estaciones = stoi(linea.substr(pos + 1));
            break;
        }
    }
    if (total_estaciones == 0) return nullptr;

    // Encuentra el id máximo
    int* ids = new int[total_estaciones];
    for (int i = 0; i < total_estaciones; ++i) {
        getline(archivo, linea);
        size_t pos1 = linea.find('|');
        int id = stoi(linea.substr(0, pos1));
        ids[i] = id;
        if (id > max_id) max_id = id;
    }

    // En vez de seekg, cerramos y reabrimos el archivo
    archivo.close();
    archivo.open("juego.map");
    if (!archivo.is_open()) {
        delete[] ids;
        return nullptr;
    }

    // Ahora sí, crea el arreglo con tamaño suficiente
    estacion** estaciones = new estacion*[max_id + 1];
    for (int i = 0; i <= max_id; ++i) estaciones[i] = nullptr;

    // Lee de nuevo y llena el arreglo
    while (getline(archivo, linea)) {
        if (linea.find("HABITACIONES|") == 0) {
            for (int i = 0; i < total_estaciones; ++i) {
                getline(archivo, linea);
                size_t p1 = linea.find('|');
                size_t p2 = linea.find('|', p1 + 1);
                size_t p3 = linea.find('|', p2 + 1);
                int id = stoi(linea.substr(0, p1));
                string nombre = linea.substr(p1 + 1, p2 - p1 - 1);
                string tipo = linea.substr(p2 + 1, p3 - p2 - 1);
                string descripcion = linea.substr(p3 + 1);

                estacion* est = new estacion();
                est->id = id;
                est->nombre = nombre;
                est->tipo = tipo;
                est->descripcion = descripcion;
                est->n1 = est->n2 = est->n3 = nullptr;
                if (estaciones[id] != nullptr) {
                    delete est; // Evita fuga de memoria
                    continue;   // O maneja el error
                }
                estaciones[id] = est;
            }
            break;
        }
    }
    delete[] ids;
    return estaciones;
}

void leerArcos(ifstream& archivo, estacion** estaciones, int max_id, ArbolTernario& arbol) {
    string linea;
    while (getline(archivo, linea)) {
        if (linea.find("ARCOS|") == 0) {
            int total = stoi(linea.substr(linea.find('|') + 1));
            for (int i = 0; i < total; ++i) {
                if (!getline(archivo, linea)) break;
                size_t p1 = linea.find('|');
                int id1 = stoi(linea.substr(0, p1));
                int id2 = stoi(linea.substr(p1 + 1));
                if (id1 < 0 || id1 > max_id || id2 < 0 || id2 > max_id) continue;
                if (estaciones[id1] == nullptr || estaciones[id2] == nullptr) continue;
                arbol.agregar_hijo(estaciones[id1], estaciones[id2]);
            }
            break;
        }
    }
}

enemigo* leerEnemigos(ifstream& archivo, int& total_enemigos) {
    string linea;
    enemigo* enemigos = nullptr;
    while (getline(archivo, linea)) {
        if (linea.find("ENEMIGOS|") == 0) {
            total_enemigos = stoi(linea.substr(linea.find('|') + 1));
            enemigos = new enemigo[total_enemigos];
            for (int i = 0; i < total_enemigos; ++i) {
                if (!getline(archivo, linea)) break;
                size_t p1 = linea.find('|');
                size_t p2 = linea.find('|', p1 + 1);
                size_t p3 = linea.find('|', p2 + 1);
                size_t p4 = linea.find('|', p3 + 1);
                enemigos[i].nombre = linea.substr(0, p1);
                enemigos[i].ataque = stoi(linea.substr(p1 + 1, p2 - p1 - 1));
                enemigos[i].vida = stoi(linea.substr(p2 + 1, p3 - p2 - 1));
                enemigos[i].precision = stof(linea.substr(p3 + 1, p4 - p3 - 1));
                enemigos[i].prob_esq = stof(linea.substr(p4 + 1));
            }
            break;
        }
    }
    return enemigos;
}

evento* leerEventos(ifstream& archivo, int& total_eventos) {
    string linea;
    evento* eventos = nullptr;
    while (getline(archivo, linea)) {
        if (linea.find("EVENTOS|") == 0) {
            total_eventos = stoi(linea.substr(linea.find('|') + 1));
            eventos = new evento[total_eventos];
            for (int i = 0; i < total_eventos; ++i) {
                if (!getline(archivo, linea)) break;
                // Separar por '|'
                size_t p1 = linea.find('|');
                size_t p2 = linea.find('|', p1 + 1);
                size_t p3 = linea.find('|', p2 + 1);
                size_t p4 = linea.find('|', p3 + 1);
                size_t p5 = linea.find('|', p4 + 1);
                size_t p6 = linea.find('|', p5 + 1);
                size_t p7 = linea.find('|', p6 + 1);
                size_t p8 = linea.find('|', p7 + 1);

                eventos[i].nombre = linea.substr(0, p1);
                eventos[i].prob_ev = stof(linea.substr(p1 + 1, p2 - p1 - 1));
                eventos[i].desc_ev = linea.substr(p2 + 1, p3 - p2 - 1);
                eventos[i].opcionA = linea.substr(p3 + 1, p4 - p3 - 1);
                eventos[i].consecA = linea.substr(p4 + 1, p5 - p4 - 1);
                eventos[i].efectoA = linea.substr(p5 + 1, p6 - p5 - 1);
                eventos[i].opcionB = linea.substr(p6 + 1, p7 - p6 - 1);
                eventos[i].consecB = linea.substr(p7 + 1, p8 - p7 - 1);
                eventos[i].efectoB = linea.substr(p8 + 1);
            }
            break;
        }
    }
    return eventos;
}

int main() {
    ifstream archivo("juego.map");
    if (!archivo.is_open()) return 1;

    int max_id = -1;
    int total_estaciones = 0;
    estacion** estaciones = leerEstaciones(archivo, max_id, total_estaciones);

    if (estaciones == nullptr) {
        archivo.close();
        return 1;
    }

    int hay_estacion = 0;
    for (int i = 0; i <= max_id; ++i) {
        if (estaciones[i] != nullptr) hay_estacion = 1;
    }
    if (!hay_estacion) {
        delete[] estaciones;
        estaciones = nullptr;
        archivo.close();
        return 1;
    }

    archivo.close();
    archivo.open("juego.map");
    if (!archivo.is_open()) {
        delete[] estaciones;
        estaciones = nullptr;
        return 1;
    }

    ArbolTernario arbol;
    for (int i = 0; i <= max_id; ++i) {
        if (estaciones[i] != nullptr) {
            arbol.set_raiz(estaciones[i]);
            break;
        }
    }

    leerArcos(archivo, estaciones, max_id, arbol);

    arbol.mostrar_preorden();

    archivo.close();
    if (estaciones != nullptr) {
        delete[] estaciones; // <-- Esto es necesario
        estaciones = nullptr;
    }
    return 0;
}

