#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include "GAP.h"

using namespace std;

/*
    Operador relocate:
    intenta mover un vendedor desde su depósito actual hacia otro depósito.
    El movimiento se aplica solo si reduce el costo y respeta la capacidad.
*/

vector<vector<int>> operador_relocate(const GAPInstance& instance, vector<vector<int>> asignacion_inicial) {
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
    // Repetimos mientras encontremos algún movimiento que mejore la solución.
    while (mejora) {
        mejora = false;
        // Probamos mover cada vendedor asignado.
        for (int j1 = 0; j1 < n; j1++) {
            int i1 = deposito_de[j1];

            // Probamos moverlo a cada depósito distinto del actual.
            for(int i2 = 0; i2 < m; i2++){
                if (i2 == i1) continue; // No tiene sentido intentar mover al mismo depósito
                
                if (i1 == -1 || i2 == -1) continue; 
                
                double costo_actual = instance.costos[i1][j1];
                double nuevo_costo = instance.costos[i2][j1];

                // Solo seguimos si el movimiento mejora el costo.
                if (nuevo_costo < costo_actual) {
                    double demanda_en_i2  = instance.demandas[i2][j1];

                    // Verificamos que el nuevo depósito tenga capacidad suficiente.
                    if (cap_restante[i2] >= demanda_en_i2) {
                        deposito_de[j1] = i2;
                        
                        // Actualizamos las capacidades restantes.
                        cap_restante[i1] += instance.demandas[i1][j1];
                        cap_restante[i2] -= instance.demandas[i2][j1];
                        
                        mejora = true;
                        break; // FIRST IMPROVEMENT: Rompemos el ciclo interior para aplicar la mejora rápido
                    }
                }
            
            // Si ya aplicamos una mejora, volvemos a empezar la búsqueda.
            }if (mejora) {
                break;
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