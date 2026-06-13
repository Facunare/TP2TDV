#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include "GAP.h"

using namespace std;

GAPInstance readInstance(const string& filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        throw runtime_error("No se pudo abrir el archivo de input: " + filename);
    }

    GAPInstance instance;

    file >> instance.m >> instance.n;

    instance.costos.assign(instance.m, vector<double>(instance.n));
    instance.demandas.assign(instance.m, vector<double>(instance.n));
    instance.capacidades.assign(instance.m, 0);

    // Lectura de matriz de costos: m filas, n columnas
    for (int i = 0; i < instance.m; i++) {
        for (int j = 0; j < instance.n; j++) {
            file >> instance.costos[i][j];
        }
    }

    // Lectura de matriz de demandas: m filas, n columnas
    for (int i = 0; i < instance.m; i++) {
        for (int j = 0; j < instance.n; j++) {
            file >> instance.demandas[i][j];
        }
    }

    // Lectura del vector de capacidades: m valores
    for (int i = 0; i < instance.m; i++) {
        file >> instance.capacidades[i];
    }

    if (file.fail()) {
        throw runtime_error("Error leyendo el archivo. Revisar formato de la instancia.");
    }

    return instance;
}

/*
    asignacion[i] contiene los vendedores asignados al deposito i.

    IMPORTANTE:
    - Internamente usamos indices 0-based.
    - En el output escribimos vendedores 1-based, porque el enunciado habla de vendedores 1..n.
*/

void writeSolution(const string& outputFilename, const vector<vector<int>>& asignacion) {
    ofstream output(outputFilename);

    if (!output.is_open()) {
        throw runtime_error("No se pudo abrir el archivo de output: " + outputFilename);
    }

    for (int i = 0; i < (int)asignacion.size(); i++) {
        for (int k = 0; k < (int)asignacion[i].size(); k++) {
            int vendedor = asignacion[i][k];

            output << vendedor + 1;

            if (k + 1 < (int)asignacion[i].size()) {
                output << " ";
            }
        }

        output << "\n";
    }
}

double calcularCosto(const GAPInstance& instance, const vector<vector<int>>& asignacion) {
    double cmax = 0;

    for (int i = 0; i < instance.m; i++) {
        for (int j = 0; j < instance.n; j++) {
            cmax = max(cmax, instance.costos[i][j]);
        }
    }

    double penalizacion = 3 * cmax;
    double costoTotal = 0;

    vector<bool> asignado(instance.n, false);

    for (int i = 0; i < instance.m; i++) {
        for (int j : asignacion[i]) {
            costoTotal += instance.costos[i][j];
            asignado[j] = true;
        }
    }

    for (int j = 0; j < instance.n; j++) {
        if (!asignado[j]) {
            costoTotal += penalizacion;
        }
    }

    return costoTotal;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        cerr << "Uso:" << endl;
        cerr << "./main <input> <output> <algoritmo> [iteraciones] [rcl_size]" << endl;
        cerr << endl;
        cerr << "Algoritmos disponibles:" << endl;
        cerr << "  cercano" << endl;
        cerr << "  demanda" << endl;
        cerr << "  cercano_relocate" << endl;
        cerr << "  demanda_relocate" << endl;
        cerr << "  cercano_swap" << endl;
        cerr << "  demanda_swap" << endl;
        cerr << "  grasp" << endl;
        return 1;
    }

    string inputFilename = argv[1];
    string outputFilename = argv[2];
    string algoritmo = argv[3];

    int iteraciones = 100;
    int rcl_size = 3;

    if (argc >= 5) {
        iteraciones = stoi(argv[4]);
    }

    if (argc >= 6) {
        rcl_size = stoi(argv[5]);
    }

    try {
        GAPInstance instance = readInstance(inputFilename);

        vector<vector<int>> asignacion;

        auto start = chrono::high_resolution_clock::now();

        if (algoritmo == "cercano") {
            asignacion = heuristica_mas_cercano(instance);
        }
        else if (algoritmo == "demanda") {
            asignacion = heuristica_mayor_demanda_primero(instance);
        }
        else if (algoritmo == "cercano_relocate") {
            asignacion = operador_relocate(instance, heuristica_mas_cercano(instance));
        }
        else if (algoritmo == "demanda_relocate") {
            asignacion = operador_relocate(instance, heuristica_mayor_demanda_primero(instance));
        }
        else if (algoritmo == "cercano_swap") {
            asignacion = operador_swap(instance, heuristica_mas_cercano(instance));
        }
        else if (algoritmo == "demanda_swap") {
            asignacion = operador_swap(instance, heuristica_mayor_demanda_primero(instance));
        }
        else if (algoritmo == "grasp") {
            asignacion = metaheuristica_GRASP(instance, iteraciones, rcl_size);
        }
        else {
            cerr << "Algoritmo no reconocido: " << algoritmo << endl;
            return 1;
        }

        auto end = chrono::high_resolution_clock::now();

        double tiempo = chrono::duration<double>(end - start).count();
        double costo = calcularCosto(instance, asignacion);

        writeSolution(outputFilename, asignacion);

        cout << inputFilename << ","
             << algoritmo << ","
             << costo << ","
             << tiempo << ","
             << iteraciones << ","
             << rcl_size << endl;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}


// https://chatgpt.com/share/6a24adcd-9420-83e9-b183-48781c5d08db
// https://chatgpt.com/share/6a24afbe-c6d0-83e9-b1a9-f3e234a5467c -> heuristica constructiva