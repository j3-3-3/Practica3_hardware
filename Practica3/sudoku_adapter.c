// Fichero:	sudoku_adapter.c

#include "sudoku_adapter.h"
#include "sudoku_core.h"

/* Buffer de ejecución único (independiente de P1) */
static CELDA runtime_grid[NUM_FILAS][NUM_COLUMNAS] __attribute__((aligned(32)));

void sa_load_initial_board(void)
{
    sudoku_cargar_tablero(runtime_grid);
    candidatos_actualizar_arm_propag_c(runtime_grid); /* inicializa listas */
}

void sa_recalc_all_candidates(void)
{
    (void)candidatos_actualizar_c(runtime_grid);
}

void sa_propagate_value(int row1_9, int col1_9, int val0_9)
{
    /* Escribe y propaga desde la celda seleccionada */
    sudoku_escribir_valor(runtime_grid, (uint8_t)(row1_9-1), (uint8_t)(col1_9-1), (uint8_t)val0_9);
    candidatos_propagar_c(runtime_grid, (uint8_t)(row1_9-1), (uint8_t)(col1_9-1));
}

int sa_is_pista(int row1_9, int col1_9)
{
    return sudoku_es_pista(runtime_grid, (uint8_t)(row1_9-1), (uint8_t)(col1_9-1));
}

int sa_get_value(int row1_9, int col1_9)
{
    return (int)sudoku_leer_valor(runtime_grid, (uint8_t)(row1_9-1), (uint8_t)(col1_9-1));
}

void sa_set_value(int row1_9, int col1_9, int val0_9)
{
    sudoku_escribir_valor(runtime_grid, (uint8_t)(row1_9-1), (uint8_t)(col1_9-1), (uint8_t)val0_9);
}

int sa_is_in_candidates(int row1_9, int col1_9, int val1_9)
{
    return sudoku_val_es_candidato(runtime_grid,
                                   (uint8_t)(row1_9-1),
                                   (uint8_t)(col1_9-1),
                                   (uint8_t)val1_9);
}

void sa_mark_cell_error(int row1_9, int col1_9)
{
    sudoku_marcar_error(runtime_grid, (uint8_t)(row1_9-1), (uint8_t)(col1_9-1));
}
