Compilación:

g++ main.cpp HeuristicaConstructiva_A.cpp HeuristicaConstructiva_B.cpp BusquedaLocal_A.cpp BusquedaLocal_B.cpp Metaheuristica.cpp Costo.cpp -o main

Ejecución:

./main <input> <output> <algoritmo> [iteraciones] [rcl_size]

Parámetros:

input: archivo de entrada con la instancia GAP.
output: archivo donde se escribe la solución.
algoritmo: método a ejecutar. Puede ser:
    cercano
    demanda
    cercano_relocate
    demanda_relocate
    cercano_swap
    demanda_swap
    grasp

iteraciones: cantidad de iteraciones de GRASP. Solo se usa si algoritmo = grasp.
rcl_size: tamaño de la lista restringida de candidatos de GRASP. Solo se usa si algoritmo = grasp.

Formato de salida:

El archivo de salida contiene una línea por depósito. La línea i contiene los vendedores asignados al depósito i, separados por espacios.