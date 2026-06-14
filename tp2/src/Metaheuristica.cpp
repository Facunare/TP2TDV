#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <utility>
#include <cstdlib>
#include <random>
#include "GAP.h"
using namespace std;

void selectionSort(vector<pair<double,int>>& v, int k) { // Selection Sort para ordenar los costos de asignación. Se ordena hasta el k-ésimo elemento
    int n = v.size();

    for (int i = 0; i < min(k, n - 1); i++) {

        int minIndex = i;

        // Buscar el mínimo por el campo .first (double)
        for (int j = i + 1; j < n; j++) {
            if (v[j].first < v[minIndex].first) {
                minIndex = j;
            }
        }

        // Intercambiar
        swap(v[i], v[minIndex]);
    }
}

void merge(vector<pair<double,int>>& v, int left, int mid, int right) { // Función de merge para Merge Sort

    vector<pair<double,int>> aux;

    int i = left;
    int j = mid + 1;

    while (i <= mid && j <= right) {

        if (v[i].first <= v[j].first) {
            aux.push_back(v[i]);
            i++;
        } else {
            aux.push_back(v[j]);
            j++;
        }
    }

    while (i <= mid) {
        aux.push_back(v[i]);
        i++;
    }

    while (j <= right) {
        aux.push_back(v[j]);
        j++;
    }

    for (int k = 0; k < aux.size(); k++) {
        v[left + k] = aux[k];
    }
}

void mergeSort(vector<pair<double,int>>& v, int left, int right) { // Merge Sort para ordenar los costos de asignación

    if (left >= right)
        return;

    int mid = left + (right - left) / 2;

    mergeSort(v, left, mid);

    mergeSort(v, mid + 1, right);

    merge(v, left, mid, right);
}

void mezclarMiVector(vector<int>& v) { // Función para mezclar el orden de los vendedores de forma randomizada
    static mt19937 g(random_device{}()); // Generador de números aleatorios
    
    shuffle(v.begin(), v.end(), g); // Mezclar el vector utilizando el generador aleatorio
}

int randomizar_indice(int n) { // Función para seleccionar un índice de forma aleatoria para seleccionar un elemento del RCL
    if(n<=0){ // Si el tamaño del RCL es 0 o negativo, no se puede seleccionar un índice válido
        return 0;
    }
    static mt19937 g(random_device{}()); // Generador de números aleatorios
    uniform_int_distribution<int> dist(0, n - 1); // Distribución uniforme para obtener un índice entre 0 y n-1
    return dist(g);
}

vector<pair<double,int>> armadoRCL(const vector<pair<double,int>>& costos_ordenados, int rcl_size, const vector<double>& capacidadRestante, const vector<vector<double>>& demandas, int j){ // Función para construir el RCL. Contiene los primeros "rcl_size" costos de asignación más bajos (y que sean factibles). Cada elemento es un par (costo, indice del deposito)
    vector<pair<double,int>> rcl;
    int i = 0;
    while(rcl.size() < rcl_size && i < costos_ordenados.size()){ // Agregar al RCL los costos de asignación más bajos hasta alcanzar el tamaño máximo del RCL o hasta que no haya más costos disponibles
        if(capacidadRestante[costos_ordenados[i].second] >= demandas[costos_ordenados[i].second][j]){ // Verificar que la asignación sea factible antes de agregarla al RCL
            rcl.push_back(costos_ordenados[i]);
        }
        i++;
    }
    return rcl;
}


vector<vector<int>> GreedyRandomized(const GAPInstance& instance, int rcl_size){ // Función para construir una solución inicial utilizando el enfoque GRASP
    vector<vector<int>> asignacion(instance.m);
    vector<double> capacidadRestante = instance.capacidades;
    vector<int> vendedores;
    for(int j=0; j<instance.n; j++){
        vendedores.push_back(j);
    }
    mezclarMiVector(vendedores); // Mezclar el orden de los vendedores para aleatorizar aun más la construcción

    for(int j : vendedores){
        vector<pair<double, int>> costos_ordenados;
        for(int i=0; i<instance.m;i++){
            costos_ordenados.push_back({instance.costos[i][j], i});
        }
        mergeSort(costos_ordenados, 0, costos_ordenados.size() - 1); // Ordenar los costos de asignación para el vendedor j utilizando Merge Sort

        //selectionSort(costos_ordenados, rcl_size); // Ordenar los costos de asignación para el vendedor j utilizando Selection Sort

        vector<pair<double, int>> rcl = armadoRCL(costos_ordenados, rcl_size, capacidadRestante, instance.demandas, j); // Armado del RCL para el vendedor j
        if(rcl.empty()){ // Si el RCL está vacío, significa que no se pudo asignar el vendedor j a ningún depósito de forma factible. Entonces, se reinicia la construcción
            return GreedyRandomized(instance, rcl_size); // Si no hay opciones en el RCL, reiniciar la construcción
        }
        int indice_random = randomizar_indice(rcl.size()); // Seleccionar un índice aleatorio para elegir un candidato del RCL
        int eleccion = rcl[indice_random].second; // Candidato seleccionado del RCL
        asignacion[eleccion].push_back(j);
        capacidadRestante[eleccion] -= instance.demandas[eleccion][j];
    }

    return asignacion;
}


vector<vector<int>> metaheuristica_GRASP(const GAPInstance& instance,int n, int rcl_size){ // Función principal de la metaheurística GRASP
    vector<vector<int>> mejor_solucion;
    double mejor_costo = numeric_limits<double>::infinity();
    for(int i=0; i<n; i++){
        vector<vector<int>> solucion_inicial = GreedyRandomized(instance, rcl_size); // Construcción de una solución inicial utilizando el enfoque GRASP

        vector<vector<int>> solucion_mejorada = operador_relocate(instance, solucion_inicial); // Se le aplica búsqueda local a la solución inicial
        double costo_solucion = calcularCosto(instance, solucion_mejorada);

        if(costo_solucion < mejor_costo){ // Se actualiza la mejor solución encontrada hasta el momento
            mejor_costo = costo_solucion;
            mejor_solucion = solucion_mejorada;
        }
    }
    return mejor_solucion;
}


/* 
Uso de IA:

- Implementación de algoritmos de ordenamiento (Selection Sort y Merge Sort) para ordenar los costos de asignación.
- Uso de la función shuffle para mezclar el orden de los vendedores.
- Implementación de una función para generar un índice aleatorio dentro de un rango específico.
- Arreglo del caso en que no se puede asignar un vendedor a ningún depósito de forma factible (RCL.size() == 0). Evitar que se haga una recursión infinita al reiniciar la construcción.

chat: https://share.gemini.google/wOHoWd2hsfGXv 
*/