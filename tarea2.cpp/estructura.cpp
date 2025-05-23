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
const int cant = 19;
estacion** nodos = new estacion*[cant];
for (int i = 0; i < cant; ++i) {
    nodos[i] = nullptr;
} 

    // Crear habitaciones (nodos)
    nodos[0] = new estacion(0, "Puente Cal y Canto", "descripcion", "INICIO");
    nodos[1] = new estacion(1, "Linea 2", "descripcion", "COMBATE");
    nodos[2]  = new estacion(2, "Santa Ana", "..", "EVENTO");
    nodos[3] = new estacion(3, "Plaza de Armas", "..", "COMBATE");
    nodos[4] = new estacion(4, "Baquedano", "..", "COMBATE");
    nodos[5] = new estacion(5, "Irarrazabal", "..", "EVENTO");
    nodos[6] = new estacion(6, "Ñuñoa", "..", "FIN");
    nodos[7] = new estacion(7, "Plaza Egaña", "..", "FIN");
    nodos[8] = new estacion(8, "Ñuble", "..", "FIN");
    nodos[9] = new estacion(9, "Tobalaba", "..", "FIN");
    nodos[10] = new estacion(10, "Bellas Artes", "..", "FIN");
    nodos[11] = new estacion(11, "Cuming", "..", "FIN");
    nodos[12] = new estacion(12, "Patronato", "..", "FIN");
    nodos[13] = new estacion(13, "Linea 3", "..", "COMBATE");
    nodos[14] = new estacion(14, "Irarrazabal","..", "EVENTO");
    nodos[15] = new estacion(15, "Plaza de Armas","..", "EVENTO");
    nodos[16] = new estacion(16, "Pajaritos","..","COMBATE");
    nodos[17] = new estacion(17, "Estacion Central","..","COMBATE");
    nodos[18] = new estacion(18, "Baquedano","..","EVENTO");



    arbolTernario arbol;
    arbol.asignarRaiz(nodos[0]);

    // Conexiones actualizadas
    arbol.insertarNodo(nodos[0], nodos[1]);
    arbol.insertarNodo(nodos[0], nodos[13]);
    arbol.insertarNodo(nodos[1], nodos[2]);
    arbol.insertarNodo(nodos[1], nodos[12]);
    arbol.insertarNodo(nodos[2], nodos[3]);
    arbol.insertarNodo(nodos[2], nodos[11]);
    arbol.insertarNodo(nodos[3], nodos[4]);
    arbol.insertarNodo(nodos[3], nodos[10]);
    arbol.insertarNodo(nodos[4], nodos[5]);
    arbol.insertarNodo(nodos[4], nodos[9]);
    arbol.insertarNodo(nodos[5], nodos[6]);
    arbol.insertarNodo(nodos[5], nodos[7]);
    arbol.insertarNodo(nodos[5], nodos[8]);
    arbol.insertarNodo(nodos[13], nodos[14]);
    arbol.insertarNodo(nodos[13], nodos[15]);
    arbol.insertarNodo(nodos[15], nodos[16]);
    arbol.insertarNodo(nodos[15], nodos[17]);
    arbol.insertarNodo(nodos[17], nodos[18]);
    arbol.insertarNodo(nodos[16], nodos[18]);

    cout << "Árbol cargado con 19 estaciones." << endl;

    return 0;
}
