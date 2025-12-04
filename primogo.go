package main

import (
	"fmt"
	"math"
	"runtime"
	"sync"
	"time"
)

const N = 1000000 // límite hasta donde calcular los primos

// ---------- Función para verificar si un número es primo ----------
func esPrimo(num int) bool {
	if num < 2 {
		return false
	}
	if num == 2 {
		return true
	}
	if num%2 == 0 {
		return false
	}
	raiz := int(math.Sqrt(float64(num)))
	for i := 3; i <= raiz; i += 2 {
		if num%i == 0 {
			return false
		}
	}
	return true
}

// ---------- Versión secuencial ----------
func contarPrimosSecuencial(n int) int {
	count := 0
	for i := 2; i <= n; i++ {
		if esPrimo(i) {
			count++
		}
	}
	return count
}

// ---------- Versión paralela ----------
func contarPrimosParalelo(n int, numGoroutines int) int {
	var wg sync.WaitGroup
	resultados := make([]int, numGoroutines)
	rango := n / numGoroutines

	for i := 0; i < numGoroutines; i++ {
		wg.Add(1)
		inicio := i*rango + 1
		fin := (i + 1) * rango
		if i == numGoroutines-1 {
			fin = n
		}

		go func(idx, start, end int) {
			defer wg.Done()
			localCount := 0
			for j := start; j <= end; j++ {
				if esPrimo(j) {
					localCount++
				}
			}
			resultados[idx] = localCount
		}(i, inicio, fin)
	}

	wg.Wait()

	total := 0
	for _, val := range resultados {
		total += val
	}
	return total
}

func main() {
	fmt.Printf("Calculando primos hasta %d...\n", N)

	// ---------- Secuencial ----------
	start := time.Now()
	totalSecuencial := contarPrimosSecuencial(N)
	tiempoSecuencial := time.Since(start).Seconds()
	fmt.Printf("Secuencial -> Primos: %d | Tiempo: %.4f s\n", totalSecuencial, tiempoSecuencial)

	// ---------- Paralelo ----------
	numCPU := runtime.NumCPU()
	runtime.GOMAXPROCS(numCPU)

	start = time.Now()
	totalParalelo := contarPrimosParalelo(N, numCPU)
	tiempoParalelo := time.Since(start).Seconds()
	fmt.Printf("Paralelo (%d hilos) -> Primos: %d | Tiempo: %.4f s\n", numCPU, totalParalelo, tiempoParalelo)

	// ---------- Speedup ----------
	speedup := tiempoSecuencial / tiempoParalelo
	fmt.Printf("\nSpeedup: %.2fx\n", speedup)
}
