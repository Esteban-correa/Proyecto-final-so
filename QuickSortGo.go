package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
	"os"
	"strconv"
)

// ---------- QuickSort Secuencial ----------
func quicksortSequential(arr []int) {
	if len(arr) < 2 {
		return
	}
	pivot := arr[len(arr)/2]
	left := 0
	right := len(arr) - 1

	for left <= right {
		for arr[left] < pivot {
			left++
		}
		for arr[right] > pivot {
			right--
		}
		if left <= right {
			arr[left], arr[right] = arr[right], arr[left]
			left++
			right--
		}
	}
	if right > 0 {
		quicksortSequential(arr[:right+1])
	}
	quicksortSequential(arr[left:])
}

// ---------- QuickSort Paralelo ----------
func quicksortParallel(arr []int, depth int, wg *sync.WaitGroup) {
	defer wg.Done()

	if len(arr) < 2 {
		return
	}
	pivot := arr[len(arr)/2]
	left := 0
	right := len(arr) - 1

	for left <= right {
		for arr[left] < pivot {
			left++
		}
		for arr[right] > pivot {
			right--
		}
		if left <= right {
			arr[left], arr[right] = arr[right], arr[left]
			left++
			right--
		}
	}

	if depth > 0 {
		var subwg sync.WaitGroup
		subwg.Add(2)
		go func() {
			quicksortParallel(arr[:right+1], depth-1, &subwg)
		}()
		go func() {
			quicksortParallel(arr[left:], depth-1, &subwg)
		}()
		subwg.Wait()
	} else {
		if right > 0 {
			quicksortSequential(arr[:right+1])
		}
		quicksortSequential(arr[left:])
	}
}

func main() {
	if len(os.Args) != 3 {
		fmt.Println("Uso: go run quicksort_comparacion.go <tamaño_del_arreglo> <profundidad_de_paralelismo>")
		return
	}

	n, _ := strconv.Atoi(os.Args[1])
	depth, _ := strconv.Atoi(os.Args[2])

	arrSeq := make([]int, n)
	arrPar := make([]int, n)

	// Inicialización de datos
	rand.Seed(42)
	for i := range arrSeq {
		val := rand.Intn(100000)
		arrSeq[i] = val
		arrPar[i] = val
	}

	// ---- Ejecución secuencial ----
	startSeq := time.Now()
	quicksortSequential(arrSeq)
	elapsedSeq := time.Since(startSeq).Seconds()

	// ---- Ejecución paralela ----
	startPar := time.Now()
	var wg sync.WaitGroup
	wg.Add(1)
	go quicksortParallel(arrPar, depth, &wg)
	wg.Wait()
	elapsedPar := time.Since(startPar).Seconds()

	// ---- Calcular Speedup ----
	speedup := elapsedSeq / elapsedPar

	fmt.Println("\n===== RESULTADOS =====")
	fmt.Printf("Tamaño del arreglo: %d\n", n)
	fmt.Printf("Profundidad de paralelismo: %d\n", depth)
	fmt.Printf("Tiempo secuencial: %.6f s\n", elapsedSeq)
	fmt.Printf("Tiempo paralelo: %.6f s\n", elapsedPar)
	fmt.Printf("Speedup: %.2fx\n", speedup)
}
