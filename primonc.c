Codigo C
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define N 1000000 // Límite hasta donde se calculan los primos

// Estructura para pasar datos a los hilos
typedef struct {
    int start;
    int end;
    int count;
} ThreadData;

// Función que verifica si un número es primo
int esPrimo(int num) {
    if (num < 2) return 0;
    if (num == 2) return 1;
    if (num % 2 == 0) return 0;
    for (int i = 3; i <= sqrt(num); i += 2)
        if (num % i == 0) return 0;
    return 1;
}

// --- Versión secuencial ---
int contarPrimosSecuencial(int n) {
    int count = 0;
    for (int i = 2; i <= n; i++)
        if (esPrimo(i)) count++;
    return count;
}

// --- Versión paralela ---
void* contarPrimosParalelo(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    data->count = 0;
    for (int i = data->start; i <= data->end; i++)
        if (esPrimo(i)) data->count++;
    pthread_exit(NULL);
}

int ejecutarParalelo(int numThreads, int n) {
    pthread_t threads[numThreads];
    ThreadData datos[numThreads];
    int rango = n / numThreads;
    int totalParalelo = 0;

    for (int i = 0; i < numThreads; i++) {
        datos[i].start = i * rango + 1;
        datos[i].end = (i == numThreads - 1) ? n : (i + 1) * rango;
        pthread_create(&threads[i], NULL, contarPrimosParalelo, (void*)&datos[i]);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        totalParalelo += datos[i].count;
    }

    return totalParalelo;
}

int main() {
    clock_t start, end;
    double tiempoSecuencial;

    printf("Calculando primos hasta %d...\n", N);

    // ---- Cálculo secuencial ----
    start = clock();
    int totalSecuencial = contarPrimosSecuencial(N);
    end = clock();
    tiempoSecuencial = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Secuencial -> Primos: %d | Tiempo: %.4f s\n\n", totalSecuencial, tiempoSecuencial);

    // ---- Pruebas con distintos números de hilos ----
    int hilosPrueba[] = {1, 2, 4, 8, 16};
    int numPruebas = sizeof(hilosPrueba) / sizeof(hilosPrueba[0]);

    for (int i = 0; i < numPruebas; i++) {
        int hilos = hilosPrueba[i];

        start = clock();
        int totalParalelo = ejecutarParalelo(hilos, N);
        end = clock();
        double tiempoParalelo = ((double)(end - start)) / CLOCKS_PER_SEC;

        double speedup = tiempoSecuencial / tiempoParalelo;
        double eficiencia = (speedup / hilos) * 100.0;

        printf("Paralelo (%2d hilos) -> Primos: %d | Tiempo: %.4f s | Speedup: %.2fx | Eficiencia: %.1f%%\n",
               hilos, totalParalelo, tiempoParalelo, speedup, eficiencia);
    }

    return 0;
}
