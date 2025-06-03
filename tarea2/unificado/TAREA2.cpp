#include "estructuras.hpp" 
#include "arbol.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;


// Lee todos los enemigos definidos en el archivo y retorna un arreglo
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
                int p1 = linea.find('|');
                int p2 = linea.find('|', p1 + 1);
                int p3 = linea.find('|', p2 + 1);
                int p4 = linea.find('|', p3 + 1);
                string nombre = linea.substr(0, p1);
                int vida = stoi(linea.substr(p1 + 1, p2 - p1 - 1));
                int ataque = stoi(linea.substr(p2 + 1, p3 - p2 - 1));
                float precision = stof(linea.substr(p3 + 1, p4 - p3 - 1));
                float prob_spawn = stof(linea.substr(p4 + 1));
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

                // Parsear la línea del evento SIN vector
                string partes[9];
                int partes_al_lei = 0;
                size_t start = 0, end;
                while ((end = linea.find('|', start)) != string::npos && partes_al_lei < 8) {
                    partes[partes_al_lei++] = linea.substr(start, end - start);
                    start = end + 1;
                }
                partes[partes_al_lei] = linea.substr(start); // última parte

                // Rellenar vacíos si faltan partes
                for (int i = partes_al_lei + 1; i < 9; ++i) partes[i] = "";

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
        cout << "No se ha podido abrir el archivo\n";
        total_habitaciones = 0;
        return NULL;
    }

    string linea;
    estacion** habitaciones = NULL;

    // Leer enemigos globales primero
    int total_enemigos = 0;
    enemigo** enemigos_globales = leer_enemigos_globales_impl(filename, total_enemigos);
    int total_eventos = 0;
    evento** eventos_globales = leer_eventos_globales_impl(filename, total_eventos);
    srand(time(NULL)); // Inicializa aleatorio

    while (getline(archivo, linea)) {
        if (linea.find("HABITACIONES|") == 0) {
            int pos = linea.find('|');
            total_habitaciones = stoi(linea.substr(pos + 1));
            habitaciones = new estacion*[total_habitaciones];
            for (int i = 0; i < total_habitaciones; ++i) {
                if (!getline(archivo, linea)) {
                    cout << "ERROR: No se pudo leer la línea de la habitación " << i << endl;
                    habitaciones[i] = NULL; // <- Marca como nulo para evitar acceso posterior
                    continue;
                }
                int pos1 = linea.find('|');
                int pos2 = linea.find('|', pos1 + 1);
                int pos3 = linea.find('|', pos2 + 1);
                if (pos1 == -1 || pos2 == -1 || pos3 == -1) {
                    cout << "ERROR: Formato incorrecto en línea de habitación: " << linea << endl;
                    habitaciones[i] = NULL;
                    continue;
                }
                string id_str = linea.substr(0, pos1);
                int id = stoi(id_str);
                if (id < 0 || id >= total_habitaciones) {
                    cout << "ERROR: id fuera de rango: " << id << endl;
                    habitaciones[i] = NULL;
                    continue;
                }
                string nombre = linea.substr(pos1 + 1, pos2 - pos1 - 1);
                string tipo = linea.substr(pos2 + 1, pos3 - pos2 - 1);
                string descripcion = linea.substr(pos3 + 1);
                habitaciones[id] = arbol.crear_estacion(id, nombre, tipo, descripcion);

                // EVENTOS
                if (tipo.find("EVENTO") != string::npos && eventos_globales && total_eventos > 0) {
                    float r = (float)rand() / RAND_MAX;
                    float suma = 0;
                    int idx = 0;
                    for (int e = 0; e < total_eventos; ++e) {
                        suma += eventos_globales[e]->probabilidad;
                        if (r <= suma) {
                            idx = e;
                            break;
                        }
                    }
                    if (suma < 1.0 && r > suma) idx = total_eventos - 1; // <-- Asegura que idx siempre sea válido
                    // Validar evento antes de asignar
                    if (eventos_globales[idx] && eventos_globales[idx]->opciones != NULL && eventos_globales[idx]->cantidad_opciones >= 2) {
                        habitaciones[id]->evento_asociado = eventos_globales[idx];
                        habitaciones[id]->evento_dinamico = false;
                    } else {
                        habitaciones[id]->evento_asociado = NULL;
                    }
                } else {
                    habitaciones[id]->evento_asociado = NULL;
                }

                // COMBATE
                if (tipo.find("COMBATE") != string::npos && enemigos_globales && total_enemigos > 0) {
                    int cant = 1 + rand() % 2;
                    habitaciones[id]->cantidad_enemigos = cant;
                    habitaciones[id]->enemigos = new enemigo*[cant];
                    for (int j = 0; j < cant; ++j) {
                        float r = (float)rand() / RAND_MAX;
                        float suma = 0;
                        habitaciones[id]->enemigos[j] = NULL;
                        for (int k = 0; k < total_enemigos; k++) {
                            suma += enemigos_globales[k]->prob_spawn;
                            if (r <= suma) {
                                habitaciones[id]->enemigos[j] = new enemigo(*enemigos_globales[k]);
                                break;
                            }
                        }
                        if (habitaciones[id]->enemigos[j] == NULL)
                            habitaciones[id]->enemigos[j] = new enemigo(*enemigos_globales[total_enemigos-1]);
                    }
                } else {
                    habitaciones[id]->cantidad_enemigos = 0;
                    habitaciones[id]->enemigos = NULL;
                }
            }
            break; // Ya leíste las habitaciones
        }
    }
    archivo.close();

    // Libera SOLO los enemigos globales aquí (no los eventos)
    if (enemigos_globales) {
        for (int i = 0; i < total_enemigos; ++i)
            delete enemigos_globales[i];
        delete[] enemigos_globales;
    }
    // NO liberes eventos_globales aquí

    return habitaciones;
}

// Lee los arcos desde el archivo y conecta las habitaciones
void leer_arcos(string filename, estacion** habitaciones, int total_habitaciones, ArbolTernario& arbol) {
    ifstream archivo;
    archivo.open(filename.c_str());
    if (!archivo.is_open()) {
        cout << "No se ha podido abrir el archivo para leer arcos\n";
        return;
    }

    string linea;
    while (getline(archivo, linea)) {
        if (linea.find("ARCOS|") == 0) {
            int pos = linea.find('|');
            if (pos == -1) {
                cout << "Error: línea sin '|': " << linea << endl;
                return;
            }
            string arcos_str = linea.substr(pos + 1);
            arcos_str.erase(0, arcos_str.find_first_not_of(" \t\r\n"));
            int total_arcos = stoi(arcos_str);
            for (int i = 0; i < total_arcos; ++i) {
                getline(archivo, linea);
                int pos_sep = linea.find('|');
                string origen_str = linea.substr(0, pos_sep);
                string destino_str = linea.substr(pos_sep + 1);
                int id_origen = stoi(origen_str);
                int id_destino = stoi(destino_str);
                arbol.agregar_hijo(habitaciones[id_origen], habitaciones[id_destino]);
            }
            break; // Ya leíste los arcos
        }
    }
    archivo.close();
}

// Devuelve la raíz del árbol (habitaciones[0])
estacion* obtener_raiz(estacion** habitaciones) {
    return habitaciones[0];
}

cola_enemigo* crear_cola_enemigos() {
    return NULL;
}

// Crear un nuevo nodo de cola
cola_enemigo* crear_nodo_cola(enemigo* e) {
    cola_enemigo* nuevo = new cola_enemigo;
    nuevo->dato = e;
    nuevo->sig = NULL;
    return nuevo;
}

// Encolar un enemigo
void encolar(cola_enemigo*& frente, cola_enemigo*& fin, enemigo* e) {
    cola_enemigo* nuevo = crear_nodo_cola(e);
    if (fin == NULL) {
        frente = fin = nuevo;
    } else {
        fin->sig = nuevo;
        fin = nuevo;
    }
}

// Desencolar un enemigo
enemigo* desencolar(cola_enemigo*& frente, cola_enemigo*& fin) {
    if (frente == NULL) return NULL;
    cola_enemigo* temp = frente;
    enemigo* e = temp->dato;
    frente = frente->sig;
    if (frente == NULL) fin = NULL;
    delete temp;
    return e;
}

bool cola_vacia(cola_enemigo* frente) {
    return frente == NULL;
}

void liberar_cola(cola_enemigo*& frente) {
    while (frente != NULL) {
        cola_enemigo* temp = frente;
        frente = frente->sig;
        delete temp;
    }
}


void push_golpe(pila_golpes& pila, const string& atacante, const string& objetivo, int fuerza, bool acierto, int vida_objetivo) {
    golpe* nuevo = new golpe{atacante, objetivo, fuerza, acierto, vida_objetivo, pila.tope};
    pila.tope = nuevo;
}

golpe* pop_golpe(pila_golpes& pila) {
    if (!pila.tope) return NULL;
    golpe* g = pila.tope;
    pila.tope = pila.tope->sig;
    return g;
}

void liberar_pila(pila_golpes& pila) {
    while (pila.tope) {
        golpe* temp = pila.tope;
        pila.tope = pila.tope->sig;
        delete temp;
    }
}

// Selecciona un enemigo aleatorio según probabilidad de aparición
enemigo* seleccionar_enemigo_aleatorio(enemigo** enemigos, int total_enemigos) {
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
    enemigo enemigo_actual = *seleccionar_enemigo_aleatorio(enemigos, total_enemigos);
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




int main() {
    string archivo_mapa = "juego.map";
    ArbolTernario arbol;
    estacion** habitaciones = NULL;
    int total_habitaciones = 0;

    // Declarar variables globales para liberar memoria al final
    int total_enemigos = 0;
    enemigo** enemigos_globales = leer_enemigos_globales_impl(archivo_mapa, total_enemigos);
    int total_eventos = 0;
    evento** eventos_globales = leer_eventos_globales_impl(archivo_mapa, total_eventos);

    // Lee habitaciones y construye el arreglo
    habitaciones = leer_habitaciones(archivo_mapa, total_habitaciones, arbol);

    if (habitaciones == NULL || total_habitaciones == 0) {
        cout << "No se pudieron leer las habitaciones." << endl;

        // Libera habitaciones parcialmente creadas
        if (habitaciones != NULL) {
            for (int i = 0; i < total_habitaciones; ++i) {
                if (habitaciones[i] != NULL) {
                    if (habitaciones[i]->enemigos != NULL) {
                        for (int j = 0; j < habitaciones[i]->cantidad_enemigos; ++j)
                            if (habitaciones[i]->enemigos[j]) delete habitaciones[i]->enemigos[j];
                        delete[] habitaciones[i]->enemigos;
                    }
                    if (habitaciones[i]->evento_dinamico && habitaciones[i]->evento_asociado != NULL) {
                        delete[] habitaciones[i]->evento_asociado->opciones;
                        delete habitaciones[i]->evento_asociado;
                    }
                    delete habitaciones[i];
                }
            }
            delete[] habitaciones;
        }

        // Libera enemigos y eventos globales
        for (int i = 0; i < total_enemigos; ++i)
            delete enemigos_globales[i];
        delete[] enemigos_globales;
        for (int i = 0; i < total_eventos; ++i)
            delete eventos_globales[i];
        delete[] eventos_globales;
        return 1;
    }

    // Conecta los arcos entre habitaciones
    leer_arcos(archivo_mapa, habitaciones, total_habitaciones, arbol);

    // Establece la raíz del árbol
    arbol.set_raiz(obtener_raiz(habitaciones));

    // --- Bucle principal del juego ---
    jugador player = {100, 10, 0.7, 0};
    estacion* actual = arbol.get_raiz();

    while (actual && player.vida > 0) {
        // Validar que la estación no sea nula
        if (actual == NULL) {
            cout << "ERROR: Habitación actual es nula. Fin del juego." << endl;
            break;
        }

        // Presentación de la estación
        cout << "\n-- " << actual->nombre << " --\n" << endl;
        cout << actual->descripcion << "\n" << endl;

        // Combate interactivo si corresponde
        if (actual->cantidad_enemigos > 0 && actual->enemigos != NULL) {
            cout << "\n¡Comienza un combate!" << endl;
            if (!combate(&player, actual->enemigos, actual->cantidad_enemigos)) {
                cout << "Has sido derrotado en combate.\n";
                break;
            }
            // --- Recuperación tras combate ---
            if (player.recuperacion > 0) {
                player.vida += player.recuperacion;
                cout << "Te recuperas tras el combate y recuperas " << player.recuperacion << " de vida. Vida actual: " << player.vida << endl;
            }
            // --- Lógica de mejora tras combate ---
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
            } else {
                cout << "[ERROR] Evento mal formado o sin opciones válidas. Se omite el evento.\n";
            }
        }

        // Mostrar estado del jugador de forma estética
        cout << "\n[Jugador] Vida: " << player.vida
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
    }

    if (player.vida > 0)
        cout << "\nFin del juego\n";

    cout << "\nEstado final del jugador:\n";
    cout << "[Jugador] Vida: " << player.vida
         << " | Ataque: " << player.ataque
         << " | Precisión: " << player.precision
         << " | Recuperación: " << player.recuperacion << endl;

    // Libera el árbol de habitaciones (y sus campos)
    arbol.liberar_arbol(arbol.get_raiz());

    // Libera solo el arreglo de punteros (NO los objetos, ya están liberados)
    if (habitaciones != NULL) {
        delete[] habitaciones;
        habitaciones = NULL;
    }

    // Libera enemigos globales
    for (int i = 0; i < total_enemigos; ++i)
        delete enemigos_globales[i];
    delete[] enemigos_globales;

    // Libera eventos globales SOLO si no fueron liberados como dinámicos
    for (int i = 0; i < total_eventos; ++i) {
        bool fue_liberado = false;
        for (int j = 0; j < total_habitaciones; ++j) {
            if (habitaciones && habitaciones[j] &&
                habitaciones[j]->evento_asociado == eventos_globales[i] &&
                habitaciones[j]->evento_dinamico) {
                fue_liberado = true;
                break;
            }
        }
        if (!fue_liberado) {
            delete[] eventos_globales[i]->opciones;
            delete eventos_globales[i];
        }
    }
    delete[] eventos_globales;

    return 0;
}

// Función para guardar el árbol en un archivo
void guardar_arbol(estacion* raiz, ofstream& out) {
    if (!raiz) {
        out << "#\n";
        return;
    }
    // Datos básicos
    out << raiz->id << "|" << raiz->nombre << "|" << raiz->tipo << "|" << raiz->descripcion << "\n";
    // Enemigos
    out << raiz->cantidad_enemigos << "\n";
    for (int i = 0; i < raiz->cantidad_enemigos; ++i) {
        enemigo* e = raiz->enemigos[i];
        out << e->nombre << "|" << e->vida << "|" << e->ataque << "|" << e->precision << "|" << e->prob_spawn << "\n";
    }
    // Evento asociado
    if (raiz->evento_asociado) {
        evento* ev = raiz->evento_asociado;
        out << "EVENTO|" << ev->nombre << "|" << ev->descripcion << "|" << ev->probabilidad << "|" << ev->cantidad_opciones << "\n";
        for (int i = 0; i < ev->cantidad_opciones; ++i) {
            out << ev->opciones[i].descripcion << "|" << ev->opciones[i].cambio_vida << "|" << ev->opciones[i].cambio_ataque
                << "|" << ev->opciones[i].cambio_precision << "|" << ev->opciones[i].cambio_recuperacion << "\n";
        }
    } else {
        out << "NO_EVENTO\n";
    }
    // Recursivo para hijos
    guardar_arbol(raiz->n1, out);
    guardar_arbol(raiz->n2, out);
    guardar_arbol(raiz->n3, out);
}

// Uso:
// ofstream out("arbol.save");
// guardar_arbol(arbol.get_raiz(), out);
// out.close();

// Carga un árbol desde un archivo
estacion* cargar_arbol(ifstream& in, ArbolTernario& arbol) {
    string linea;
    if (!getline(in, linea)) return NULL;
    if (linea == "#") return NULL;

    // Datos básicos
    int p1 = linea.find('|');
    int p2 = linea.find('|', p1 + 1);
    int p3 = linea.find('|', p2 + 1);
    int id = stoi(linea.substr(0, p1));
    string nombre = linea.substr(p1 + 1, p2 - p1 - 1);
    string tipo = linea.substr(p2 + 1, p3 - p2 - 1);
    string descripcion = linea.substr(p3 + 1);

    estacion* nodo = arbol.crear_estacion(id, nombre, tipo, descripcion);

    // Enemigos
    getline(in, linea);
    int cant_enemigos = stoi(linea);
    nodo->cantidad_enemigos = cant_enemigos;
    if (cant_enemigos > 0) {
        nodo->enemigos = new enemigo*[cant_enemigos];
        for (int i = 0; i < cant_enemigos; ++i) {
            getline(in, linea);
            int q1 = linea.find('|');
            int q2 = linea.find('|', q1 + 1);
            int q3 = linea.find('|', q2 + 1);
            int q4 = linea.find('|', q3 + 1);
            string enom = linea.substr(0, q1);
            int evida = stoi(linea.substr(q1 + 1, q2 - q1 - 1));
            int eataque = stoi(linea.substr(q2 + 1, q3 - q2 - 1));
            float eprecision = stof(linea.substr(q3 + 1, q4 - q3 - 1));
            float eprob = stof(linea.substr(q4 + 1));
            nodo->enemigos[i] = new enemigo{enom, evida, eataque, eprecision, eprob};
        }
    } else {
        nodo->enemigos = NULL;
    }

    // Evento asociado
    getline(in, linea);
    if (linea.find("EVENTO|") == 0) {
        int e1 = linea.find('|');
        int e2 = linea.find('|', e1 + 1);
        int e3 = linea.find('|', e2 + 1);
        int e4 = linea.find('|', e3 + 1);
        string ev_nombre = linea.substr(e1 + 1, e2 - e1 - 1);
        string ev_desc = linea.substr(e2 + 1, e3 - e2 - 1);
        float ev_prob = stof(linea.substr(e3 + 1, e4 - e3 - 1));
        int ev_cant = stoi(linea.substr(e4 + 1));
        evento* ev = new evento;
        ev->nombre = ev_nombre;
        ev->descripcion = ev_desc;
        ev->probabilidad = ev_prob;
        ev->cantidad_opciones = ev_cant;
        ev->opciones = new opcion_evento[ev_cant];
        for (int i = 0; i < ev_cant; ++i) {
            getline(in, linea);
            int o1 = linea.find('|');
            int o2 = linea.find('|', o1 + 1);
            int o3 = linea.find('|', o2 + 1);
            int o4 = linea.find('|', o3 + 1);
            ev->opciones[i].descripcion = linea.substr(0, o1);
            ev->opciones[i].cambio_vida = stoi(linea.substr(o1 + 1, o2 - o1 - 1));
            ev->opciones[i].cambio_ataque = stoi(linea.substr(o2 + 1, o3 - o2 - 1));
            ev->opciones[i].cambio_precision = stof(linea.substr(o3 + 1, o4 - o3 - 1));
            ev->opciones[i].cambio_recuperacion = stoi(linea.substr(o4 + 1));
        }
        nodo->evento_asociado = ev;
        nodo->evento_dinamico = true;
    } else {
        nodo->evento_asociado = NULL;
    }

    // Recursivo para hijos
    nodo->n1 = cargar_arbol(in, arbol);
    nodo->n2 = cargar_arbol(in, arbol);
    nodo->n3 = cargar_arbol(in, arbol);
    return nodo;
}

// Uso:
// ifstream in("arbol.save");
// estacion* raiz = cargar_arbol(in, arbol);
// arbol.set_raiz(raiz);
// in.close();