#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <unistd.h>

// ---------- Función para medir tiempo ----------
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

// ---------- QuickSort Secuencial ----------
void quicksort_secuencial(int *arr, int low, int high) {
    if (low < high) {
        int pivot = arr[high];
        int i = (low - 1);
        for (int j = low; j <= high - 1; j++) {
            if (arr[j] < pivot) {
                i++;
                int tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
        int tmp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = tmp;
        int pi = i + 1;

        quicksort_secuencial(arr, low, pi - 1);
        quicksort_secuencial(arr, pi + 1, high);
    }
}

// ---------- QuickSort Paralelo ----------
void quicksort_paralelo(int *arr, int low, int high, int profundidad) {
    if (low < high) {
        int pivot = arr[high];
        int i = (low - 1);
        for (int j = low; j <= high - 1; j++) {
            if (arr[j] < pivot) {
                i++;
                int tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
        int tmp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = tmp;
        int pi = i + 1;

        if (profundidad > 0) {
            pid_t pid = fork();
            if (pid == 0) {
                quicksort_paralelo(arr, low, pi - 1, profundidad - 1);
                exit(0);
            } else {
                quicksort_paralelo(arr, pi + 1, high, profundidad - 1);
                waitpid(pid, NULL, 0);
            }
        } else {
            quicksort_secuencial(arr, low, pi - 1);
            quicksort_secuencial(arr, pi + 1, high);
        }
    }
}

// ---------- Función principal ----------
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <tamaño_del_arreglo> <profundidad_de_paralelismo>\n", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);
    int depth = atoi(argv[2]);

    int shmid = shmget(IPC_PRIVATE, n * sizeof(int), IPC_CREAT | 0666);
    int *arr_par = (int *)shmat(shmid, NULL, 0);

    int *arr_seq = malloc(n * sizeof(int));

    // Inicialización de datos aleatorios
    srand(42);
    for (int i = 0; i < n; i++) {
        int val = rand() % 100000;
        arr_seq[i] = val;
        arr_par[i] = val;
    }

    // ---- Ejecución secuencial ----
    double start_seq = get_time();
    quicksort_secuencial(arr_seq, 0, n - 1);
    double end_seq = get_time();
    double tiempo_seq = end_seq - start_seq;

    // ---- Ejecución paralela ----
    double start_par = get_time();
    quicksort_paralelo(arr_par, 0, n - 1, depth);
    double end_par = get_time();
    double tiempo_par = end_par - start_par;

    // ---- Calcular Speedup ----
    double speedup = tiempo_seq / tiempo_par;

    printf("\n===== RESULTADOS =====\n");
    printf("Tamaño del arreglo: %d\n", n);
    printf("Profundidad de paralelismo: %d\n", depth);
    printf("Tiempo secuencial: %.6f s\n", tiempo_seq);
    printf("Tiempo paralelo: %.6f s\n", tiempo_par);
    printf("Speedup: %.2fx\n", speedup);

    // Liberar memoria compartida
    shmdt(arr_par);
    shmctl(shmid, IPC_RMID, NULL);
    free(arr_seq);
    return 0;
}
