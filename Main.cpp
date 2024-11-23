#include <iostream>
#include <vector>
#include <cmath>  // Para sqrt, necesario para subcuadros de mayor tamaño
#include <chrono> // Para medir el tiempo
#include <thread>
#include <mutex>
#include <queue>
#include <future>
#include <omp.h>
#include <random>
#include <iomanip> // Para formatear la salida
#include "Secuencial.h"
#include "paralelo.h"
#include "sudokus.h"

using namespace std;
// Mutex para gestionar acceso a variables compartidas

void menuPrincipal();

// Cambiado a std::mutex para evitar ambigüedad

const int NUM_HILOS = 8;

// Llamada a la función `resolverSudoku` pasando el objeto `board25x25_dificultad_media`
void resolver25x25() {
	resolverSudoku(board25x25_dificultad_media);
}
void resolver25x25p() {
	resolverSudokuConHilos(board25x25_dificultad_media);
}

// Llamada a la función `resolverSudoku` pasando el objeto `board16x16_dificultad_media`
void resolver16x16() {
	resolverSudoku(board16x16_dificultad_media);
}
void resolver16x16p() {
	resolverSudokuConHilos(board16x16_dificultad_media);
}

// Llamada a la función `resolverSudoku` pasando el objeto `board9x9_dificultad_media`
void resolver9x9() {
	resolverSudoku(board9x9_dificultad_media);
}
void resolver9x9p() {
	resolverSudokuConHilos(board9x9_dificultad_media);
}

// Menú principal
void menuPrincipal() {
	int opcionPrincipal;
	int opcionSudoku;

	while (true) {
		std::cout << "=== MENU PRINCIPAL ===" << std::endl;
		std::cout << "1. Solucionar Sudoku sin paralelizar" << std::endl;
		std::cout << "2. Solucionar Sudoku con técnicas de paralelización (por filas)" << std::endl;
		std::cout << "3. Verificar cantidad de hilos disponibles" << std::endl;
		std::cout << "4. Salir" << std::endl;
		std::cout << "Elija una opción: ";
		std::cin >> opcionPrincipal;

		if (opcionPrincipal == 4) {
			std::cout << "Saliendo del programa..." << std::endl;
			break;
		}

		switch (opcionPrincipal) {
		case 1: {
			std::cout << "\n=== Elija el tamaño del Sudoku ===" << std::endl;
			std::cout << "1. Sudoku 9x9" << std::endl;
			std::cout << "2. Sudoku 16x16" << std::endl;
			std::cout << "3. Sudoku 25x25" << std::endl;
			std::cout << "Elija una opción: ";
			std::cin >> opcionSudoku;

			auto start = std::chrono::high_resolution_clock::now();
			switch (opcionSudoku) {
			case 1:
				resolver9x9();
				break;
			case 2:
				resolver16x16();
				break;
			case 3:
				resolver25x25();
				break;
				// Añadir más casos para 16x16 y 25x25 según sea necesario.
			default:
				std::cout << "Opción no válida." << std::endl;
				continue;
			}
			auto end = std::chrono::high_resolution_clock::now();
			auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			std::cout << "Tiempo para resolver el Sudoku: " << duration_ms << " ms" << std::endl;

			break;
		}

		case 2: {  // Resolver Sudoku con paralelización por filas aleatorias
			std::cout << "\n=== Elija el tamaño del Sudoku ===" << std::endl;
			std::cout << "1. Sudoku 9x9" << std::endl;
			std::cout << "2. Sudoku 16x16" << std::endl;
			std::cout << "3. Sudoku 25x25" << std::endl;
			std::cout << "Elija una opción: ";
			std::cin >> opcionSudoku;

			auto start = std::chrono::high_resolution_clock::now();
			switch (opcionSudoku) {
			case 1:
				resolver9x9p();

				break;
			case 2:
				resolver16x16p();

				break;
			case 3:
				resolver25x25p();

				break;
			default:
				std::cout << "Opción no válida." << std::endl;
				continue;
			}
			auto end = std::chrono::high_resolution_clock::now();
			auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			std::cout << "Tiempo para resolver el Sudoku: " << duration_ms << " ms" << std::endl;
			break;
		}

		case 3: {
			unsigned int numHilos = std::thread::hardware_concurrency();
			if (numHilos == 0) {
				std::cout << "No se pudo determinar el número de hilos disponibles." << std::endl;
			}
			else {
				std::cout << "Número de hilos disponibles: " << numHilos << std::endl;
			}
			break;
		}

		default:
			std::cout << "Opción no válida." << std::endl;
			break;
		}

		std::cout << std::endl;
	}
}

int main() {
	menuPrincipal();
	return 0;
}