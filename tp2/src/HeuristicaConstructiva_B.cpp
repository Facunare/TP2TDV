#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include "GAP.h"

using namespace std;

// Hecha con ayuda de IA (solo el sort de vendedores)
vector<vector<int>> heuristica_mayor_demanda_primero(const GAPInstance& instance) {
    vector<vector<int>> asignacion(instance.m); // asignacion[i] guarda los vendedores asignados al depósito i
    vector<double> capacidadRestante = instance.capacidades;

    // Lista de vendedores para iterar
    vector<int> vendedores(instance.n);
    for (int j = 0; j < instance.n; j++) {
        vendedores[j] = j;
    }

    // Actualizamos el orden de los vendedores para procesar primero los de mayor demanda
    sort(vendedores.begin(), vendedores.end(), [&](int a, int b) {
        return instance.demandas[0][a] > instance.demandas[0][b];
    });

    // Recorremos cada vendedor en el orden original
    for (int j : vendedores) {
        int mejorDeposito = -1;
        double mejorCosto = numeric_limits<double>::infinity();

        // Buscamos el depósito más cercano que tenga capacidad suficiente
        for (int i = 0; i < instance.m; i++) {
            if (instance.demandas[i][j] <= capacidadRestante[i]) {
                if (instance.costos[i][j] < mejorCosto) {
                    mejorCosto = instance.costos[i][j];
                    mejorDeposito = i;
                }
            }
        }

        // Si encontramos un depósito factible, asignamos el vendedor y actualizamos la capacidad restante
        if (mejorDeposito != -1) {
            asignacion[mejorDeposito].push_back(j);
            capacidadRestante[mejorDeposito] -= instance.demandas[mejorDeposito][j];
        }

        // Si mejorDeposito queda en -1, el vendedor queda sin asignar.
        // La penalización se calcula después al evaluar el costo total.
    }

    return asignacion;
}