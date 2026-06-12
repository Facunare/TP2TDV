#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

/*
 * OPERADOR SWAP (BÚSQUEDA LOCAL)
 * Intercambia los depósitos asignados entre dos vendedores distintos
 * si eso mejora la distancia total y respeta las capacidades.
 */
vector<vector<int>> operador_swap(const GAPInstance& instance, vector<vector<int>> asignacion_inicial) {
    int m = instance.m;
    int n = instance.n;

    // ========================================================================
    // 1. "Construir una solución s* en S" (Adaptación de la matriz inicial)
    // ========================================================================
    // Pasamos de matriz a un vector 1D para saber en O(1) en qué depósito está cada vendedor.
    vector<int> deposito_de(n, -1);
    vector<double> cap_restante = instance.capacidades;

    for (int i = 0; i < m; i++) {
        for (int j : asignacion_inicial[i]) {
            deposito_de[j] = i;
            // Calculamos cuánto espacio libre le queda a cada depósito
            cap_restante[i] -= instance.demandas[i][j];
        }
    }

    // ========================================================================
    // 2 y 4. "repetir" y "mientras exista s en N(s*) tal que f(s) < f(s*)"
    // ========================================================================
    bool mejora = true; // Forzamos la entrada al ciclo la primera vez
    
    while (mejora) {
        mejora = false; // Asumimos que no hay mejora hasta que demostremos lo contrario
        
        // Exploramos el Vecindario N(s*): probamos todos los pares de vendedores j1 y j2
        for (int j1 = 0; j1 < n; j1++) {
            for (int j2 = j1 + 1; j2 < n; j2++) {
                
                int i1 = deposito_de[j1]; // Depósito actual de j1
                int i2 = deposito_de[j2]; // Depósito actual de j2

                // Si alguno no tiene depósito asignado, o ya están en el mismo, los ignoramos
                if (i1 == -1 || i2 == -1 || i1 == i2) {
                    continue; 
                }

                // ========================================================================
                // 3. "elegir s en N(s*) tal que f(s) < f(s*)..."
                // ========================================================================
                // A. CHEQUEO DE MEJORA: ¿Baja el costo si los cruzamos?
                double costo_actual = instance.costos[i1][j1] + instance.costos[i2][j2];
                double nuevo_costo = instance.costos[i2][j1] + instance.costos[i1][j2];

                if (nuevo_costo < costo_actual) {
                    
                    // B. CHEQUEO DE FACTIBILIDAD: ¿Entran en los nuevos depósitos?
                    // La capacidad necesaria en i2 es la demanda de j1 que entra, menos la de j2 que sale.
                    double cap_req_i2 = instance.demandas[i2][j1] - instance.demandas[i2][j2];
                    // La capacidad necesaria en i1 es la demanda de j2 que entra, menos la de j1 que sale.
                    double cap_req_i1 = instance.demandas[i1][j2] - instance.demandas[i1][j1];

                    // Verificamos que las capacidades remanentes soporten el cambio
                    if (cap_restante[i2] >= cap_req_i2 && cap_restante[i1] >= cap_req_i1) {
                        
                        // "... y actualizar s* = s"
                        // Aplicamos el intercambio
                        deposito_de[j1] = i2;
                        deposito_de[j2] = i1;
                        
                        // Actualizamos el espacio libre de cada depósito
                        cap_restante[i1] -= cap_req_i1;
                        cap_restante[i2] -= cap_req_i2;
                        
                        mejora = true; // Encontramos una mejora, marcamos la bandera para repetir el while
                        break; // FIRST IMPROVEMENT: Rompemos el ciclo interior para aplicar la mejora rápido
                    }
                }
            }
            if (mejora) {
                break; // Rompemos el ciclo exterior para reiniciar la exploración desde la nueva solución s*
            }
        }
    } // Fin del while: Si 'mejora' es false, llegamos a un mínimo local y termina la búsqueda.

    // Reconstruimos la matriz de asignación final para el output
    vector<vector<int>> nueva_asignacion(m);
    for (int j = 0; j < n; j++) {
        if (deposito_de[j] != -1) {
            nueva_asignacion[deposito_de[j]].push_back(j);
        }
    }
    return nueva_asignacion;
}