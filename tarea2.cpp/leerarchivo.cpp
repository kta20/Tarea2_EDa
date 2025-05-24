using namespace std;
#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>



/*
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
        n1(NULL), n2(NULL), n3(NULL) {
};
    }
estacion** listaEstaciones(ifstream& archivo) {
    string inicio, caracter,nombreEstacion; 
    int total_estaciones , i;
    while (getline(archivo, inicio)) {
        if (inicio.find("HABITACIONES|") == 0) {
            size_t pos = inicio.find('|');
            total_estaciones = stoi(inicio.substr(pos + 1));
            cout << "Total de estaciones: " << total_estaciones << endl;


        }
            }

    }
            





using namespace std;

int main() {
    fstream archivo("juego.txt");  // abrir archivo

    if (!archivo.is_open()) {
        cout << "Error al abrir el archivo." << endl;
        return 1;  // termina el programa con error
    }

    // Aquí puedes continuar con la lectura y procesamiento del archivo

    archivo.close();  // cerrar archivo al final

    return 0;  // todo bien
}*/


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

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
        n1(nullptr), n2(nullptr), n3(nullptr) {}
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

                size_t pos1 = inicio.find('|');
                size_t pos2 = inicio.find('|', pos1 + 1);
                size_t pos3 = inicio.find('|', pos2 + 1);
                int id = stoi(inicio.substr(0, pos1));


                string nombre = inicio.substr(pos1 + 1, pos2 - pos1 - 1);
                string tipo = inicio.substr(pos2 + 1, pos3 - pos2 - 1);
                string descripcion = inicio.substr(pos3 + 1);

                nodos[i] = new estacion(id, nombre, descripcion, tipo);
            }

            cout << "ID primera estación: " << nodos[0]->id << endl;
            cout << "Nombre primera estación: " << nodos[0]->nombre << endl;
            cout << "Descripción primera estación: " << nodos[0]->descripcion << endl;
            cout << "Tipo primera estación: " << nodos[0]->tipo << endl;

            return nodos;
        }
    }
    return nullptr;
}



int main() {
    ifstream archivo("juego.txt");  // abrir archivo en modo lectura

    if (!archivo.is_open()) {
        cout << "Error al abrir el archivo." << endl;
        return 1;  // termina el programa con error
    }

    estacion** estaciones = listaEstaciones(archivo);

    if (estaciones == nullptr) {
        cout << "No se encontró la sección HABITACIONES|" << endl;
    }

    archivo.close();

    return 0;
}
