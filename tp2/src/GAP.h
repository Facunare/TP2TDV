#ifndef GAP_H
#define GAP_H

#include <vector>
#include <string>

using namespace std;

struct GAPInstance {
    int m; // cantidad de depositos
    int n; // cantidad de vendedores

    vector<vector<double>> costos;     // costos[i][j]
    vector<vector<double>> demandas;   // demandas[i][j]
    vector<double> capacidades;        // capacidades[i]
};

// Lectura y escritura
GAPInstance readInstance(const string& filename);

void writeSolution(
    const string& outputFilename,
    const vector<vector<int>>& asignacion
);

// Heuristicas constructivas
vector<vector<int>> heuristica_mas_cercano(const GAPInstance& instance);

vector<vector<int>> heuristica_mayor_demanda_primero(const GAPInstance& instance);

// Busqueda local
vector<vector<int>> operador_relocate(
    const GAPInstance& instance,
    vector<vector<int>> asignacion_inicial
);

vector<vector<int>> operador_swap(
    const GAPInstance& instance,
    vector<vector<int>> asignacion_inicial
);

// Metaheuristica
vector<vector<int>> metaheuristica_GRASP(
    const GAPInstance& instance,
    int n,
    int rcl_size
);

// Evaluacion de soluciones
double calcularCosto(
    const GAPInstance& instance,
    const vector<vector<int>>& asignacion
);

#endif