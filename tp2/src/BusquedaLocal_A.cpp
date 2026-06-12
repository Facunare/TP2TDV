#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

vector<vector<int>> operador_relocate(const GAPInstance& instance, vector<vector<int>> asignacion_inicial) {
    int m = instance.m;
    int n = instance.n;

    vector<int> deposito_de(n, -1);
    vector<double> cap_restante = instance.capacidades;

    for (int i = 0; i < m; i++) {
        for (int j : asignacion_inicial[i]) {
            deposito_de[j] = i;
            cap_restante[i] -= instance.demandas[i][j];
        }
    }

    bool mejora = true;
    
    while (mejora) {
        mejora = false;
        
        for (int j1 = 0; j1 < n; j1++) {
            int i1 = deposito_de[j1];
            for(int i2 = 0; i2 < m; i2++){
                if (i2 == i1) continue; // No tiene sentido intentar mover al mismo depósito
                
                if (i1 == -1 || i2 == -1) continue; 
                
                double costo_actual = instance.costos[i1][j1];
                double nuevo_costo = instance.costos[i2][j1];

                if (nuevo_costo < costo_actual) {
                    double demanda_en_i2  = instance.demandas[i2][j1];

                    if (cap_restante[i2] >= demanda_en_i2) {
                        deposito_de[j1] = i2;
                        
                        cap_restante[i1] += instance.demandas[i1][j1];
                        cap_restante[i2] -= instance.demandas[i2][j1];
                        
                        mejora = true;
                        break; 
                    }
                }


                
            }if (mejora) {
                break;
            }
        }
    } 

    vector<vector<int>> nueva_asignacion(m);
    for (int j = 0; j < n; j++) {
        if (deposito_de[j] != -1) {
            nueva_asignacion[deposito_de[j]].push_back(j);
        }
    }
    return nueva_asignacion;
}