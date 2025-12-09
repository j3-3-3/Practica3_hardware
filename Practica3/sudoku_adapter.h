// Fichero:	sudoku_adapter.c

#ifndef __SUDOKU_ADAPTER_H__
#define __SUDOKU_ADAPTER_H__

#include <stdint.h>

/* Carga el tablero inicial (independiente de P1) */
void sa_load_initial_board(void);

/* Recalcula TODOS los candidatos (uso inicial y casos especiales) */
void sa_recalc_all_candidates(void);

/* Propaga el valor nuevo a sus vecinos (fila, columna, región) */
void sa_propagate_value(int row1_9, int col1_9, int val0_9);

/* Lee si la celda es una pista (no modificable). Retorna 1/0 */
int sa_is_pista(int row1_9, int col1_9);

/* Lee el valor actual (0..9) de la celda */
int sa_get_value(int row1_9, int col1_9);

/* Escribe el valor (0..9) de la celda */
void sa_set_value(int row1_9, int col1_9, int val0_9);

/* Comprueba si val está en la lista de candidatos (1 sí, 0 no) */
int sa_is_in_candidates(int row1_9, int col1_9, int val1_9);

/* Marca “error” en la celda (activar bit correspondiente) */
void sa_mark_cell_error(int row1_9, int col1_9);

#endif
