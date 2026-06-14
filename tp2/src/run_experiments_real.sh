#!/bin/bash

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

CSV="results/resultados_real.csv"
echo "instancia,algoritmo,costo,tiempo,iteraciones,rcl_size" > "$CSV"

INSTANCIA="./instances/real/real_instance"

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

echo "Ejecutando instancia real..."
echo ""

for algoritmo in "${ALGORITMOS[@]}"
do
    output="outputs/real_${algoritmo}.txt"

    echo "  Ejecutando $algoritmo..."
    ./main "$INSTANCIA" "$output" "$algoritmo" >> "$CSV"
done

for iter in "${ITERACIONES[@]}"
do
    for rcl in "${RCL_SIZES[@]}"
    do
        output="outputs/real_grasp_${iter}_${rcl}.txt"

        echo "  Ejecutando GRASP iter=$iter rcl=$rcl..."
        ./main "$INSTANCIA" "$output" grasp "$iter" "$rcl" >> "$CSV"
    done
done

echo ""
echo "Experimentación sobre instancia real terminada."
echo "Resultados guardados en: $CSV"
echo "Soluciones guardadas en: outputs/"