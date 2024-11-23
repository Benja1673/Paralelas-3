#include <iostream>
#include <vector>
#include <cmath>  // Para sqrt, necesario para subcuadros de mayor tama�o
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

// Funci�n para verificar si es seguro colocar un n�mero en una celda en Sudoku de cualquier tama�o
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
	// Verificar la subcuadr�cula
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

// Algoritmo de backtracking con memoria din�mica y poda
bool solveSudoku(int** board, int size, int row, int col) {
	// Si hemos llegado al final del tablero
	if (row == size) return true;
	// Si la columna se sale de los l�mites, pasa a la siguiente fila
	if (col == size) return solveSudoku(board, size, row + 1, 0);
	// Si la celda ya tiene un valor, pasa a la siguiente
	if (board[row][col] != 0) return solveSudoku(board, size, row, col + 1);

	// Poda: verificar n�meros v�lidos en la posici�n actual
	for (int num = 1; num <= size; num++) {
		if (isSafe(board, size, row, col, num)) {
			board[row][col] = num; // Colocar el n�mero provisionalmente
			if (solveSudoku(board, size, row, col + 1)) return true; // Avanza
			board[row][col] = 0; // Backtrack: quitar el n�mero
		}
	}
	return false; // Si no hay ninguna opci�n v�lida, se devuelve falso
}

// Funci�n auxiliar para verificar si es seguro colocar un n�mero en una celda espec�fica
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

// Funci�n para resolver una fila del Sudoku utilizando backtracking
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

// Funci�n principal para resolver un Sudoku de cualquier tama�o
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