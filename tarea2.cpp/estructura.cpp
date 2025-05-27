using namespace std;
#include <iostream>
#include <string>
#include <sstream>
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
        n1(NULL), n2(NULL), n3(NULL) {}
        
    //         estacion(int id, string nombre, string descripcion, string tipo){
    //     id = -1;
    //     nombre = _nombre;
    //     descripcion = _descripcion;
    //     tipo = _tipo ;
    //     n1 = NULL;
    //     n2 = NULL;
    //     n3 = NULL;
    // }
};



/*
class arbolTernario {
private:
    estacion* raiz;

public:
    arbolTernario() : raiz(NULL) {}

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


void arbolPreOrden(estacion* nodo, int n) {
    if (nodo == NULL) return;

    for (int i = 0; i < n; ++i) {
        cout << "  ";
    }
    cout << nodo->nombre << endl;

    arbolPreOrden(nodo->n1, n + 1);
    arbolPreOrden(nodo->n2, n + 1);
    arbolPreOrden(nodo->n3, n + 1);
}

void Combate(estacion* nodo) {
    cout << "Has encontrado un enemigo en " << nodo->nombre << endl;
    cout << "¡Prepárate para el combate!" << endl;
    }
*/
int main() {

const int cant = 19;
estacion** nodos = new estacion*[cant];
for (int i = 0; i < cant; ++i) {
    nodos[i] = NULL;
} 

/*
    // Crear nodos
    nodos[0] = new estacion(0, "Puente Cal y Canto", "Eres un superviviente de una explosión nuclear, milagrosamente te encontrabas a salvo en las profundidades de la estación Cal y Canto, ansias reencontrarte con tu familia (esposa e hija) y saber si están vivas. Te lanzas resuelto en su búsqueda teniendo en cuenta que su hogar era próximo a la estación Ñuñoa.", "INICIO");
    nodos[1] = new estacion(1, "Linea 2", "Transitas por la boletería para acceder a las vías del metro, cuando visualizas a lo lejos un tumulto acercándote.", "COMBATE");
    nodos[2]  = new estacion(2, "Santa Ana", "Encuentras una oficina de seguridad abierta con recursos en su interior. ", "EVENTO");
    nodos[3] = new estacion(3, "Plaza de Armas", "Ves una fogata entre la oscuridad de un andén, te acercas y te embiste algo. ", "COMBATE");
    nodos[4] = new estacion(4, "Baquedano", "Contemplas uno de los cuadros decorativos de la estación, de repente escuchas gruñidos y sonidos metálicos. ", "COMBATE");
    nodos[5] = new estacion(5, "Irarrazabal", "Logras salir brevemente a las afueras del metro observando un panorama desolador...", "EVENTO");
    nodos[6] = new estacion(6, "Ñuñoa", "Finalmente hayas a tu familia, están vivas y en buen estado. Vivian en un refugio a salvo, vas con ellas al refugio. ", "FIN");
    nodos[7] = new estacion(7, "Plaza Egaña", "Te empiezas a sentir mareado y confundido, hueles algo extraño y te desplomas. Mueres a causa de un gas toxico que se filtraba a la estación desde el exterior. ", "FIN");
    nodos[8] = new estacion(8, "Ñuble", "Estas exhausto, pierdes fuerzas y ya no quieres continuar. Te rindes y mueres.  ", "FIN");
    nodos[9] = new estacion(9, "Tobalaba", "Luego de ayudar a la familia, te quedas sin recursos y no tienes nada con que defenderte. Morirás a los peligros que te asechan sin poder defenderte. ", "FIN");
    nodos[10] = new estacion(10, "Bellas Artes", "Ves la estación destrozada y en medio de ella 2 botellas con algo que parece agua. Piensas en cual beber...  ", "FIN");
    nodos[11] = new estacion(11, "Cuming", "Sientes la tierra moverse bruscamente. La tierra se abre y te traga. ", "FIN");
    nodos[12] = new estacion(12, "Patronato", ".La estación esta llena de supervivientes en pánico y desquiciados. Estas en medio del mar tumultuoso de gente. Mueres aplastado. ", "FIN");
    nodos[13] = new estacion(13, "Linea 3", "Te encuentras con un grupo de supervivientes que quieren robar tus recursos. De repente algo entre las sombras con tentáculos los agarra y arrastra a la oscuridad. Estas en alerta y sientes algo que se acerca...", "COMBATE");
    nodos[14] = new estacion(14, "Parque Almagro","Sientes algo jadear que te asecha, giras en una esquina para sorprenderlo, esperas en guardia para atacarlo... cada vez más cerca de ti... ¡ES UN PERRO!", "EVENTO");
    nodos[15] = new estacion(15, "Pajaritos","Te diriges a la terminal pajaritos pensando en que alguien habrá abandonado su equipaje entre tanto caos. No te equivocaste... ","COMBATE");
    nodos[16] = new estacion(16, "Estacion Central","Ves una farmacia saqueada al interior de la estación. Te encaminas a ver si queda algo... ","COMBATE");



    arbolTernario arbol;
    arbol.asignarRaiz(nodos[0]);

    // Conexiones entre nodos
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
    arbol.insertarNodo(nodos[13], nodos[5]);
    arbol.insertarNodo(nodos[14], nodos[16]);
    arbol.insertarNodo(nodos[14], nodos[15]);





    arbolPreOrden(arbol.obtenRaiz(), 0);

    /BONUS
    arbol.insertarNodo(nodos[13], nodos[5]);
    arbol.insertarNodo(nodos[15], nodos[4]);
    arbol.insertarNodo(nodos[16], nodos[4]);*/
// Ya tienes tus nodos creados previamente...

/* Declaramos el puntero "actual" e inicializamos con la raíz
estacion* actual = nodos[0];

int opcion; 

do {

    cout << "********************************" << endl;
    cout << "Estás en: " << actual->nombre << endl;
    cout << actual->descripcion << endl;
    cout << "********************************" << endl;

    // Mostrar opciones de avance
    if (actual->n1) cout << "1. Ir a " << actual->n1->nombre << endl;
    if (actual->n2) cout << "2. Ir a " << actual->n2->nombre << endl;
    if (actual->n3) cout << "3. Ir a " << actual->n3->nombre << endl;

    // Obtener decisión
    cout << "Elige tu opción: ";
    cin >> opcion;

    switch (opcion) {
        case 1:
            if (actual->n1) actual = actual->n1;
            else { cout << "Fin del juego :c" << endl;
            return 0; }
            break;
        case 2:
            if (actual->n2) actual = actual->n2;
            else { cout << "Fin del juego :c" << endl;
            return 0; }
            break;
        case 3:
            if (actual->n3) actual = actual->n3;
            else { cout << "Fin del juego :c" << endl; 
            return 0; }
            break;
        default:
            cout << "Fin del juego :c" << endl;
            return 0;
    }

    if (actual->tipo == "FIN") {
        cout << "********************************" << endl;
        cout << "Has llegado a un final: " << actual->nombre << endl;
        cout << actual->descripcion << endl;
        cout << "********************************" << endl;
        break;
    }

} while (true);*/

ifstream listaEstaciones("juego.txt");
cout << "********************************" << endl;

    return 0;
}

