#include "estructuras.hpp" 
#include "arbol.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

const unsigned NO_ENCONTRADO = (unsigned)-1;

//almacena partes de un string en un arreglo. util para almacenar la info de cada linea (eventos, estaciones...)
void split(const string& texto, char separador, string partes[], int max_partes, int& cantidad_partes) {
    unsigned pos_in = 0;
    cantidad_partes = 0;
    unsigned pos_separador = texto.find(separador, pos_in);

    while (pos_separador != NO_ENCONTRADO && cantidad_partes < max_partes - 1) {
        partes[cantidad_partes] = texto.substr(pos_in, pos_separador - pos_in);
        cantidad_partes++;
        pos_in = pos_separador + 1;
        pos_separador = texto.find(separador, pos_in);
    }

    partes[cantidad_partes] = texto.substr(pos_in);
    cantidad_partes++;
}

void liberar_enemigos(enemigo** enemigos, int total) {
    if (!enemigos) return;
    for (int i = 0; i < total; ++i)
        if (enemigos[i]) delete enemigos[i];
    delete[] enemigos;
}

void liberar_eventos(evento** eventos, int total) {
    if (!eventos) return;
    for (int i = 0; i < total; ++i) {
        if (eventos[i]) {
            if (eventos[i]->opciones) delete[] eventos[i]->opciones;
            delete eventos[i];
        }
    }
    delete[] eventos;
}

void liberar_habitaciones(estacion** habitaciones, int total_habitaciones) {
    if (!habitaciones) return;
    // NO hagas: for (int i = 0; i < total_habitaciones; ++i) delete habitaciones[i];
    delete[] habitaciones;
}

int seleccionar_por_probabilidad(float* probs, int total) {
    float r = (float)rand() / RAND_MAX;
    float suma = 0;
    for (int i = 0; i < total; ++i) {
        suma += probs[i];
        if (r <= suma) return i;
    }
    return total - 1;
}

enemigo** leer_enemigos_globales_impl(string filename, int& total_enemigos) {
    ifstream archivo(filename.c_str());
    string linea;
    enemigo** enemigos = NULL;
    while (getline(archivo, linea)) {
        if (linea.find("ENEMIGOS|") == 0) {
            int pos = linea.find('|');
            total_enemigos = stoi(linea.substr(pos + 1));
            enemigos = new enemigo*[total_enemigos];
            for (int i = 0; i < total_enemigos; ++i) {
                getline(archivo, linea);
                string partes[5];
                int n_partes = 0;
                split(linea, '|', partes, 5, n_partes);
                string nombre = partes[0];
                int vida = stoi(partes[1]);
                int ataque = stoi(partes[2]);
                float precision = stof(partes[3]);
                float prob_spawn = stof(partes[4]);
                enemigos[i] = new enemigo{nombre, vida, ataque, precision, prob_spawn};
            }
            break;
        }
    }
    archivo.close();
    return enemigos;
}

// Lee todos los eventos definidos en el archivo y retorna un arreglo
evento** leer_eventos_globales_impl(string filename, int& total_eventos) {
    ifstream archivo(filename.c_str());
    string linea;
    evento** eventos = NULL;
    while (getline(archivo, linea)) {
        if (linea.find("EVENTOS|") == 0) {
            int pos = linea.find('|');
            total_eventos = stoi(linea.substr(pos + 1));
            eventos = new evento*[total_eventos];
            int evento_al_lei = 0;
            while (evento_al_lei < total_eventos && getline(archivo, linea)) {
                if (linea.empty() || linea == "&") continue;
                string partes[9];
                int n_partes = 0;
                split(linea, '|', partes, 9, n_partes);
                for (int i = n_partes; i < 9; ++i) partes[i] = "";
                evento* ev = new evento;
                ev->nombre = partes[0];
                // Probabilidad
                size_t pos_prob = partes[1].find(' ');
                ev->probabilidad = (pos_prob != string::npos) ? stof(partes[1].substr(pos_prob + 1)) : 0.5;
                ev->descripcion = partes[2];

                ev->opciones = new opcion_evento[2];
                ev->cantidad_opciones = 2;

                // Opción A
                ev->opciones[0].descripcion = (partes[3].size() > 3 ? partes[3].substr(3) : partes[3]);
                if (!partes[4].empty())
                    ev->opciones[0].descripcion += " (" + partes[4] + ")";
                else
                    ev->opciones[0].descripcion += " (Sin consecuencia)";
                ev->opciones[0].cambio_vida = 0;
                ev->opciones[0].cambio_ataque = 0;
                ev->opciones[0].cambio_precision = 0;
                ev->opciones[0].cambio_recuperacion = 0;
                if (partes[5].find("Vida:") != string::npos)
                    ev->opciones[0].cambio_vida = stoi(partes[5].substr(partes[5].find("Vida:") + 5));
                if (partes[5].find("Ataque:") != string::npos)
                    ev->opciones[0].cambio_ataque = stoi(partes[5].substr(partes[5].find("Ataque:") + 7));
                if (partes[5].find("Precision:") != string::npos)
                    ev->opciones[0].cambio_precision = stof(partes[5].substr(partes[5].find("Precision:") + 10));
                if (partes[5].find("Recuperacion:") != string::npos)
                    ev->opciones[0].cambio_recuperacion = stoi(partes[5].substr(partes[5].find("Recuperacion:") + 13));

                // Opción B
                ev->opciones[1].descripcion = (partes[6].size() > 3 ? partes[6].substr(3) : partes[6]);
                if (!partes[7].empty())
                    ev->opciones[1].descripcion += " (" + partes[7] + ")";
                else
                    ev->opciones[1].descripcion += " (Sin consecuencia)";
                ev->opciones[1].cambio_vida = 0;
                ev->opciones[1].cambio_ataque = 0;
                ev->opciones[1].cambio_precision = 0;
                ev->opciones[1].cambio_recuperacion = 0;
                if (partes[8].find("Vida:") != string::npos)
                    ev->opciones[1].cambio_vida = stoi(partes[8].substr(partes[8].find("Vida:") + 5));
                if (partes[8].find("Ataque:") != string::npos)
                    ev->opciones[1].cambio_ataque = stoi(partes[8].substr(partes[8].find("Ataque:") + 7));
                if (partes[8].find("Precision:") != string::npos)
                    ev->opciones[1].cambio_precision = stof(partes[8].substr(partes[8].find("Precision:") + 10));
                if (partes[8].find("Recuperacion:") != string::npos)
                    ev->opciones[1].cambio_recuperacion = stoi(partes[8].substr(partes[8].find("Recuperacion:") + 13));

                eventos[evento_al_lei++] = ev;
            }
            break;
        }
    }
    archivo.close();
    return eventos;
}

// Lee las habitaciones y las llena con enemigos/eventos
estacion** leer_habitaciones(string filename, int& total_habitaciones, ArbolTernario& arbol) {
    ifstream archivo;
    archivo.open(filename.c_str());
    if (!archivo.is_open()) {
        total_habitaciones = 0;
        return NULL;
    }
    string linea;
    estacion** habitaciones = NULL;

    int total_enemigos = 0;
    enemigo** enemigos_globales = leer_enemigos_globales_impl(filename, total_enemigos);
    int total_eventos = 0;
    evento** eventos_globales = leer_eventos_globales_impl(filename, total_eventos);
    srand(time(NULL)); 

    while (getline(archivo, linea)) {
        if (linea.find("HABITACIONES|") == 0) {
            int pos = linea.find('|');
            total_habitaciones = stoi(linea.substr(pos + 1));
            habitaciones = new estacion*[total_habitaciones];
            for (int i = 0; i < total_habitaciones; ++i) {
                if (!getline(archivo, linea)) {
                    habitaciones[i] = NULL;
                    continue;
                }
                string partes[4];
                int n_partes = 0;
                split(linea, '|', partes, 4, n_partes);
                if (n_partes < 4) {
                    habitaciones[i] = NULL;
                    continue;
                }
                int id = stoi(partes[0]);
                if (id < 0 || id >= total_habitaciones) {
                  //  cout << "ERROR: id fuera de rango: " << id << endl;
                    habitaciones[i] = NULL;
                    continue;
                }
                string nombre = partes[1];
                string tipo = partes[2];
                string descripcion = partes[3];
                habitaciones[id] = arbol.crear_estacion(id, nombre, tipo, descripcion);

                // EVENTOS
                if (tipo.find("EVENTO") != string::npos && eventos_globales && total_eventos > 0) {
                    float r = (float)rand() / RAND_MAX;
                    float suma = 0;
                    int j = 0;
                    for (int e = 0; e < total_eventos; ++e) {
                        suma += eventos_globales[e]->probabilidad;
                        if (r <= suma) {
                            j = e;
                            break;
                        }
                    }
                    if (suma < 1.0 && r > suma) j = total_eventos - 1; // <-- Asegura que idx siempre sea válido
                    // Validar evento antes de asignar
                    if (eventos_globales[j] && eventos_globales[j]->opciones != NULL && eventos_globales[j]->cantidad_opciones >= 2) {
                        habitaciones[id]->evento_asociado = eventos_globales[j];
                        habitaciones[id]->evento_dinamico = false;
                    } else {
                        habitaciones[id]->evento_asociado = NULL;
                    }
                } else {
                    habitaciones[id]->evento_asociado = NULL;
                }

                // COMBATE
                // Forzar combate en estaciones específicas
                bool forzar_combate = (
                    nombre == "Linea 2" ||
                    nombre == "Linea 3" ||
                    nombre == "Plaza de Armas" ||
                    nombre == "Baquedano" ||
                    nombre == "Pajaritos"
                );

                if ((tipo.find("COMBATE") != string::npos && enemigos_globales && total_enemigos > 0) || forzar_combate) {
                    // Cantidad aleatoria entre 1 y 3 si es forzado, o 1-2 si no
                    int cant = forzar_combate ? (1 + rand() % 3) : (1 + rand() % 2);
                    habitaciones[id]->cantidad_enemigos = cant;
                    habitaciones[id]->enemigos = new enemigo*[cant];
                    for (int j = 0; j < cant; ++j) {
                        float probs_enemigos[total_enemigos];
                        for (int k = 0; k < total_enemigos; ++k)
                            probs_enemigos[k] = enemigos_globales[k]->prob_spawn;
                        int idx_enemigo = seleccionar_por_probabilidad(probs_enemigos, total_enemigos);
                        habitaciones[id]->enemigos[j] = new enemigo(*enemigos_globales[idx_enemigo]);
                    }
                } else {
                    habitaciones[id]->cantidad_enemigos = 0;
                    habitaciones[id]->enemigos = NULL;
                }
            }
            break; 
        }
    }
    archivo.close();

    // NO liberes enemigos_globales aquí
    return habitaciones;
}

// Lee los arcos desde el archivo y conecta las habitaciones
void leer_arcos(string filename, estacion** habitaciones, int total_habitaciones, ArbolTernario& arbol) {
    ifstream archivo;
    archivo.open(filename.c_str());
    if (!archivo.is_open()) {
        return;
    }
    string linea;
    while (getline(archivo, linea)) {
        if (linea.find("ARCOS|") == 0) {
            int pos = linea.find('|');
            if (pos == -1) {
                return;
            }
            string arcos_str = linea.substr(pos + 1);
            arcos_str.erase(0, arcos_str.find_first_not_of(" \t\r\n"));
            int total_arcos = stoi(arcos_str);
            for (int i = 0; i < total_arcos; ++i) {
                getline(archivo, linea);
                string partes[2];
                int n_partes = 0;
                split(linea, '|', partes, 2, n_partes);
                int id_origen = stoi(partes[0]);
                int id_destino = stoi(partes[1]);
                arbol.agregar_hijo(habitaciones[id_origen], habitaciones[id_destino]);
            }
            break;
        }
    }
    archivo.close();
}

estacion* obtener_raiz(estacion** habitaciones) {
    return habitaciones[0];
}

// Selecciona un enemigo aleatorio según probabilidad de aparición
enemigo* enemigo_aleatorio(enemigo** enemigos, int total_enemigos) {
    float r = (float)rand() / RAND_MAX;
    float suma = 0;
    for (int i = 0; i < total_enemigos; ++i) {
        suma += enemigos[i]->prob_spawn;
        if (r <= suma) return enemigos[i];
    }
    return enemigos[total_enemigos-1];
}

// Combate interactivo
bool combate(jugador* player, enemigo** enemigos, int total_enemigos) {
    enemigo enemigo_actual = *enemigo_aleatorio(enemigos, total_enemigos);
    cout << "¡Te enfrentas a " << enemigo_actual.nombre << "!" << endl;

    while (player->vida > 0 && enemigo_actual.vida > 0) {
        cout << "\nTu vida: " << player->vida << " | Vida de " << enemigo_actual.nombre << ": " << enemigo_actual.vida << endl;
        cout << "Elige tipo de golpe:\n";
        cout << "1. Golpe fuerte (más daño, menos probabilidad de acierto)\n";
        cout << "2. Golpe regular (menos daño, más probabilidad de acierto)\n";
        int tipo_golpe = 0;
        do {
            cout << "Opción (1-2): ";
            cin >> tipo_golpe;
        } while (tipo_golpe != 1 && tipo_golpe != 2);

        int fuerza = (tipo_golpe == 1) ? player->ataque : player->ataque / 2;
        float prob_crit = (tipo_golpe == 1) ? player->precision * 0.6f : player->precision;

        float prob = (float)rand() / RAND_MAX;
        bool acierto_jugador = (prob < prob_crit);

        if (acierto_jugador) {
            enemigo_actual.vida -= fuerza;
            if (enemigo_actual.vida < 0) enemigo_actual.vida = 0;
            cout << "¡Golpeaste a " << enemigo_actual.nombre << " por " << fuerza << " de daño!" << endl;
        } else {
            cout << "¡Fallaste el golpe!" << endl;
        }

        if (enemigo_actual.vida <= 0) break;

        // Turno del enemigo
        float prob_e = (float)rand() / RAND_MAX;
        bool acierto_enemigo = (prob_e < enemigo_actual.precision);
        int fuerza_enemigo = enemigo_actual.ataque;

        if (acierto_enemigo) {
            player->vida -= fuerza_enemigo;
            if (player->vida < 0) player->vida = 0;
            cout << enemigo_actual.nombre << " te golpea por " << fuerza_enemigo << " de daño!" << endl;
        } else {
            cout << enemigo_actual.nombre << " falla su ataque!" << endl;
        }
    }

    float porcentaje = (float)player->vida / 100.0f * 100.0f;
    cout << "\nPorcentaje de vida restante: " << porcentaje << "%" << endl;

    return player->vida > 0;
}


bonus* leer_bonus(string filename, int& total_bonus) {
    ifstream archivo(filename.c_str());
    string linea;
    bonus* bonus_arr = NULL;
    while (getline(archivo, linea)) {
        if (linea.find("BONUS|") == 0) {
            int pos = linea.find('|');
            total_bonus = stoi(linea.substr(pos + 1));
            bonus_arr = new bonus[total_bonus];
            for (int i = 0; i < total_bonus; ++i) {
                getline(archivo, linea);
                string partes[4];
                int n_partes = 0;
                split(linea, '|', partes, 4, n_partes);
                bonus_arr[i].nombre = partes[0];
                bonus_arr[i].id_origen = stoi(partes[1]);
                bonus_arr[i].accion = partes[2];
                bonus_arr[i].desc_post = partes[3];
            }
            break;
        }
    }
    archivo.close();
    return bonus_arr;
}

void aplicar_mejora(jugador& player) {
    cout << "\nElige una mejora:\n";
    cout << "1. +10 Vida\n";
    cout << "2. +2 Ataque\n";
    cout << "3. +0.1 Precisión\n";
    cout << "4. +1 Recuperación\n";
    int mejora = 0;
    do {
        cout << "Ingresa el número de la mejora: ";
        if (!(cin >> mejora)) {
            cin.clear();
            cin.ignore(10000, '\n');
            mejora = 0;
        }
    } while (mejora < 1 || mejora > 4);

    switch (mejora) {
        case 1:
            player.vida += 10;
            cout << "¡Has ganado +10 Vida! Vida actual: " << player.vida << endl;
            break;
        case 2:
            player.ataque += 2;
            cout << "¡Has ganado +2 Ataque! Ataque actual: " << player.ataque << endl;
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
}

int main() {
    string archivo_mapa = "juego.map";
    ArbolTernario arbol;
    estacion** habitaciones = NULL;
    int total_habitaciones = 0;

    int total_enemigos = 0;
    enemigo** enemigos_globales = leer_enemigos_globales_impl(archivo_mapa, total_enemigos);
    int total_eventos = 0;
    evento** eventos_globales = leer_eventos_globales_impl(archivo_mapa, total_eventos);

    habitaciones = leer_habitaciones(archivo_mapa, total_habitaciones, arbol);

    if (habitaciones == NULL || total_habitaciones == 0) {
        cout << "No se pudieron leer las habitaciones." << endl;
        liberar_habitaciones(habitaciones, total_habitaciones);
        return 1;
    }

    leer_arcos(archivo_mapa, habitaciones, total_habitaciones, arbol);
    arbol.set_raiz(habitaciones[0]);

    jugador player = {100, 10, 0.7, 0};
    estacion* actual = arbol.get_raiz();

    int total_bonus = 0;
    bonus* bonus_arr = leer_bonus(archivo_mapa, total_bonus);
    bool bonus_usado = false;

    while (actual && player.vida > 0) {
        // Validar que la estación no sea nula
        if (actual == NULL) {
            cout << "ERROR: Habitación actual es nula. Fin del juego." << endl;
            break;
        }

        cout << "\n-- " << actual->nombre << " --\n" << endl;
        cout << actual->descripcion << "\n" << endl;

        if (actual->cantidad_enemigos > 0 && actual->enemigos != NULL) {
            cout << "\n¡Comienza un combate!" << endl;
            if (!combate(&player, actual->enemigos, actual->cantidad_enemigos)) {
                cout << "Has sido derrotado en combate.\n";
                break;
            }
            if (player.recuperacion > 0) {
                player.vida += player.recuperacion;
                cout << "Te recuperas tras el combate y recuperas " << player.recuperacion << " de vida. Vida actual: " << player.vida << endl;
            }
            aplicar_mejora(player);
        }

        // Evento si corresponde
        if (actual->evento_asociado != NULL) {
            evento* ev = actual->evento_asociado;
            if (ev && ev->opciones != NULL && ev->cantidad_opciones >= 2 &&
                !ev->opciones[0].descripcion.empty() && !ev->opciones[1].descripcion.empty()) {
                cout << "\n" << ev->descripcion << endl;
                cout << "A: " << ev->opciones[0].descripcion.substr(0, ev->opciones[0].descripcion.find('(')) << endl;
                cout << "B: " << ev->opciones[1].descripcion.substr(0, ev->opciones[1].descripcion.find('(')) << endl;
                char opcion;
                bool opcion_valida = false;
                do {
                    cout << "Elige opción (A/B): ";
                    cin >> opcion;
                    if (opcion == 'A' || opcion == 'a' || opcion == 'B' || opcion == 'b') {
                        opcion_valida = true;
                    } else {
                        opcion_valida = false;
                        cin.clear();
                        cin.ignore(10000, '\n');
                    }
                } while (!opcion_valida);
                // Convierte a mayúscula para el índice
                if (opcion == 'a') opcion = 'A';
                if (opcion == 'b') opcion = 'B';
                int idx = opcion - 'A';
                if (idx >= 0 && idx < 2) {
                    // Mostrar consecuencia textual (lo que está entre paréntesis)
                    string desc = ev->opciones[idx].descripcion;
                    size_t ini = desc.find('(');
                    size_t fin = desc.find(')');
                    if (ini != string::npos && fin != string::npos && fin > ini) {
                        cout << desc.substr(ini + 1, fin - ini - 1) << endl;
                    } else {
                        // Si no hay paréntesis, muestra la descripción completa o un mensaje neutro
                        cout << "Sin consecuencia especial." << endl;
                    }
                    // Aplicar efectos
                    player.vida += ev->opciones[idx].cambio_vida;
                    player.ataque += ev->opciones[idx].cambio_ataque;
                    player.precision += ev->opciones[idx].cambio_precision;
                    player.recuperacion += ev->opciones[idx].cambio_recuperacion;
                    // Mostrar solo los efectos aplicados (opcional)
                    if (ev->opciones[idx].cambio_vida != 0) cout << "Vida " << (ev->opciones[idx].cambio_vida > 0 ? "+" : "") << ev->opciones[idx].cambio_vida << " ";
                    if (ev->opciones[idx].cambio_ataque != 0) cout << "Ataque " << (ev->opciones[idx].cambio_ataque > 0 ? "+" : "") << ev->opciones[idx].cambio_ataque << " ";
                    if (ev->opciones[idx].cambio_precision != 0) cout << "Precisión " << (ev->opciones[idx].cambio_precision > 0 ? "+" : "") << ev->opciones[idx].cambio_precision << " ";
                    if (ev->opciones[idx].cambio_recuperacion != 0) cout << "Recuperación " << (ev->opciones[idx].cambio_recuperacion > 0 ? "+" : "") << ev->opciones[idx].cambio_recuperacion << " ";
                    cout << endl;
                }
            }
        }

        // Mostrar estado del jugador de forma estética
        cout << "\n[Dr.Wolves] Vida: " << player.vida
             << " | Ataque: " << player.ataque
             << " | Precisión: " << player.precision
             << " | Recuperación: " << player.recuperacion << endl;

        // Mostrar opciones de hijos y dejar elegir
        estacion* hijos[3];
        int num_hijos = 0;
        if (actual->n1) hijos[num_hijos++] = actual->n1;
        if (actual->n2) hijos[num_hijos++] = actual->n2;
        if (actual->n3) hijos[num_hijos++] = actual->n3;

        if (num_hijos == 0) {
            break;
        } else if (num_hijos == 1) {
            if (hijos[0] == NULL) {
                break;
            }
            cout << "Avanzas automáticamente a: " << hijos[0]->nombre << endl;
            actual = hijos[0];
        } else {
            cout << "¿A dónde quieres ir?\n";
            for (int i = 0; i < num_hijos; ++i) {
                if (hijos[i] == NULL) {
                    cout << (i+1) << ") [ERROR: Habitación nula]" << endl;
                } else {
                    cout << (i+1) << ") " << hijos[i]->nombre << endl;
                }
            }
            int eleccion = 0;
            do {
                cout << "Elige opción (1-" << num_hijos << "): ";
                if (!(cin >> eleccion)) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    eleccion = 0;
                }
            } while (eleccion < 1 || eleccion > num_hijos || hijos[eleccion-1] == NULL);
            actual = hijos[eleccion-1];
        }

        // BONUS: Viaje al pasado
        if (!bonus_usado && total_bonus > 0 && actual->id == bonus_arr[0].id_origen) {
            cout << '\n' <<bonus_arr[0].desc_post << endl;
            actual = arbol.get_raiz(); // Retorna a Cal y Canto
            bonus_usado = true;
            continue; // Vuelve a mostrar la raíz y repite el ciclo
        }
    }

    if (player.vida > 0)
        cout << "\nFin del juego\n";

    cout << "\nEstado final del jugador:\n";
    cout << "[Dr.Wolves] Vida: " << player.vida
         << " | Ataque: " << player.ataque
         << " | Precisión: " << player.precision
         << " | Recuperación: " << player.recuperacion << endl;


    liberar_habitaciones(habitaciones, total_habitaciones);
    liberar_enemigos(enemigos_globales, total_enemigos);
    liberar_eventos(eventos_globales, total_eventos);
    if (bonus_arr) delete[] bonus_arr;

    return 0;
}