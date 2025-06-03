#include "estructuras.hpp"
#include "cola_enemigos.hpp"
#include "combate.hpp"
#include <iostream>
using namespace std;

int main() {
    jugador player = {30, 7, 0.7, 5};

    enemigo* e1 = new enemigo{"Pim", 10, 2, 0.8, 1.0};
    enemigo* e2 = new enemigo{"Pomplin", 12, 1, 0.5, 1.0};

    cola_enemigo* frente = NULL;
    cola_enemigo* fin = NULL;
    encolar(frente, fin, e1);
    encolar(frente, fin, e2);

    bool resultado = combate(&player, frente, fin);

    if (resultado)
        cout << "¡El jugador sobrevivió al combate!" << endl;
    else
        cout << "El jugador ha sido derrotado." << endl;

    liberar_cola(frente);
    delete e1;
    delete e2;
    return 0;
}