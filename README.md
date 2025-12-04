Requisitos del Sistema
Hardware
CPU multinúcleo (recomendado: 8 núcleos o más)
4 GB RAM mínimo (ideal 8 GB)
Software
Linux (recomendado: Ubuntu, Debian, Kali o similar)
Compiladores e intérpretes:
gcc para C
Go 1.20+
Rust (cargo 1.70+)
Python 3.10+
Librerías Python:
pip install numpy matplotlib

Instrucciones de Instalación por Lenguaje
C
Instalar compilador
sudo apt install build-essential
Compilar programas
cd C
make
Ejecutar QuickSort
./quicksort 1000000
Ejecutar Primo
./primo 1000000

Go
Instalar Go
sudo apt install golang
Ejecutar QuickSort
cd Go
go run QuickSortGo.go 1000000
Ejecutar Primes
go run PrimoGo.go 1000000

Rust
Instalar Rust
curl https://sh.rustup.rs -sSf | sh
source $HOME/.cargo/env
Compilar
cd Rust
cargo build --release
Ejecutar
./target/release/QuickSortRust 1000000
./target/release/PrimoRust 1000000

Python
Instalar dependencias
sudo apt install python3 python3-pip
pip install numpy
Ejecutar QuickSort
python3 QuickSortPy.py 1000000
Ejecutar Primo
python3 PrimoPy.py 1000000

Cómo Reproducir los Experimentos
Para reproducir los experimentos del informe:
Seleccione el lenguaje que desea probar.
Ejecute el programa correspondiente indicando el tamaño del problema.
Cada programa:
ejecuta pruebas secuenciales
ejecuta pruebas concurrentes con distintos niveles de hilos/procesos
muestra tablas con tiempo y speedup

Ejemplo con Rust:
./target/release/QuickSortRust 1000000
Ejemplo con Go:
go run PrimoGo.go 500000
Los resultados se pueden exportar manualmente o redirigir a archivo:
go run QuickSortGo.go 1000000 > salida.txt

Notas finales
Todas las implementaciones siguen la misma estructura lógica para permitir comparaciones justas.
La variación en los resultados se debe al modelo de concurrencia propio de cada lenguaje.

Autores:
Jhon Alexander Argaez
Esteban Correa Roldan
