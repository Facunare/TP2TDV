#!/bin/bash

# ============================================================
# Script de experimentación para GAP
# Genera outputs de soluciones y un CSV con costo y tiempo.
#
# Uso desde la carpeta src:
#   chmod +x run_experiments.sh
#   ./run_experiments.sh
# ============================================================

set -e

mkdir -p outputs
mkdir -p results

echo "Compilando..."

g++ -std=c++17 main.cpp \
    HeuristicaConstructiva_A.cpp \
    HeuristicaConstructiva_B.cpp \
    BusquedaLocal_A.cpp \
    BusquedaLocal_B.cpp \
    Metaheuristica.cpp \
    -o main

CSV="results/resultados.csv"
echo "instancia,algoritmo,costo,tiempo,iteraciones,rcl_size" > "$CSV"

ALGORITMOS=(
    "cercano"
    "demanda"
    "cercano_relocate"
    "demanda_relocate"
    "cercano_swap"
    "demanda_swap"
)

ITERACIONES=(50 100 200)
RCL_SIZES=(2 3 5)

echo "Comenzando experimentación..."
echo ""

for instancia in ./instances/gap/gap_a/* ./instances/gap/gap_b/* ./instances/gap/gap_e/*
do
    if [ ! -f "$instancia" ]; then
        continue
    fi

    nombre=$(basename "$instancia")

    echo "Instancia: $instancia"

    for algoritmo in "${ALGORITMOS[@]}"
    do
        output="outputs/${nombre}_${algoritmo}.txt"

        echo "  Ejecutando $algoritmo..."
        ./main "$instancia" "$output" "$algoritmo" >> "$CSV"
    done

    for iter in "${ITERACIONES[@]}"
    do
        for rcl in "${RCL_SIZES[@]}"
        do
            output="outputs/${nombre}_grasp_${iter}_${rcl}.txt"

            echo "  Ejecutando GRASP iter=$iter rcl=$rcl..."
            ./main "$instancia" "$output" grasp "$iter" "$rcl" >> "$CSV"
        done
    done

    echo ""
done

echo "Experimentación terminada."
echo "Resultados guardados en: $CSV"
echo "Soluciones guardadas en: outputs/"