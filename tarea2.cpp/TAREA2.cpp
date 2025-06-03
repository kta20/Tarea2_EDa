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
                getline(archivo, linea);
                int pos1 = linea.find('|');
                int pos2 = linea.find('|', pos1 + 1);
                int pos3 = linea.find('|', pos2 + 1);
                string id_str = linea.substr(0, pos1);
                cout << "DEBUG: id_str = '" << id_str << "'" << endl;
                int id = stoi(id_str);
                string nombre = linea.substr(pos1 + 1, pos2 - pos1 - 1);
                string tipo = linea.substr(pos2 + 1, pos3 - pos2 - 1);
                string descripcion = linea.substr(pos3 + 1);
                habitaciones[id] = arbol.crear_estacion(id, nombre, tipo, descripcion);
                if (tipo.find("EVENTO") != string::npos) {
                    int idx = rand() % total_eventos;
                    habitaciones[id]->evento_asociado = eventos_globales[idx];
                } else {
                    habitaciones[id]->evento_asociado = NULL;
                }
                // Si es COMBATE, asigna enemigos aleatorios
                if (tipo.find("COMBATE") != string::npos) {
                    int cant = 1 + rand() % 2;
                    habitaciones[id]->cantidad_enemigos = cant;
                    habitaciones[id]->enemigos = new enemigo*[cant];
                    for (int j = 0; j < cant; ++j) {
                        float r = (float)rand() / RAND_MAX;
                        float suma = 0;
                        for (int k = 0; k < total_enemigos; ++k) {
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

// =======================
// Main
// =======================
int main() {
    string archivo_mapa = "juego.map";
    ArbolTernario arbol;

    int total_habitaciones = 0;
    // Lee habitaciones y construye el arreglo
    estacion** habitaciones = leer_habitaciones(archivo_mapa, total_habitaciones, arbol);

    if (habitaciones == nullptr || total_habitaciones == 0) {
        cout << "No se pudieron leer las habitaciones." << endl;
        return 1;
    }

    // Conecta los arcos entre habitaciones
    leer_y_conectar_arcos(archivo_mapa, habitaciones, total_habitaciones, arbol);

    // Establece la raíz del árbol
    arbol.set_raiz(obtener_raiz(habitaciones));

    // Muestra el árbol en preorden
    arbol.mostrar_preorden();

    // --- INICIO DEL JUEGO INTERACTIVO ---
    jugador player = {100, 10, 0.7, 0}; // valores iniciales ejemplo
    estacion* actual = habitaciones[0]; // Empieza en la raíz

    while (true) {
        cout << "\nEstás en: " << actual->nombre << " (" << actual->tipo << ")\n";
        cout << actual->descripcion << endl;

        // Si hay enemigos, combate
        if (actual->cantidad_enemigos > 0 && actual->enemigos != nullptr) {
            cout << "¡Enemigos encontrados!\n";
            cola_enemigo* frente = nullptr;
            cola_enemigo* fin = nullptr;
            for (int i = 0; i < actual->cantidad_enemigos; ++i)
                encolar(frente, fin, actual->enemigos[i]);
            if (!combate(&player, frente, fin)) {
                cout << "¡Has muerto en combate!\n";
                break;
            }
            liberar_cola(frente);
        }

        // Si hay evento
        if (actual->evento_asociado != nullptr) {
            evento* ev = actual->evento_asociado;
            cout << "\nEvento: " << ev->nombre << "\n" << ev->descripcion << endl;
            for (int i = 0; i < ev->cantidad_opciones; ++i)
                cout << char('A'+i) << ": " << ev->opciones[i].descripcion << endl;
            cout << "Elige opción: ";
            char op;
            cin >> op;
            int idx = toupper(op) - 'A';
            if (idx >= 0 && idx < ev->cantidad_opciones) {
                player.vida += ev->opciones[idx].cambio_vida;
                player.ataque += ev->opciones[idx].cambio_ataque;
                player.precision += ev->opciones[idx].cambio_precision;
                player.recuperacion += ev->opciones[idx].cambio_recuperacion;
                cout << "Resultado aplicado. Vida: " << player.vida << ", Ataque: " << player.ataque << endl;
            }
        }

        // Si es FIN, termina el juego
        if (actual->tipo.find("FIN") != string::npos) {
            cout << "\n¡Fin del juego!\n";
            break;
        }

        // Mostrar opciones de movimiento
        cout << "\nOpciones de movimiento:\n";
        int opciones = 0;
        estacion* hijos[3] = {actual->n1, actual->n2, actual->n3};
        for (int i = 0; i < 3; ++i) {
            if (hijos[i]) {
                cout << opciones+1 << ". " << hijos[i]->nombre << " (" << hijos[i]->tipo << ")\n";
                opciones++;
            }
        }
        if (opciones == 0) {
            cout << "No hay más caminos. Fin del juego.\n";
            break;
        }
        cout << "Elige destino (1-" << opciones << "): ";
        int eleccion;
        cin >> eleccion;
        if (eleccion < 1 || eleccion > opciones) {
            cout << "Opción inválida. Fin del juego.\n";
            break;
        }
        actual = hijos[eleccion-1];
    }

    // Libera memoria
    if (habitaciones != nullptr) {
        delete[] habitaciones;
        habitaciones = nullptr;
    }
    return 0;
};