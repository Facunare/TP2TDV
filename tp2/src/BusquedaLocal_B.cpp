#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

/*
    Operador swap:
    intenta intercambiar los depósitos de dos vendedores.
    El cambio se aplica solo si reduce el costo total y respeta las capacidades.
*/

vector<vector<int>> operador_swap(const GAPInstance& instance, vector<vector<int>> asignacion_inicial) {
    int m = instance.m;
    int n = instance.n;

    // deposito_de[j] indica en qué depósito está asignado el vendedor j.
    // Si vale -1, el vendedor no está asignado.

    vector<int> deposito_de(n, -1);
    vector<double> cap_restante = instance.capacidades; // Copiamos las capacidades para ir actualizando el espacio disponible.

    // Construimos deposito_de y calculamos la capacidad restante inicial.
    for (int i = 0; i < m; i++) {
        for (int j : asignacion_inicial[i]) {
            deposito_de[j] = i;
            cap_restante[i] -= instance.demandas[i][j];
        }
    }

    bool mejora = true; 
    // Repetimos mientras encontremos algún intercambio que mejore la solución.
    while (mejora) {
        mejora = false;
        
        // Exploramos el Vecindario N(s*): probamos todos los pares de vendedores j1 y j2
        for (int j1 = 0; j1 < n; j1++) {
            for (int j2 = j1 + 1; j2 < n; j2++) {
                
                int i1 = deposito_de[j1]; // Depósito actual de j1
                int i2 = deposito_de[j2]; // Depósito actual de j2

                // Si alguno no tiene depósito asignado, o ya están en el mismo, los ignoramos
                if (i1 == -1 || i2 == -1 || i1 == i2) {
                    continue; 
                }

                // elegir s en N(s*) tal que f(s) < f(s*)...
                double costo_actual = instance.costos[i1][j1] + instance.costos[i2][j2]; // Costo actual de mantener a cada vendedor en su depósito.
                double nuevo_costo = instance.costos[i2][j1] + instance.costos[i1][j2]; // Costo si intercambiamos los depósitos de ambos vendedores.

                 // Solo seguimos si el intercambio mejora el costo.
                if (nuevo_costo < costo_actual) {
                    
                    // La capacidad necesaria en i2 es la demanda de j1 que entra, menos la de j2 que sale.
                    double cap_req_i2 = instance.demandas[i2][j1] - instance.demandas[i2][j2];
                    // La capacidad necesaria en i1 es la demanda de j2 que entra, menos la de j1 que sale.
                    double cap_req_i1 = instance.demandas[i1][j2] - instance.demandas[i1][j1];

                    // Verificamos que ambos depósitos tengan capacidad suficiente.
                    if (cap_restante[i2] >= cap_req_i2 && cap_restante[i1] >= cap_req_i1) {
                        
                        // Aplicamos el intercambio
                        deposito_de[j1] = i2;
                        deposito_de[j2] = i1;
                        
                        // Actualizamos el espacio libre de cada depósito
                        cap_restante[i1] -= cap_req_i1;
                        cap_restante[i2] -= cap_req_i2;
                        
                        mejora = true; 
                        break; // FIRST IMPROVEMENT
                    }
                }
            }
            if (mejora) {
                break; // Rompemos el ciclo exterior para reiniciar la exploración desde la nueva solución s*
            }
        }
    }

    // Reconstruimos la asignación final en formato vector de depósitos.
    vector<vector<int>> nueva_asignacion(m);
    for (int j = 0; j < n; j++) {
        if (deposito_de[j] != -1) {
            nueva_asignacion[deposito_de[j]].push_back(j);
        }
    }
    return nueva_asignacion;
}