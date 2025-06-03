#include "combate.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;

bool combate(jugador* player, cola_enemigo*& frente, cola_enemigo*& fin) {
    srand(time(NULL)); // Solo para pruebas, en el juego real inicializa una vez en main
    while (!cola_vacia(frente) && player->vida > 0) {
        enemigo* enemigo_actual = frente->dato;
        float prob = (float)rand() / RAND_MAX;
        if (prob < player->precision) {
            enemigo_actual->vida -= player->ataque;
            cout << "¡Jugador golpea a " << enemigo_actual->nombre << " por " << player->ataque << " de daño!" << endl;
        } else {
            cout << "¡Jugador falla el ataque!" << endl;
        }
        if (enemigo_actual->vida <= 0) {
            cout << enemigo_actual->nombre << " ha sido derrotado." << endl;
            desencolar(frente, fin);
            continue;
        }
        cola_enemigo* temp = frente;
        while (temp != NULL) {
            float prob_e = (float)rand() / RAND_MAX;
            if (prob_e < temp->dato->precision) {
                player->vida -= temp->dato->ataque;
                cout << temp->dato->nombre << " golpea al Jugador por " << temp->dato->ataque << " de daño!" << endl;
            } else {
                cout << temp->dato->nombre << " falla!" << endl;
            }
            temp = temp->sig;
        }
        cout << "Jugador vida: " << player->vida << endl;
    }
    return player->vida > 0;
}