using namespace std;
#include <iostream>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>



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

estacion** leerEnemigos(ifstream& archivo) {
    string inicio;
    int total_enemigos=0;
    float vida, ataque, descripcion;
        while (getline(archivo, inicio)) {
        if (inicio.find("ENEMIGOS|") == 0) {
            size_t pos = inicio.find('|');
            total_enemigos = stoi(inicio.substr(pos + 1));
            estacion** nodos = new estacion*[total_enemigos];
            for (int i = 0; i < total_enemigos; ++i) {
                if (!getline(archivo, inicio)) {
                    break;
            size_t pos1 = inicio.find('|');
            size_t pos2 = inicio.find('|', pos1 + 1);
            size_t pos3 = inicio.find('|', pos2 + 1);
            size_t pos4 = inicio.find('|', pos3 + 1);

            string enemigo = inicio.substr(0, pos1);
            float vida = stoi(inicio.substr(pos1 + 1, pos2 - pos1 - 1));
            float ataque = stoi(inicio.substr(pos2 + 1, pos3 - pos2 - 1));
            float precision = stoi(inicio.substr(pos3 + 1, pos4 - pos3 - 1));
            float probabilidad = stoi(inicio.substr(pos4 + 1));

            estacion* actual = new estacion(0, enemigo, "", "");

        }
    }
}
        }
    };