#include <iostream>
#include <vector>
#include <cmath>  // Para sqrt, necesario para subcuadros de mayor tamaño
#include <chrono> // Para medir el tiempo de ejecución
#include <thread> // Para gestionar hilos en paralelo
#include <mutex>  // Para la sincronización de hilos
#include <queue>  // Para el uso de colas
#include <future> // Para manejar tareas asincrónicas
#include <omp.h>  // Para las directivas de paralelización con OpenMP
#include <random> // Para generar números aleatorios
#include <iomanip> // Para formatear la salida
#include "sudokus.h"   // Archivo con las representaciones de tableros de Sudoku
#include "paralelo.h"  // Archivo para la resolución paralela del Sudoku

const int NUM_HILOS = 8;

// Estructura para almacenar casillas con múltiples opciones
struct CasillaPendiente {
	int fila;
	int columna;
	std::vector<int> opciones;  // Opciones posibles para la casilla
};

// Función para verificar si es seguro colocar un número en una casilla específica
bool isSafeLocal(int** board, int row, int col, int num, int dimension) {
	for (int x = 0; x < dimension; x++) {
		if (board[row][x] == num || board[x][col] == num) return false;
	}
	int subSize = std::sqrt(dimension);
	int startRow = row - row % subSize;
	int startCol = col - col % subSize;
	for (int i = 0; i < subSize; i++) {
		for (int j = 0; j < subSize; j++) {
			if (board[startRow + i][startCol + j] == num) return false;
		}
	}
	return true;
}

// Esta función gestiona las casillas pendientes mientras otros hilos resuelven el Sudoku
void gestionarCasillasPendientes(std::vector<CasillaPendiente>& pendientes, int** board, int dimension, bool& progreso) {
	while (true) {
		bool cambios = false;
		for (auto& casilla : pendientes) {
			std::vector<int> nuevasOpciones;
			for (int num : casilla.opciones) {
				if (isSafeLocal(board, casilla.fila, casilla.columna, num, dimension)) {
					nuevasOpciones.push_back(num);
				}
			}

			if (nuevasOpciones.size() == 1) {
				// Actualiza la casilla con la única opción disponible
				board[casilla.fila][casilla.columna] = nuevasOpciones[0];
				progreso = true;
				cambios = true; // Hubo un cambio en el tablero
			}
			else {
				casilla.opciones = nuevasOpciones;
			}
		}

		// Si no hay cambios, terminamos la función
		if (!cambios) break;
		std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Pequeña espera para no consumir demasiados recursos
	}
}

// Resolver una fila utilizando backtracking con propagación de restricciones
bool resolverFilapLocal(int** localBoard, int fila, int dimension, std::vector<std::vector<std::vector<bool>>>& restricciones) {
	for (int col = 0; col < dimension; col++) {
		if (localBoard[fila][col] == 0) {
			for (int num = 1; num <= dimension; num++) {
				if (isSafeLocal(localBoard, fila, col, num, dimension) && !restricciones[fila][col][num - 1]) {
					localBoard[fila][col] = num;

					// Actualizar restricciones
					restricciones[fila][col][num - 1] = true;

					if (resolverFilapLocal(localBoard, fila, dimension, restricciones)) return true;

					// Revertir restricciones y retroceder
					restricciones[fila][col][num - 1] = false;
					localBoard[fila][col] = 0;
				}
			}
			return false;
		}
	}
	return true;
}

// Función para calcular la cantidad de casillas vacías en cada fila
void ordenarFilasPorVacias(int** board, int dimension, std::vector<int>& filasOrdenadas) {
	std::vector<std::pair<int, int>> filaConVacías;

	for (int i = 0; i < dimension; i++) {
		int vacias = 0;
		for (int j = 0; j < dimension; j++) {
			if (board[i][j] == 0) vacias++;
		}
		filaConVacías.push_back({ vacias, i });
	}

	// Ordenar las filas por la cantidad de casillas vacías (de menor a mayor)
	std::sort(filaConVacías.begin(), filaConVacías.end());
	for (auto& item : filaConVacías) {
		filasOrdenadas.push_back(item.second);  // Solo nos interesa el índice de la fila
	}
}

// Función para calcular el número de posibles soluciones por casilla en cada fila
void ordenarFilasPorSolucionesPosibles(int** board, int dimension, std::vector<int>& filasOrdenadas) {
	std::vector<std::pair<int, int>> filaConSoluciones;

	for (int i = 0; i < dimension; i++) {
		int posibles = 0;
		for (int j = 0; j < dimension; j++) {
			if (board[i][j] == 0) {
				int numPosibles = 0;
				for (int num = 1; num <= dimension; num++) {
					if (isSafeLocal(board, i, j, num, dimension)) {
						numPosibles++;
					}
				}
				posibles += numPosibles;  // Acumula las posibles soluciones por casilla
			}
		}
		filaConSoluciones.push_back({ posibles, i });
	}

	// Ordenar las filas por la cantidad de soluciones posibles por casilla (de menor a mayor)
	std::sort(filaConSoluciones.begin(), filaConSoluciones.end());
	for (auto& item : filaConSoluciones) {
		filasOrdenadas.push_back(item.second);  // Solo nos interesa el índice de la fila
	}
}

// Función principal para manejar los hilos con dos prioridades
void manejarHilosConPrioridad(int** board, int dimension, bool* filasResueltas) {
	// Inicializar restricciones
	std::vector<std::vector<std::vector<bool>>> restricciones(
		dimension, std::vector<std::vector<bool>>(dimension, std::vector<bool>(dimension, false)));

	// Lista de casillas pendientes (con múltiples opciones)
	std::vector<CasillaPendiente> pendientes;

	// Copias locales por hilo
	int*** localBoards = new int** [NUM_HILOS];
	for (int i = 0; i < NUM_HILOS; i++) {
		localBoards[i] = new int* [dimension];
		for (int j = 0; j < dimension; j++) {
			localBoards[i][j] = new int[dimension];
			std::copy(board[j], board[j] + dimension, localBoards[i][j]);
		}
	}

	// Crear una variable de progreso
	bool progreso = false;

	// Crear el hilo para gestionar las casillas pendientes
	std::thread hiloPendientes(gestionarCasillasPendientes, std::ref(pendientes), board, dimension, std::ref(progreso));

	// Primero, ordenar las filas por la cantidad de casillas vacías
	std::vector<int> filasOrdenadas;
	ordenarFilasPorVacias(board, dimension, filasOrdenadas);

	// Luego, ordenar las filas por el número de soluciones posibles por casilla
	std::vector<int> filasOrdenadasPorSoluciones;
	ordenarFilasPorSolucionesPosibles(board, dimension, filasOrdenadasPorSoluciones);

	// Resolver casillas con hilos, primero usando la prioridad de filas con menos casillas vacías
#pragma omp parallel num_threads(NUM_HILOS)
	{
		int threadID = omp_get_thread_num();
		while (true) {
			bool filaProcesada = false;
			for (int i = 0; i < filasOrdenadas.size(); i++) {
				int fila = filasOrdenadas[i];
				if (!filasResueltas[fila]) {
					if (resolverFilapLocal(localBoards[threadID], fila, dimension, restricciones)) {
						filasResueltas[fila] = true;
						filaProcesada = true;
					}
				}
			}
			// Después, resolver las filas ordenadas por la cantidad de soluciones posibles
			if (!filaProcesada) {
				for (int i = 0; i < filasOrdenadasPorSoluciones.size(); i++) {
					int fila = filasOrdenadasPorSoluciones[i];
					if (!filasResueltas[fila]) {
						if (resolverFilapLocal(localBoards[threadID], fila, dimension, restricciones)) {
							filasResueltas[fila] = true;
							filaProcesada = true;
						}
					}
				}
			}
			if (!filaProcesada) break; // Si ya no hay más filas pendientes, se detiene
		}
	}

	// Esperar que el hilo de casillas pendientes termine su trabajo
	hiloPendientes.join();

	// Actualizar el tablero con los resultados de los tableros locales
#pragma omp parallel for schedule(dynamic)
	for (int fila = 0; fila < dimension; fila++) {
		if (filasResueltas[fila]) {
			for (int i = 0; i < NUM_HILOS; i++) {
				for (int col = 0; col < dimension; col++) {
					if (localBoards[i][fila][col] != 0) {
						board[fila][col] = localBoards[i][fila][col];
					}
				}
			}
		}
	}

	// Liberar la memoria de las copias locales
	for (int i = 0; i < NUM_HILOS; i++) {
		for (int j = 0; j < dimension; j++) {
			delete[] localBoards[i][j];
		}
		delete[] localBoards[i];
	}
	delete[] localBoards;
}

// Controlar la lógica de resolución
void controlarHilos(int** board, int dimension) {
	bool* filasResueltas = new bool[dimension]();

	bool progreso = true;
	while (progreso) {
		progreso = false;

		manejarHilosConPrioridad(board, dimension, filasResueltas);

		int filasPendientes = 0;
		for (int i = 0; i < dimension; i++) {
			if (!filasResueltas[i]) filasPendientes++;
		}

		if (filasPendientes == 0) {
			std::cout << "Todas las filas han sido resueltas." << std::endl;
			printBoard(board, dimension);
			progreso = false;
		}
		else {
			std::cout << "Filas pendientes de resolver: " << filasPendientes << std::endl;
			progreso = true;
		}
	}

	delete[] filasResueltas;
}

// Resolver el Sudoku con hilos
void resolverSudokuConHilos(std::vector<std::vector<int>>& initialBoard) {
	int dimension = initialBoard.size();
	int** board = initializeBoard(initialBoard);



	controlarHilos(board, dimension);


	freeBoard(board, dimension);
}