using namespace std;
#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include "estructuras.hpp"
#include "arbol.hpp"

map<int, estacion*> leerEstaciones(ifstream& archivo) {
    string linea;
    map<int, estacion*> estaciones;
    while (getline(archivo, linea)) {
        if (linea.find("HABITACIONES|") == 0) {
            size_t pos = linea.find('|');
            int total = stoi(linea.substr(pos + 1));
            for (int i = 0; i < total; ++i) {
                if (!getline(archivo, linea)) break;
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
                estaciones[id] = est;
            }
            break;
        }
    }
    return estaciones;
}

estacion** listaEstaciones(ifstream& archivo) {
    string inicio;
    int total_estaciones = 0;
    int max_id = 0;

    // Primero, encuentra cuántas estaciones hay y el id máximo
    streampos pos_archivo = archivo.tellg();
    while (getline(archivo, inicio)) {
        if (inicio.find("HABITACIONES|") == 0) {
            size_t pos = inicio.find('|');
            total_estaciones = stoi(inicio.substr(pos + 1));
            break;
        }
    }
    // Encuentra el id máximo
    int* ids = new int[total_estaciones];
    for (int i = 0; i < total_estaciones; ++i) {
        getline(archivo, inicio);
        size_t pos1 = inicio.find('|');
        int id = stoi(inicio.substr(0, pos1));
        ids[i] = id;
        if (id > max_id) max_id = id;
    }
    // Vuelve al inicio de las estaciones
    archivo.clear();
    archivo.seekg(pos_archivo);

    // Ahora sí, crea el arreglo con tamaño suficiente
    estacion** nodos = new estacion*[max_id + 1];
    for (int i = 0; i <= max_id; ++i) nodos[i] = nullptr;

    // Lee de nuevo y llena el arreglo
    while (getline(archivo, inicio)) {
        if (inicio.find("HABITACIONES|") == 0) {
            for (int i = 0; i < total_estaciones; ++i) {
                getline(archivo, inicio);
                size_t pos1 = inicio.find('|');
                size_t pos2 = inicio.find('|', pos1 + 1);
                size_t pos3 = inicio.find('|', pos2 + 1);
                int id = stoi(inicio.substr(0, pos1));
                string nombre = inicio.substr(pos1 + 1, pos2 - pos1 - 1);
                string tipo = inicio.substr(pos2 + 1, pos3 - pos2 - 1);
                string descripcion = inicio.substr(pos3 + 1);

                estacion* actual = new estacion();
                actual->id = id;
                actual->nombre = nombre;
                actual->tipo = tipo;
                actual->descripcion = descripcion;
                actual->n1 = nullptr;
                actual->n2 = nullptr;
                actual->n3 = nullptr;
                nodos[id] = actual;
            }
            delete[] ids;
            return nodos;
        }
    }
    delete[] ids;
    return nullptr;
}




void leerArcos(ifstream& archivo, map<int, estacion*>& estaciones, ArbolTernario& arbol) {
    string linea;
    while (getline(archivo, linea)) {
        if (linea.find("ARCOS|") == 0) {
            size_t pos = linea.find('|');
            int total = stoi(linea.substr(pos + 1));
            for (int i = 0; i < total; ++i) {
                if (!getline(archivo, linea)) break;
                size_t p1 = linea.find('|');
                int id1 = stoi(linea.substr(0, p1));
                int id2 = stoi(linea.substr(p1 + 1));
                if (estaciones.count(id1) && estaciones.count(id2)) {
                    arbol.agregar_hijo(estaciones[id1], estaciones[id2]);
                } else {
                    cerr << "Error: id de estación no encontrado (" << id1 << " o " << id2 << ")" << endl;
                }
            }
            break;
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
    ifstream archivo("juego.map");
    if (!archivo.is_open()) {
        cout << "Error al abrir el archivo." << endl;
        return 1;
    }

    auto estaciones = leerEstaciones(archivo);

    if (estaciones.empty()) {
        cout << "No se encontró la sección HABITACIONES|" << endl;
        archivo.close();
        return 1;
    }

    archivo.clear();
    archivo.seekg(0, ios::beg);

    ArbolTernario arbol;
    arbol.set_raiz(estaciones.begin()->second); // Usa la estación con menor id como raíz

    leerArcos(archivo, estaciones, arbol);

    cout << "Árbol de estaciones (preorden):" << endl;
    arbol.mostrar_preorden();

    archivo.close();
    return 0;
}

