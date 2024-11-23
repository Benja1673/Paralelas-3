

#pragma once
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

#include <iostream>

#ifndef SUDOKUS_H
#define SUDOKUS_H

#include <vector>

// Declaración de los tableros como variables externas
extern std::vector<std::vector<int>> board9x9_dificultad_media;
extern std::vector<std::vector<int>> board16x16_dificultad_media;
extern std::vector<std::vector<int>> board25x25_dificultad_media;

int** initializeBoard(const std::vector<std::vector<int>>& initialBoard);
void freeBoard(int** board, int size);
void printBoard(int** board, int size);

#endif // SUDOKUS_H