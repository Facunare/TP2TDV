#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

struct GAPInstance {
    int m; // cantidad de depositos
    int n; // cantidad de vendedores

    vector<vector<double>> costos;     // costos[i][j] = costo/distancia deposito i -> vendedor j
    vector<vector<double>> demandas;   // demandas[i][j] = demanda si vendedor j va al deposito i
    vector<double> capacidades;        // capacidades[i] = capacidad del deposito i
};

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

/*
    Esta funcion NO es una heuristica.
    Solo crea una solucion vacia para que el programa compile y genere un output valido
    en cuanto al formato.

    Mas adelante, reemplazar esta funcion por algo como:

    vector<vector<int>> asignacion = heuristicaConstructiva1(instance);

    o:

    vector<vector<int>> asignacion = metaheuristica(instance);
*/

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

vector<vector<int>> heuristica_mas_cercano(const GAPInstance& instance) {
    vector<vector<int>> asignacion(instance.m);
    vector<double> capacidadRestante = instance.capacidades;

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

int main(int argc, char** argv) {
    string inputFilename = "instances/gap/gap_a/a05100.txt";
    string outputFilename = "instances/gap/gap_a/a05100 salida.txt";

    /*
        Si se pasan argumentos por consola, se usan esos.
        Ejemplo:
        ./main instances/gap/gap_a/a05100.txt "instances/gap/gap_a/a05100 salida.txt"
    */
    if (argc >= 2) {
        inputFilename = argv[1];
    }

    if (argc >= 3) {
        outputFilename = argv[2];
    }

    cout << "Reading file " << inputFilename << endl;

    try {
        GAPInstance instance = readInstance(inputFilename);

        cout << "Instancia leida correctamente." << endl;
        cout << "Depositos: " << instance.m << endl;
        cout << "Vendedores: " << instance.n << endl;

        /*
            ACA DESPUES VA LA MAGIA.

            Por ahora dejamos una solucion vacia.
            Mas adelante reemplazar esta linea por la heuristica correspondiente:

            vector<vector<int>> asignacion = heuristicaConstructiva1(instance);
            vector<vector<int>> asignacion = heuristicaConstructiva2(instance);
            vector<vector<int>> asignacion = busquedaLocal(instance, solucionInicial);
            vector<vector<int>> asignacion = metaheuristica(instance);
        */
        vector<vector<int>> asignacion = heuristica_mayor_demanda(instance);

        writeSolution(outputFilename, asignacion);

        cout << "Solucion escrita en " << outputFilename << endl;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}


// https://chatgpt.com/share/6a24adcd-9420-83e9-b183-48781c5d08db
// https://chatgpt.com/share/6a24afbe-c6d0-83e9-b1a9-f3e234a5467c -> heuristica constructiva