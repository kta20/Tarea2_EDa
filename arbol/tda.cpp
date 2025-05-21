#include <iostream>
#include <string>

using namespace std;

// -----------------------------
// Estructura del nodo del árbol
// -----------------------------
struct Nodo {
    int id;
    string nombre;
    Nodo* hijo1;
    Nodo* hijo2;
    Nodo* hijo3;

    Nodo(int id, string nombre)
        : id(id), nombre(nombre), hijo1(nullptr), hijo2(nullptr), hijo3(nullptr) {}
};

// -----------------------------
// Clase Árbol Ternario
// -----------------------------
class ArbolTernario {
private:
    Nodo* raiz;

    void imprimir(Nodo* nodo, int nivel) {
        if (!nodo) return;

        for (int i = 0; i < nivel; ++i) cout << "  ";
        cout << "- " << nodo->nombre << " (ID: " << nodo->id << ")" << endl;

        imprimir(nodo->hijo1, nivel + 1);
        imprimir(nodo->hijo2, nivel + 1);
        imprimir(nodo->hijo3, nivel + 1);
    }

    void liberar(Nodo* nodo) {
        if (!nodo) return;
        liberar(nodo->hijo1);
        liberar(nodo->hijo2);
        liberar(nodo->hijo3);
        delete nodo;
    }

    Nodo* buscar(Nodo* nodo, int idBuscado) {
        if (!nodo) return nullptr;
        if (nodo->id == idBuscado) return nodo;

        Nodo* resultado = buscar(nodo->hijo1, idBuscado);
        if (resultado) return resultado;
        resultado = buscar(nodo->hijo2, idBuscado);
        if (resultado) return resultado;
        return buscar(nodo->hijo3, idBuscado);
    }

public:
    ArbolTernario() : raiz(nullptr) {}

    ~ArbolTernario() {
        liberar(raiz);
    }

    void setRaiz(int id, string nombre) {
        raiz = new Nodo(id, nombre);
    }

    bool insertar(int padreID, int hijoID, string nombre) {
        Nodo* padre = buscar(raiz, padreID);
        if (!padre) return false;

        Nodo* nuevo = new Nodo(hijoID, nombre);
        if (!padre->hijo1) padre->hijo1 = nuevo;
        else if (!padre->hijo2) padre->hijo2 = nuevo;
        else if (!padre->hijo3) padre->hijo3 = nuevo;
        else {
            delete nuevo;
            return false; // No hay espacio disponible
        }
        return true;
    }

    void imprimir() {
        imprimir(raiz, 0);
    }
};

// -----------------------------
// Función principal
// -----------------------------
int main() {
    ArbolTernario arbol;

    arbol.setRaiz(0, "Lugares");
    arbol.insertar(0, 1, "Playa");
    arbol.insertar(0, 2, "Bosque");
    arbol.insertar(0, 3, "Montaña");

    arbol.insertar(1, 4, "Olas");
    arbol.insertar(1, 5, "Corales");
    arbol.insertar(1, 6, "Rocas");

    arbol.insertar(2, 7, "Pantano");
    arbol.insertar(2, 8, "Selva");
    arbol.insertar(2, 9, "Cueva");


    arbol.insertar(3, 10, "Montañas nevadas");
    arbol.insertar(3, 11, "Glaciares");
    arbol.insertar(3, 12, "Río helado");


    cout << "Árbol Ternario:\n";
    arbol.imprimir();

    return 0;
}
