#include "estructuras.hpp"
#include "arbol.hpp"
#include "leer_mapa.hpp"
#include "cola_enemigos.hpp"
#include "combate.hpp"
#include <iostream>
using namespace std;

// Definición real de la variable global
evento** eventos_globales = NULL;

int main() {
    int total_habitaciones = 0;
    int total_eventos = 0;
    ArbolTernario arbol;
    LectorMapa lector;

    // Usar los métodos de las clases
    estacion** habitaciones = lector.leer_habitaciones("juego.map", total_habitaciones, arbol);
    lector.leer_y_conectar_arcos("juego.map", habitaciones, total_habitaciones, arbol);
    estacion* actual = lector.obtener_raiz(habitaciones);
    arbol.set_raiz(actual);

    jugador player = {30, 7, 0.7, 5};

    // Asigna un evento global aleatorio
    // habitaciones[id]->evento_asociado = eventos_globales[idx];

    // Recorrido principal del juego
    while (actual != NULL && player.vida > 0) {
        cout << "\nEstás en: " << actual->nombre << endl;
        cout << actual->descripcion << endl;

        if (actual->tipo.find("COMBATE") != string::npos) {
            cola_enemigo* frente = NULL;
            cola_enemigo* fin = NULL;
            for (int i = 0; i < actual->cantidad_enemigos; i++) {
                encolar(frente, fin, actual->enemigos[i]);
            }
            bool resultado = combate(&player, frente, fin);
            liberar_cola(frente);
            if (!resultado) {
                cout << "¡Has sido derrotado!" << endl;
                break;
            }

            // --- Lógica de recuperación y mejora tras combate ---
            int vida_recuperada = player.recuperacion;
            player.vida += vida_recuperada;
            cout << "Recuperas " << vida_recuperada << " puntos de vida tras el combate. Vida actual: " << player.vida << endl;

            cout << "Elige una mejora:\n";
            cout << "1. +4 Vida\n";
            cout << "2. +1 Ataque\n";
            cout << "3. +0.1 Precisión\n";
            cout << "4. +1 Recuperación\n";
            int mejora = 0;
            do {
                cout << "Ingresa el número de la mejora: ";
                cin >> mejora;
            } while (mejora < 1 || mejora > 4);

            switch (mejora) {
                case 1:
                    player.vida += 4;
                    cout << "¡Has ganado +4 Vida! Vida actual: " << player.vida << endl;
                    break;
                case 2:
                    player.ataque += 1;
                    cout << "¡Has ganado +1 Ataque! Ataque actual: " << player.ataque << endl;
                    break;
                case 3:
                    player.precision += 0.1;
                    cout << "¡Has ganado +0.1 Precisión! Precisión actual: " << player.precision << endl;
                    break;
                case 4:
                    player.recuperacion += 1;
                    cout << "¡Has ganado +1 Recuperación! Recuperación actual: " << player.recuperacion << endl;
                    break;
            }
        } else if (actual->tipo.find("EVENTO") != string::npos && actual->evento_asociado != NULL) {
            evento* ev = actual->evento_asociado;
            cout << "\n--- EVENTO: " << ev->nombre << " ---" << endl;
            cout << ev->descripcion << endl;
            for (int i = 0; i < ev->cantidad_opciones; i++) {
                cout << (i+1) << ". " << ev->opciones[i].descripcion << endl;
            }
            int eleccion = 0;
            do {
                cout << "Elige una opción (1-" << ev->cantidad_opciones << "): ";
                cin >> eleccion;
            } while (eleccion < 1 || eleccion > ev->cantidad_opciones);

            opcion_evento& op = ev->opciones[eleccion-1];
            player.vida += op.cambio_vida;
            player.ataque += op.cambio_ataque;
            player.precision += op.cambio_precision;
            player.recuperacion += op.cambio_recuperacion;

            cout << "Consecuencia aplicada:" << endl;
            if (op.cambio_vida != 0) cout << "  Vida: " << (op.cambio_vida > 0 ? "+" : "") << op.cambio_vida << endl;
            if (op.cambio_ataque != 0) cout << "  Ataque: " << (op.cambio_ataque > 0 ? "+" : "") << op.cambio_ataque << endl;
            if (op.cambio_precision != 0) cout << "  Precisión: " << (op.cambio_precision > 0 ? "+" : "") << op.cambio_precision << endl;
            if (op.cambio_recuperacion != 0) cout << "  Recuperación: " << (op.cambio_recuperacion > 0 ? "+" : "") << op.cambio_recuperacion << endl;
            cout << "Estado actual: Vida=" << player.vida << ", Ataque=" << player.ataque
                << ", Precisión=" << player.precision << ", Recuperación=" << player.recuperacion << endl;
        }

        // --- Selección de siguiente habitación ---
        int opciones = 0;
        if (actual->n1 != NULL) opciones++;
        if (actual->n2 != NULL) opciones++;
        if (actual->n3 != NULL) opciones++;

        if (opciones == 0) {
            actual = NULL; // Fin del recorrido
        } else if (opciones == 1) {
            if (actual->n1 != NULL) actual = actual->n1;
            else if (actual->n2 != NULL) actual = actual->n2;
            else actual = actual->n3;
        } else {
            cout << "\nElige la siguiente habitación:\n";
            int idx = 1;
            if (actual->n1 != NULL) cout << idx++ << ". " << actual->n1->nombre << endl;
            if (actual->n2 != NULL) cout << idx++ << ". " << actual->n2->nombre << endl;
            if (actual->n3 != NULL) cout << idx++ << ". " << actual->n3->nombre << endl;

            int eleccion = 0;
            do {
                cout << "Ingresa el número de la habitación: ";
                cin >> eleccion;
            } while (eleccion < 1 || eleccion > opciones);

            if (eleccion == 1 && actual->n1 != NULL) actual = actual->n1;
            else if (eleccion == 2 && actual->n2 != NULL) actual = actual->n2;
            else if (eleccion == 3 && actual->n3 != NULL) actual = actual->n3;
            else actual = NULL;
        }
    }

    // Liberar memoria de enemigos y habitaciones
    for (int i = 0; i < total_habitaciones; i++) {
        if (habitaciones[i]->enemigos != NULL) {
            for (int j = 0; j < habitaciones[i]->cantidad_enemigos; j++) {
                delete habitaciones[i]->enemigos[j];
            }
            delete[] habitaciones[i]->enemigos;
        }
        delete habitaciones[i];
    }
    delete[] habitaciones;

    // Libera memoria de eventos globales
    for (int i = 0; i < total_eventos; i++) {
        delete[] eventos_globales[i]->opciones;
        delete eventos_globales[i];
    }
    delete[] eventos_globales;

    if (player.vida > 0 && actual == NULL) {
        cout << "\n¡Felicidades! Has llegado al final del recorrido con vida." << endl;
    } else if (player.vida <= 0) {
        cout << "\nHas muerto. Fin del juego." << endl;
    } else {
        cout << "\nFin del juego." << endl;
    }

    return 0;
}