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
#include "sudokus.h"
#include "Secuencial.h"
#include "paralelo.h"

// Función para verificar si es seguro colocar un número en una celda en Sudoku de cualquier tamaño
bool isSafe(int** board, int size, int row, int col, int num) {
	// Verificar la fila
	for (int x = 0; x < size; x++) {
		if (board[row][x] == num) {
			return false;
		}
	}
	// Verificar la columna
	for (int x = 0; x < size; x++) {
		if (board[x][col] == num) {
			return false;
		}
	}
	// Verificar la subcuadrícula
	int subSize = static_cast<int>(std::sqrt(size));
	int startRow = row - row % subSize;
	int startCol = col - col % subSize;
	for (int i = 0; i < subSize; i++) {
		for (int j = 0; j < subSize; j++) {
			if (board[startRow + i][startCol + j] == num) {
				return false;
			}
		}
	}
	return true;
}

// Algoritmo de backtracking con memoria dinámica y poda
bool solveSudoku(int** board, int size, int row, int col) {
	// Si hemos llegado al final del tablero
	if (row == size) return true;
	// Si la columna se sale de los límites, pasa a la siguiente fila
	if (col == size) return solveSudoku(board, size, row + 1, 0);
	// Si la celda ya tiene un valor, pasa a la siguiente
	if (board[row][col] != 0) return solveSudoku(board, size, row, col + 1);

	// Poda: verificar números válidos en la posición actual
	for (int num = 1; num <= size; num++) {
		if (isSafe(board, size, row, col, num)) {
			board[row][col] = num; // Colocar el número provisionalmente
			if (solveSudoku(board, size, row, col + 1)) return true; // Avanza
			board[row][col] = 0; // Backtrack: quitar el número
		}
	}
	return false; // Si no hay ninguna opción válida, se devuelve falso
}

// Función auxiliar para verificar si es seguro colocar un número en una celda específica
bool isSafe(const std::vector<std::vector<int>>& board, int row, int col, int num, int dimension) {
	for (int x = 0; x < dimension; x++) {
		if (board[row][x] == num || board[x][col] == num) return false;
	}
	int subSize = std::sqrt(dimension);
	int startRow = row - row % subSize, startCol = col - col % subSize;
	for (int i = 0; i < subSize; i++) {
		for (int j = 0; j < subSize; j++) {
			if (board[startRow + i][startCol + j] == num) return false;
		}
	}
	return true;
}

// Función para resolver una fila del Sudoku utilizando backtracking
bool resolverFila(std::vector<std::vector<int>>& board, int fila, int dimension) {
	for (int col = 0; col < dimension; col++) {
		if (board[fila][col] == 0) {
			for (int num = 1; num <= dimension; num++) {
				if (isSafe(board, fila, col, num, dimension)) {
					board[fila][col] = num;
					if (resolverFila(board, fila, dimension)) return true;
					board[fila][col] = 0;
				}
			}
			return false;
		}
	}
	return true;
}

// Función principal para resolver un Sudoku de cualquier tamaño
void resolverSudoku(const std::vector<std::vector<int>>& initialBoard) {
	int size = initialBoard.size();
	int** board = initializeBoard(initialBoard);

	// Imprimir el Sudoku antes de resolverlo
	std::cout << "Sudoku a resolver:" << std::endl;
	printBoard(board, size);

	if (solveSudoku(board, size, 0, 0)) {
	}
	else {
		std::cout << "No se pudo resolver el Sudoku." << std::endl;
	}

	freeBoard(board, size);
}