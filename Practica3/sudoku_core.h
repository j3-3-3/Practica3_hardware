// Fichero:	sudoku_core.h

#ifndef SUDOKU_CORE_H
#define SUDOKU_CORE_H

#include <stdint.h>
#include "celda.h"
#include <inttypes.h>

/* Tamaño del Sudoku */
#define NUM_FILAS     9
#define NUM_COLUMNAS  16

/* Carga tablero inicial (solo C) en dest */
void sudoku_cargar_tablero(CELDA dest[NUM_FILAS][NUM_COLUMNAS]);

/* Devuelve 1 si la celda (f,c) es pista, 0 si no */
int sudoku_es_pista(CELDA grid[NUM_FILAS][NUM_COLUMNAS], uint8_t f, uint8_t c);

/* Devuelve el valor almacenado (0..9) */
uint8_t sudoku_leer_valor(CELDA grid[NUM_FILAS][NUM_COLUMNAS], uint8_t f, uint8_t c);

/* Escribe valor (0..9); si val != 0 y la celda era pista no modifica */
void sudoku_escribir_valor(CELDA grid[NUM_FILAS][NUM_COLUMNAS], uint8_t f, uint8_t c, uint8_t val);

/* Marca error en la celda */
void sudoku_marcar_error(CELDA grid[NUM_FILAS][NUM_COLUMNAS], uint8_t f, uint8_t c);

/* Comprueba si val (1..9) es candidato de la celda (bit a 0) */
int sudoku_val_es_candidato(CELDA grid[NUM_FILAS][NUM_COLUMNAS], uint8_t f, uint8_t c, uint8_t val);

/* Propaga un valor ya fijado desde (fila,col) a vecinos (elimina candidatos) */
void candidatos_propagar_c(CELDA grid[NUM_FILAS][NUM_COLUMNAS], uint8_t fila, uint8_t columna);

/* Recalcula TODAS las listas de candidatos; retorna # de celdas vacías */
int candidatos_actualizar_c(CELDA grid[NUM_FILAS][NUM_COLUMNAS]);


extern int
candidatos_actualizar_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);

extern int
candidatos_actualizar_arm_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);

extern void
candidatos_propagar_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                               uint8_t fila, uint8_t columna);

extern int
candidatos_actualizar_all(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);


int candidatos_actualizar_c_propag_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);

int candidatos_actualizar_arm_propag_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);


#endif
