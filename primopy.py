import math
import time
import multiprocessing

# Constantes
MAX_PROCESOS = 4
N = 1_000_000

# --- Función que verifica si un número es primo ---
def es_primo(num):
    if num < 2:
        return False
    if num == 2:
        return True
    if num % 2 == 0:
        return False
    limite = int(math.sqrt(num)) + 1
    for i in range(3, limite, 2):
        if num % i == 0:
            return False
    return True

# --- Versión secuencial ---
def contar_primos_secuencial(n):
    count = 0
    for i in range(2, n + 1):
        if es_primo(i):
            count += 1
    return count

# --- Función auxiliar para los procesos ---
def contar_primos_rango(rango):
    inicio, fin = rango
    count = 0
    for i in range(inicio, fin + 1):
        if es_primo(i):
            count += 1
    return count

# --- Versión paralela ---
def contar_primos_paralelo(n, num_procesos):
    rango = n // num_procesos
    rangos = []
    for i in range(num_procesos):
        start = i * rango + 1
        end = n if i == num_procesos - 1 else (i + 1) * rango
        rangos.append((start, end))

    with multiprocessing.Pool(processes=num_procesos) as pool:
        resultados = pool.map(contar_primos_rango, rangos)

    return sum(resultados)

# --- Programa principal ---
if __name__ == "__main__":
    print(f"Calculando números primos hasta {N}...\n")

    # Secuencial
    inicio_seq = time.time()
    total_seq = contar_primos_secuencial(N)
    duracion_seq = time.time() - inicio_seq
    print(f"Secuencial -> Primos: {total_seq} | Tiempo: {duracion_seq:.4f} s")

    # Paralelo
    inicio_par = time.time()
    total_par = contar_primos_paralelo(N, MAX_PROCESOS)
    duracion_par = time.time() - inicio_par
    print(f"Paralelo ({MAX_PROCESOS} procesos) -> Primos: {total_par} | Tiempo: {duracion_par:.4f} s")

    # Speedup
    speedup = duracion_seq / duracion_par if duracion_par > 0 else 0
    print(f"\nSpeedup: {speedup:.2f}x")
