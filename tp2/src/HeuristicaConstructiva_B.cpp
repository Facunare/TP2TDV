#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include "main.cpp"

using namespace std;

// Hecha con ayuda de IA
vector<vector<int>> heuristica_mayor_demanda_primero(const GAPInstance& instance) {
    vector<vector<int>> asignacion(instance.m);
    vector<double> capacidadRestante = instance.capacidades;

    vector<int> vendedores(instance.n);

    for (int j = 0; j < instance.n; j++) {
        vendedores[j] = j;
    }

    sort(vendedores.begin(), vendedores.end(), [&](int a, int b) {
        return instance.demandas[0][a] > instance.demandas[0][b];
    });

    for (int j : vendedores) {
        int mejorDeposito = -1;
        double mejorCosto = numeric_limits<double>::infinity();

        for (int i = 0; i < instance.m; i++) {
            if (instance.demandas[i][j] <= capacidadRestante[i]) {
                if (instance.costos[i][j] < mejorCosto) {
                    mejorCosto = instance.costos[i][j];
                    mejorDeposito = i;
                }
            }
        }

        if (mejorDeposito != -1) {
            asignacion[mejorDeposito].push_back(j);
            capacidadRestante[mejorDeposito] -= instance.demandas[mejorDeposito][j];
        }
    }

    return asignacion;
}