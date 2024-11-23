#pragma once
#ifndef Secuencial_H
#define Secuencial_H

#include <vector>
#include <iostream>
#include <cmath>
#include <chrono>
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

// Prototipos de funciones secuenciales

// Prototipos de funciones secuenciales
bool isSafe(int** board, int size, int row, int col, int num);
bool solveSudoku(int** board, int size, int row, int col);


bool isSafe(const std::vector<std::vector<int>>& board, int row, int col, int num, int dimension);
bool resolverFila(std::vector<std::vector<int>>& board, int fila, int dimension);
void resolverSudoku(const std::vector<std::vector<int>>& initialBoard);
#endif // Secuencial_H