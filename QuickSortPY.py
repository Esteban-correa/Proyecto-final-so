#!/usr/bin/env python3
# quicksort_comparacion.py
# Uso: python3 quicksort_comparacion.py <tamaño_del_arreglo> <profundidad_de_paralelismo>
# Ejemplo: python3 quicksort_comparacion.py 100000 3

import sys
import random
import time
from concurrent.futures import ProcessPoolExecutor, as_completed

# ---------------- QuickSort secuencial (in-place) ----------------
def quicksort_secuencial(arr, lo=0, hi=None):
    if hi is None:
        hi = len(arr) - 1
    if lo < hi:
        p = partition(arr, lo, hi)
        quicksort_secuencial(arr, lo, p - 1)
        quicksort_secuencial(arr, p + 1, hi)

def partition(arr, lo, hi):
    pivot = arr[hi]
    i = lo - 1
    for j in range(lo, hi):
        if arr[j] < pivot:
            i += 1
            arr[i], arr[j] = arr[j], arr[i]
    arr[i + 1], arr[hi] = arr[hi], arr[i + 1]
    return i + 1

# ---------------- QuickSort paralelo (funcional, devuelve lista ordenada) ----------------
# estrategia: dividir en left/pivot/right por valor de pivote; si depth>0, lanzar tareas en procesos;
# cuando depth==0, ordenamos secuencialmente localmente.
def parallel_quicksort(arr, depth, executor=None):
    # caso base
    if len(arr) < 2:
        return arr

    pivot = arr[len(arr) // 2]
    left = [x for x in arr if x < pivot]
    middle = [x for x in arr if x == pivot]
    right = [x for x in arr if x > pivot]

    if depth > 0 and executor is not None:
        # submit two tasks: left and right
        fut_left = executor.submit(parallel_quicksort, left, depth - 1, None)
        fut_right = executor.submit(parallel_quicksort, right, depth - 1, None)
        left_sorted = fut_left.result()
        right_sorted = fut_right.result()
    elif depth > 0:
        # no executor provided -> create a temporary one (rare path)
        with ProcessPoolExecutor(max_workers=2) as tmp_exec:
            fut_left = tmp_exec.submit(parallel_quicksort, left, depth - 1, None)
            fut_right = tmp_exec.submit(parallel_quicksort, right, depth - 1, None)
            left_sorted = fut_left.result()
            right_sorted = fut_right.result()
    else:
        # depth == 0: do local sequential sorts
        quicksort_secuencial(left, 0, len(left)-1 if left else -1)
        quicksort_secuencial(right, 0, len(right)-1 if right else -1)
        left_sorted = left
        right_sorted = right

    return left_sorted + middle + right_sorted

# ---------------- Main ----------------
def main():
    if len(sys.argv) != 3:
        print("Uso: python3 quicksort_comparacion.py <tamaño_del_arreglo> <profundidad_de_paralelismo>")
        sys.exit(1)

    n = int(sys.argv[1])
    depth = int(sys.argv[2])

    # reproducibilidad
    random.seed(42)
    arr = [random.randrange(0, 100_000) for _ in range(n)]
    arr_seq = arr.copy()
    arr_par = arr.copy()

    # Secuencial
    t0 = time.perf_counter()
    quicksort_secuencial(arr_seq)
    t1 = time.perf_counter()
    tiempo_seq = t1 - t0

    # Paralelo: creamos un executor y lanzamos la función paralela con él
    t2 = time.perf_counter()
    # elegimos max_workers = 2**depth pero limitada para evitar demasiados procesos
    max_workers = min(2 ** depth,  (os_cpu_count := None) or  (2 ** depth))
    # preferir usar cpu_count
    try:
        import os
        os_c = os.cpu_count() or 1
        max_workers = min(2 ** depth, os_c * 2)  # to avoid excessive oversubscription
    except Exception:
        max_workers = min(2 ** depth, 8)

    with ProcessPoolExecutor(max_workers=max_workers) as executor:
        sorted_par = parallel_quicksort(arr_par, depth, executor)
    t3 = time.perf_counter()
    tiempo_par = t3 - t2

    speedup = tiempo_seq / tiempo_par if tiempo_par > 0 else float('inf')

    print("\n===== RESULTADOS =====")
    print(f"Tamaño del arreglo: {n}")
    print(f"Profundidad de paralelismo: {depth}")
    print(f"Tiempo secuencial: {tiempo_seq:.6f} s")
    print(f"Tiempo paralelo: {tiempo_par:.6f} s")
    print(f"Speedup: {speedup:.2f}x")

if __name__ == "__main__":
    main()
