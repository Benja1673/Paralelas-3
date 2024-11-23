#ifndef PARALELO_H
#define PARALELO_H

#include <vector>
#include <iostream>
#include <cmath>
#include <omp.h>
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

#include <vector>

// Verifica si es seguro colocar un n�mero en una casilla espec�fica
bool isSafeLocal(int** board, int row, int col, int num, int dimension);

// Gestiona las casillas pendientes mientras otros hilos resuelven el Sudoku
//void gestionarCasillasPendientes(std::vector<CasillaPendiente>& pendientes, int** board, int dimension, bool& progreso);

// Resuelve una fila utilizando backtracking con propagaci�n de restricciones
bool resolverFilapLocal(int** localBoard, int fila, int dimension, std::vector<std::vector<std::vector<bool>>>& restricciones);

// Calcula la cantidad de casillas vac�as en cada fila
void ordenarFilasPorVacias(int** board, int dimension, std::vector<int>& filasOrdenadas);

// Calcula el n�mero de posibles soluciones por casilla en cada fila
void ordenarFilasPorSolucionesPosibles(int** board, int dimension, std::vector<int>& filasOrdenadas);

// Funci�n principal para manejar los hilos con prioridades
void manejarHilosConPrioridad(int** board, int dimension, bool* filasResueltas);

// Controla la l�gica de resoluci�n
void controlarHilos(int** board, int dimension);

// Resuelve el Sudoku utilizando hilos
void resolverSudokuConHilos(std::vector<std::vector<int>>& initialBoard);

#endif // PARALELO_H#pragma once
