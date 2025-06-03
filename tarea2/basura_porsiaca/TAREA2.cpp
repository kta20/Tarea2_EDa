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
            int evento_idx = 0;
            while (evento_idx < total_eventos && getline(archivo, linea)) {
                if (linea.empty() || linea == "&") continue;
                // Nombre del evento
                string nombre = linea;
                // Probabilidad
                getline(archivo, linea);
                float prob = 0.0;
                if (linea.find("Probabilidad") != string::npos) {
                    size_t p = linea.find(' ');
                    prob = stof(linea.substr(p + 1));
                }
                // Descripción
                getline(archivo, linea);
                string descripcion = linea;

                // Opciones (A: ... | B: ...)
                opcion_evento* opciones = new opcion_evento[2]; // Máximo 2 opciones por evento
                int cant_opciones = 0;
                for (int i = 0; i < 2; i++) {
                    if (!getline(archivo, linea)) break;
                    if (linea.empty()) break;
                    if (linea[1] == ':') {
                        opciones[cant_opciones].descripcion = linea.substr(3); // después de "A: " o "B: "
                        // Consecuencia
                        getline(archivo, linea);
                        opciones[cant_opciones].cambio_vida = 0;
                        opciones[cant_opciones].cambio_ataque = 0;
                        opciones[cant_opciones].cambio_precision = 0;
                        opciones[cant_opciones].cambio_recuperacion = 0;
                        // Busca palabras clave en la consecuencia
                        if (linea.find("Vida") != string::npos) {
                            size_t posv = linea.find("Vida");
                            int valor = stoi(linea.substr(0, posv));
                            opciones[cant_opciones].cambio_vida = valor;
                        }
                        if (linea.find("Precision") != string::npos) {
                            size_t posp = linea.find("Precision");
                            float valor = stof(linea.substr(0, posp));
                            opciones[cant_opciones].cambio_precision = valor;
                        }
                        if (linea.find("Ataque") != string::npos) {
                            size_t posa = linea.find("Ataque");
                            int valor = stoi(linea.substr(0, posa));
                            opciones[cant_opciones].cambio_ataque = valor;
                        }
                        if (linea.find("Recuperacion") != string::npos) {
                            size_t posr = linea.find("Recuperacion");
                            int valor = stoi(linea.substr(0, posr));
                            opciones[cant_opciones].cambio_recuperacion = valor;
                        }
                        cant_opciones++;
                    }
                }
                // Crear evento
                evento* ev = new evento;
                ev->nombre = nombre;
                ev->descripcion = descripcion;
                ev->probabilidad = prob;
                ev->opciones = opciones;
                ev->cantidad_opciones = cant_opciones;
                eventos[evento_idx++] = ev;
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
                    break;
                }
                int pos1 = linea.find('|');
                int pos2 = linea.find('|', pos1 + 1);
                int pos3 = linea.find('|', pos2 + 1);
                if (pos1 == -1 || pos2 == -1 || pos3 == -1) {
                    cout << "ERROR: Formato incorrecto en línea de habitación: " << linea << endl;
                    continue;
                }
                string id_str = linea.substr(0, pos1);
                int id = stoi(id_str);
                cout << "DEBUG: id_str = '" << id_str << "', id = " << id << ", total_habitaciones = " << total_habitaciones << endl;
                if (id < 0 || id >= total_habitaciones) {
                    cout << "ERROR: id fuera de rango: " << id << endl;
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
                    habitaciones[id]->evento_asociado = eventos_globales[idx];
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
    return habitaciones;
}

// Lee los arcos desde el archivo y conecta las habitaciones
void leer_y_conectar_arcos(string filename, estacion** habitaciones, int total_habitaciones, ArbolTernario& arbol) {
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

// =======================
// Manejo de la cola de enemigos
// =======================
cola_enemigo* crear_cola_enemigos() {
    return NULL;
}

void encolar(cola_enemigo*& frente, cola_enemigo*& fin, enemigo* e) {
    cola_enemigo* nuevo = new cola_enemigo;
    nuevo->dato = e;
    nuevo->sig = NULL;
    if (fin) {
        fin->sig = nuevo;
    } else {
        frente = nuevo;
    }
    fin = nuevo;
}

enemigo* desencolar(cola_enemigo*& frente, cola_enemigo*& fin) {
    if (!frente) return NULL;
    cola_enemigo* temp = frente;
    enemigo* e = temp->dato;
    frente = frente->sig;
    if (!frente) fin = NULL;
    delete temp;
    return e;
}

bool cola_vacia(cola_enemigo* frente) {
    return frente == NULL;
}

void liberar_cola(cola_enemigo*& frente) {
    while (frente) {
        cola_enemigo* temp = frente;
        frente = frente->sig;
        delete temp;
    }
}

// =======================
// Combate
// =======================
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

// Estructura para registrar golpes

void push_golpe(pila_golpes& pila, const string& atacante, const string& objetivo, int fuerza, bool acierto, int vida_objetivo) {
    golpe* nuevo = new golpe{atacante, objetivo, fuerza, acierto, vida_objetivo, pila.tope};
    pila.tope = nuevo;
}

golpe* pop_golpe(pila_golpes& pila) {
    if (!pila.tope) return nullptr;
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
bool combate_interactivo(jugador* player, enemigo** enemigos, int total_enemigos) {
    pila_golpes pila;
    pila.tope = nullptr;

    // Selecciona enemigo aleatorio
    enemigo enemigo_actual = *seleccionar_enemigo_aleatorio(enemigos, total_enemigos);

    cout << "¡Te enfrentas a " << enemigo_actual.nombre << "!" << endl;

    while (player->vida > 0 && enemigo_actual.vida > 0) {
        cout << "\nTu vida: " << player->vida << " | Vida de " << enemigo_actual.nombre << ": " << enemigo_actual.vida << endl;
        cout << "Elige fuerza de tu golpe (1-" << player->ataque << "): ";
        int fuerza;
        cin >> fuerza;
        if (fuerza < 1) fuerza = 1;
        if (fuerza > player->ataque) fuerza = player->ataque;

        // Determina si acierta el jugador
        float prob = (float)rand() / RAND_MAX;
        bool acierto_jugador = (prob < player->precision);

        if (acierto_jugador) {
            enemigo_actual.vida -= fuerza;
            if (enemigo_actual.vida < 0) enemigo_actual.vida = 0;
            cout << "¡Golpeaste a " << enemigo_actual.nombre << " por " << fuerza << " de daño!" << endl;
        } else {
            cout << "¡Fallaste el golpe!" << endl;
        }
        push_golpe(pila, "Jugador", enemigo_actual.nombre, fuerza, acierto_jugador, enemigo_actual.vida);

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
        push_golpe(pila, enemigo_actual.nombre, "Jugador", fuerza_enemigo, acierto_enemigo, player->vida);
    }

    // Pasa los golpes a un arreglo
    int total_golpes = 0;
    golpe* temp = pila.tope;
    while (temp) {
        total_golpes++;
        temp = temp->sig;
    }
    golpe** historial = new golpe*[total_golpes];
    for (int i = total_golpes-1; i >= 0; --i) {
        golpe* g = pop_golpe(pila);
        historial[i] = g;
    }

    // Muestra historial
    cout << "\n--- Historial de golpes ---\n";
    for (int i = 0; i < total_golpes; ++i) {
        cout << historial[i]->atacante << " -> " << historial[i]->objetivo
             << " | Fuerza: " << historial[i]->fuerza
             << " | " << (historial[i]->acierto ? "Acierta" : "Falla")
             << " | Vida objetivo: " << historial[i]->vida_objetivo << endl;
        delete historial[i];
    }
    delete[] historial;

    // Muestra porcentaje de vida restante
    float porcentaje = (float)player->vida / 100.0f * 100.0f;
    cout << "\nPorcentaje de vida restante: " << porcentaje << "%" << endl;

    return player->vida > 0;
}

// =======================
// Main
// =======================
int main() {
    string archivo_mapa = "juego.map";
    ArbolTernario arbol;
    estacion** habitaciones = nullptr;
    int total_habitaciones = 0;

    // Declarar variables globales para liberar memoria al final
    int total_enemigos = 0;
    enemigo** enemigos_globales = leer_enemigos_globales_impl(archivo_mapa, total_enemigos);
    int total_eventos = 0;
    evento** eventos_globales = leer_eventos_globales_impl(archivo_mapa, total_eventos);

    // Lee habitaciones y construye el arreglo
    habitaciones = leer_habitaciones(archivo_mapa, total_habitaciones, arbol);

    if (habitaciones == nullptr || total_habitaciones == 0) {
        cout << "No se pudieron leer las habitaciones." << endl;
        // Liberar memoria de enemigos y eventos globales si habitaciones no se pudo leer
        for (int i = 0; i < total_enemigos; ++i)
            delete enemigos_globales[i];
        delete[] enemigos_globales;
        for (int i = 0; i < total_eventos; ++i)
            delete eventos_globales[i];
        delete[] eventos_globales;
        return 1;
    }

    // Conecta los arcos entre habitaciones
    leer_y_conectar_arcos(archivo_mapa, habitaciones, total_habitaciones, arbol);

    // Establece la raíz del árbol
    arbol.set_raiz(obtener_raiz(habitaciones));

    // --- Bucle principal del juego ---
    jugador player = {100, 10, 0.7, 0};
    estacion* actual = arbol.get_raiz();

    while (actual && player.vida > 0) {
        cout << "\n=== Estás en: " << actual->nombre << " ===\n";
        cout << actual->descripcion << endl;

        // Combate interactivo si corresponde
        if (actual->cantidad_enemigos > 0 && actual->enemigos != nullptr) {
            cout << "\n¡Comienza un combate!" << endl;
            if (!combate_interactivo(&player, actual->enemigos, actual->cantidad_enemigos)) {
                cout << "Has sido derrotado en combate.\n";
                break;
            }
        }

        // Evento si corresponde
        if (actual->evento_asociado != nullptr) {
            evento* ev = actual->evento_asociado;
            cout << "\n¡Evento!: " << ev->nombre << endl;
            cout << ev->descripcion << endl;
            for (int i = 0; i < ev->cantidad_opciones; ++i) {
                cout << char('A' + i) << ": " << ev->opciones[i].descripcion << endl;
            }
            cout << "Elige opción (A/B): ";
            char opcion;
            cin >> opcion;
            int idx = toupper(opcion) - 'A';
            if (idx >= 0 && idx < ev->cantidad_opciones) {
                opcion_evento& op = ev->opciones[idx];
                player.vida += op.cambio_vida;
                player.ataque += op.cambio_ataque;
                player.precision += op.cambio_precision;
                player.recuperacion += op.cambio_recuperacion;
                cout << "Resultado: ";
                if (op.cambio_vida != 0) cout << "Vida " << (op.cambio_vida > 0 ? "+" : "") << op.cambio_vida << " ";
                if (op.cambio_ataque != 0) cout << "Ataque " << (op.cambio_ataque > 0 ? "+" : "") << op.cambio_ataque << " ";
                if (op.cambio_precision != 0) cout << "Precisión " << (op.cambio_precision > 0 ? "+" : "") << op.cambio_precision << " ";
                if (op.cambio_recuperacion != 0) cout << "Recuperación " << (op.cambio_recuperacion > 0 ? "+" : "") << op.cambio_recuperacion << " ";
                cout << endl;
            }
        }

        // Mostrar estado del jugador
        cout << "Estado del jugador: Vida=" << player.vida << " Ataque=" << player.ataque << " Precisión=" << player.precision << endl;

        // Mostrar opciones de hijos y dejar elegir (sin vector)
        estacion* hijos[3];
        int num_hijos = 0;
        if (actual->n1) hijos[num_hijos++] = actual->n1;
        if (actual->n2) hijos[num_hijos++] = actual->n2;
        if (actual->n3) hijos[num_hijos++] = actual->n3;

        if (num_hijos == 0) {
            cout << "No hay más caminos desde aquí. Fin del recorrido.\n";
            break;
        } else if (num_hijos == 1) {
            cout << "Avanzas automáticamente a: " << hijos[0]->nombre << endl;
            actual = hijos[0];
        } else {
            cout << "¿A dónde quieres ir?\n";
            for (int i = 0; i < num_hijos; ++i) {
                cout << (i+1) << ") " << hijos[i]->nombre << endl;
            }
            int eleccion = 0;
            do {
                cout << "Elige opción (1-" << num_hijos << "): ";
                cin >> eleccion;
            } while (eleccion < 1 || eleccion > num_hijos);
            actual = hijos[eleccion-1];
        }
    }

    if (player.vida > 0)
        cout << "\n¡Felicidades, has sobrevivido la aventura!\n";
    else
        cout << "\nHas muerto. Fin del juego.\n";

    // --- Liberación de memoria (igual que ya tienes) ---
    if (habitaciones != nullptr) {
        for (int i = 0; i < total_habitaciones; ++i) {
            if (habitaciones[i]) {
                if (habitaciones[i]->enemigos != nullptr) {
                    for (int j = 0; j < habitaciones[i]->cantidad_enemigos; ++j) {
                        if (habitaciones[i]->enemigos[j]) {
                            delete habitaciones[i]->enemigos[j];
                        }
                    }
                    delete[] habitaciones[i]->enemigos;
                }
                // NO hacer delete habitaciones[i]->evento_asociado aquí
                delete habitaciones[i];
            }
        }
        delete[] habitaciones;
        habitaciones = nullptr;
    }
    for (int i = 0; i < total_enemigos; ++i)
        delete enemigos_globales[i];
    delete[] enemigos_globales;

    for (int i = 0; i < total_eventos; ++i)
        delete eventos_globales[i];
    delete[] eventos_globales;
    return 0;
};

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
    if (!getline(in, linea)) return nullptr;
    if (linea == "#") return nullptr;

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
        nodo->enemigos = nullptr;
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
    } else {
        nodo->evento_asociado = nullptr;
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