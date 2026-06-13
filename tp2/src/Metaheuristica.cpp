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

void selectionSort(vector<pair<double,int>>& v) {
    int n = v.size();

    for (int i = 0; i < n - 1; i++) {

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

void merge(vector<pair<double,int>>& v, int left, int mid, int right) {

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

    for (int k = 0; k < (int)aux.size(); k++) {
        v[left + k] = aux[k];
    }
}

void mergeSort(vector<pair<double,int>>& v, int left, int right) {

    if (left >= right)
        return;

    int mid = left + (right - left) / 2;

    mergeSort(v, left, mid);

    mergeSort(v, mid + 1, right);

    merge(v, left, mid, right);
}

void mezclarMiVector(vector<int>& v) {
    static mt19937 g(random_device{}()); 
    
    shuffle(v.begin(), v.end(), g);
}

int randomizar_indice(int size) {
    static mt19937 g(random_device{}()); 
    uniform_int_distribution<int> dist(0, size - 1);
    return dist(g);
}

vector<pair<double,int>> armadoRCL(const vector<pair<double,int>>& costos_ordenados, int rcl_size, const vector<double>& capacidadRestante, const vector<vector<double>>& demandas, int j){
    vector<pair<double,int>> rcl;
    int i = 0;
    while(rcl.size() < rcl_size && i < costos_ordenados.size()){
        if(capacidadRestante[costos_ordenados[i].second] >= demandas[costos_ordenados[i].second][j]){
            rcl.push_back(costos_ordenados[i]);
        }
        i++;
    }
    return rcl;
}


vector<vector<int>> GreedyRandomized(const GAPInstance& instance, int rcl_size){
    vector<vector<int>> asignacion(instance.m);
    vector<double> capacidadRestante = instance.capacidades;
    vector<int> vendedores;
    for(int j=0; j<instance.n; j++){
        vendedores.push_back(j);
    }
    mezclarMiVector(vendedores); // Mezclar el orden de los vendedores para aleatorizar aun mas la construcción

    for(int j : vendedores){
        vector<pair<double, int>> costos_ordenados;
        for(int i=0; i<instance.m;i++){
            costos_ordenados.push_back({instance.costos[i][j], i});
        }
        mergeSort(costos_ordenados, 0, costos_ordenados.size() - 1);

        vector<pair<double, int>> rcl = armadoRCL(costos_ordenados, rcl_size, capacidadRestante, instance.demandas, j);
        // if(rcl.empty()){
        //     return GreedyRandomized(instance, rcl_size); // Si no hay opciones en el RCL, reiniciar la construcción
        // }
        if (rcl.empty()) {
            continue;
        }
        int indice_random = randomizar_indice(rcl.size());
        int eleccion = rcl[indice_random].second;
        asignacion[eleccion].push_back(j);
        capacidadRestante[eleccion] -= instance.demandas[eleccion][j];
    }

    return asignacion;
}


vector<vector<int>> metaheuristica_GRASP(const GAPInstance& instance,int n, int rcl_size){
    vector<vector<int>> mejor_solucion;
    double mejor_costo = numeric_limits<double>::infinity();
    for(int i=0; i<n; i++){
        vector<vector<int>> solucion_inicial = GreedyRandomized(instance, rcl_size);

        vector<vector<int>> solucion_mejorada = operador_relocate(instance, solucion_inicial);
        double costo_solucion = calcularCosto(instance, solucion_mejorada);

        if(costo_solucion < mejor_costo){
            mejor_costo = costo_solucion;
            mejor_solucion = solucion_mejorada;
        }
    }
    return mejor_solucion;
}
