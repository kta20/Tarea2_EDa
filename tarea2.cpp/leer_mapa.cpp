#include "leer_mapa.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>

// Borra estas dos funciones de la parte superior de leer_mapa.cpp:
// enemigo** leer_enemigos_globales(string filename, int& total_enemigos) {
// total_enemigos = 0;
// return NULL;
// }

// evento** leer_eventos_globales(string filename, int& total_eventos) {
// total_eventos = 0;
// return NULL;
// }

estacion** LectorMapa::leer_habitaciones(string filename, int& total_habitaciones, ArbolTernario& arbol) {
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
    enemigo** enemigos_globales = leer_enemigos_globales(filename, total_enemigos);
    int total_eventos = 0;
    evento** eventos_globales = leer_eventos_globales(filename, total_eventos);
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
                string nombre = linea.substr(pos1 + 1, pos2 - pos1 - 1);
                string tipo = linea.substr(pos2 + 1, pos3 - pos2 - 1);
                string descripcion = linea.substr(pos3 + 1);
                int id = stoi(id_str);
                habitaciones[id] = arbol.crear_estacion(id, nombre, tipo, descripcion);
                if (tipo.find("EVENTO") != string::npos) {
                    int idx = rand() % total_eventos;
                    habitaciones[id]->evento_asociado = eventos_globales[idx];
                } else {
                    habitaciones[id]->evento_asociado = NULL;
                }
                // NUEVO: Si es COMBATE, asigna enemigos aleatorios
                if (tipo.find("COMBATE") != string::npos) {
                    // Decide cuántos enemigos (ejemplo: 1 o 2 aleatorio)
                    int cant = 1 + rand() % 2;
                    habitaciones[id]->cantidad_enemigos = cant;
                    habitaciones[id]->enemigos = new enemigo*[cant];
                    for (int j = 0; j < cant; ++j) {
                        // Selecciona enemigo aleatorio según probabilidad de spawn
                        float r = (float)rand() / RAND_MAX;
                        float suma = 0;
                        for (int k = 0; k < total_enemigos; ++k) {
                            suma += enemigos_globales[k]->prob_spawn;
                            if (r <= suma) {
                                // Clona el enemigo para que cada uno tenga su propia vida
                                habitaciones[id]->enemigos[j] = new enemigo(*enemigos_globales[k]);
                                break;
                            }
                        }
                        // Si no se asignó por probabilidad, asigna el último
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
    // Libera enemigos_globales después si quieres
    return habitaciones;
}

// Lee todos los enemigos definidos en el archivo y retorna un arreglo
enemigo** leer_enemigos_globales(string filename, int& total_enemigos) {
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

evento** leer_eventos_globales(string filename, int& total_eventos) {
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

// Lee los arcos desde el archivo y conecta las habitaciones
void LectorMapa::leer_y_conectar_arcos(string filename, estacion** habitaciones, int total_habitaciones, ArbolTernario& arbol) {
    ifstream archivo;
    archivo.open(filename.c_str());
    if (!archivo.is_open()) {
        cout << "No se ha podido abrir el archivo para leer arcos\n";
        return;
    }

    string linea;
    // Buscar la sección ARCOS
    while (getline(archivo, linea)) {
        if (linea.find("ARCOS|") == 0) {
            int pos = linea.find('|');
            if (pos == -1) {
                cout << "Error: línea sin '|': " << linea << endl;
                return;
            }
            string arcos_str = linea.substr(pos + 1);
            arcos_str.erase(0, arcos_str.find_first_not_of(" \t\r\n")); // quita espacios
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
estacion* LectorMapa::obtener_raiz(estacion** habitaciones) {
    return habitaciones[0];
}