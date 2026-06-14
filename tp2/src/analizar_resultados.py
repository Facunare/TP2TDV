import pandas as pd
import os

print("Directorio actual:")
print(os.getcwd())
# Cargar CSV
df = pd.read_csv("./results/resultados.csv")

print("="*60)
print("PROMEDIO POR ALGORITMO")
print("="*60)

resumen = (
    df.groupby("algoritmo")
      .agg(
          costo_promedio=("costo", "mean"),
          tiempo_promedio=("tiempo", "mean")
      )
      .sort_values("costo_promedio")
)

print(resumen)

print("\n")
print("="*60)
print("PROMEDIO DE GRASP POR PARAMETROS")
print("="*60)

grasp = df[df["algoritmo"] == "grasp"]

tuning = (
    grasp.groupby(["iteraciones", "rcl_size"])
         .agg(
             costo_promedio=("costo", "mean"),
             tiempo_promedio=("tiempo", "mean")
         )
         .reset_index()
         .sort_values(["iteraciones", "rcl_size"])
)

print(tuning)

print("\n")
print("="*60)
print("MEJOR METODO POR INSTANCIA")
print("="*60)

idx = df.groupby("instancia")["costo"].idxmin()
mejores = df.loc[idx, ["instancia", "algoritmo", "costo"]]

print(mejores.sort_values("instancia"))

print("\n")
print("="*60)
print("MEJOR CONFIGURACION GRASP")
print("="*60)

mejor_grasp = tuning.loc[tuning["costo_promedio"].idxmin()]
print(mejor_grasp)