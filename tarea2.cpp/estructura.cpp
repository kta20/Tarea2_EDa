using namespace std;
#include <iostream>
#include <string>



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

class arbolTernario {
private:
    estacion* raiz;

public:
    arbolTernario() : raiz(nullptr) {}

    void asignarRaiz(estacion* nodo) {
        raiz = nodo;
    }

    estacion* obtenRaiz() {
        return raiz;
    }

    void insertarNodo(estacion* padre, estacion* hijo) {
        if (!padre->n1) padre->n1 = hijo;
        else if (!padre->n2) padre->n2 = hijo;
        else if (!padre->n3) padre->n3 = hijo;
        else {
            cout<< padre->id << " ya tiene 3 hijos.\n";
        }
    }
};

int main() {
    cout << "Creando el árbol ternario...\n";

    return 0;
}
