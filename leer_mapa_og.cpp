#include "estructuras.hpp"
#include "arbol.cpp"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

using namespace std;

// lee las habitaciones desde el archivo y retorna un arreglo de punteros a estacion
estacion** leer_habitaciones(string filename, int& total_habitaciones) {
    ifstream archivo;
    archivo.open(filename.c_str());
    if (!archivo.is_open()) {
        cout << "No se ha podido abrir el archivo\n";
        total_habitaciones = 0;
        return NULL;
    }

    string linea;
    estacion** habitaciones = NULL;

    while (getline(archivo, linea)) {
        if (linea.find("HABITACIONES|") == 0) {
            int pos = linea.find('|');
            total_habitaciones = stoi(linea.substr(pos + 1));
            habitaciones = new estacion*[total_habitaciones];
            for (int i = 0; i < total_habitaciones; ++i) {
                getline(archivo, linea);
                int pos1 = linea.find('|');
                int pos2 = linea.find('|', pos1 + 1);
                int pos3 = linea.find('|', pos2 + 1);
                string id_str = linea.substr(0, pos1);
                string nombre = linea.substr(pos1 + 1, pos2 - pos1 - 1);
                string tipo = linea.substr(pos2 + 1, pos3 - pos2 - 1);
                string descripcion = linea.substr(pos3 + 1);
                int id = stoi(id_str);
                habitaciones[id] = crear_estacion(id, nombre, tipo, descripcion);
            }
            break; // Ya leíste las habitaciones
        }
    }
    archivo.close();
    return habitaciones;
}

// Lee los arcos desde el archivo y conecta las habitaciones
void leer_y_conectar_arcos(string filename, estacion** habitaciones, int total_habitaciones) {
    ifstream archivo;
    archivo.open(filename.c_str());
    if (!archivo.is_open()) {
        cout << "No se ha podido abrir el archivo para leer arcos\n";
        return;
    }

    string linea;
    // Buscar la sección ARCOS
    while (getline(archivo, linea)) {
        if (linea.find("ARCOS|") == 0) {
            int pos = linea.find('|');
            int total_arcos = stoi(linea.substr(pos + 1));
            for (int i = 0; i < total_arcos; ++i) {
                getline(archivo, linea);
                int pos_sep = linea.find('|');
                string origen_str = linea.substr(0, pos_sep);
                string destino_str = linea.substr(pos_sep + 1);
                int id_origen = stoi(origen_str);
                int id_destino = stoi(destino_str);
                agregar_hijo(habitaciones[id_origen], habitaciones[id_destino]);
            }
            break; // Ya leíste los arcos
        }
    }
    archivo.close();
}

// Devuelve la raíz del árbol (habitaciones[0])
estacion* obtener_raiz(estacion** habitaciones) {
    return habitaciones[0];
}