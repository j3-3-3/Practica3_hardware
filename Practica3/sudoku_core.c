// Fichero:	sudoku_core.c

#include "sudoku_core.h"
#include "celda.h"
#include "timer2.h"
#include "cola.h"

/* Tabla para inicio de regi贸n (3x3) */
static const uint8_t init_region[NUM_FILAS] = {0,0,0,3,3,3,6,6,6};

/* Tablero inicial independiente (9x9) */
static CELDA tablero_inicial[NUM_FILAS][NUM_COLUMNAS] __attribute__((aligned(32))) =
{
	0x0015, 0x0000, 0x0000, 0x0013, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0019, 0x0000, 0x0000, 0x0000, 0x0015, 0, 0, 0, 0, 0, 0, 0,
	0x0000, 0x0019, 0x0016, 0x0017, 0x0000, 0x0015, 0x0000, 0x0013, 0x0000, 0, 0, 0, 0, 0, 0, 0,
	0x0000, 0x0018, 0x0000, 0x0019, 0x0000, 0x0000, 0x0016, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
	0x0000, 0x0000, 0x0015, 0x0018, 0x0016, 0x0011, 0x0014, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
	0x0000, 0x0000, 0x0014, 0x0012, 0x0000, 0x0013, 0x0000, 0x0017, 0x0000, 0, 0, 0, 0, 0, 0, 0,
	0x0000, 0x0017, 0x0000, 0x0015, 0x0000, 0x0019, 0x0012, 0x0016, 0x0000, 0, 0, 0, 0, 0, 0, 0,
	0x0016, 0x0000, 0x0000, 0x0000, 0x0018, 0x0000, 0x0000, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0012, 0x0000, 0x0000, 0x0011, 0, 0, 0, 0, 0, 0, 0
	};

/* Prepara el espacio del tablero en memoria */
void sudoku_cargar_tablero(CELDA dest[NUM_FILAS][NUM_COLUMNAS])
{
    uint8_t f, c;
    for (f = 0; f < NUM_FILAS; ++f)
        for (c = 0; c < NUM_COLUMNAS; ++c)
            dest[f][c] = tablero_inicial[f][c];
}

/* Bit pista = 0x0010, error = 0x0020 */
int sudoku_es_pista(CELDA grid[NUM_FILAS][NUM_COLUMNAS], uint8_t f, uint8_t c)
{
    return (grid[f][c] & CELDA_FLAG_PISTA) ? 1 : 0;
}

uint8_t sudoku_leer_valor(CELDA grid[NUM_FILAS][NUM_COLUMNAS], uint8_t f, uint8_t c)
{
    return celda_leer_valor(grid[f][c]);
}

void sudoku_escribir_valor(CELDA grid[NUM_FILAS][NUM_COLUMNAS], uint8_t f, uint8_t c, uint8_t val)
{
    if (sudoku_es_pista(grid, f, c) && val != 0) return;
    celda_poner_valor(&grid[f][c], (uint8_t)(val & 0x0F));
}

void sudoku_marcar_error(CELDA grid[NUM_FILAS][NUM_COLUMNAS], uint8_t f, uint8_t c)
{
    grid[f][c] |= CELDA_FLAG_ERROR;
}

/* 0 => candidato disponible */
int sudoku_val_es_candidato(CELDA grid[NUM_FILAS][NUM_COLUMNAS], uint8_t f, uint8_t c, uint8_t val)
{
    uint16_t mask;
    if (val < 1 || val > 9) return 0;
    mask = (uint16_t)(1u << (BIT_CANDIDATOS + val - 1));
    return (grid[f][c] & mask) ? 0 : 1;
}

static inline void eliminar_en_celda(CELDA grid[NUM_FILAS][NUM_COLUMNAS],
                                     uint8_t f, uint8_t c, uint8_t valor)
{
    if (f >= NUM_FILAS || c >= 9) return;
    if (sudoku_leer_valor(grid, f, c) == 0 && valor != 0)
        celda_eliminar_candidato(&grid[f][c], valor);
}

/* *****************************************************************************
 * Propaga el valor de una determinada celda
 * para actualizar las listas de candidatos
 * de las celdas en su fila, columna y region
 */
void candidatos_propagar_c(CELDA grid[NUM_FILAS][NUM_COLUMNAS], uint8_t fila, uint8_t columna)
{
	uint8_t i, j, init_i, init_j;
	const uint8_t init_region[NUM_FILAS] = {0,0,0,3,3,3,6,6,6};

	if (fila >= NUM_FILAS || columna >= 9) return;  // 9 celdas reales por fila

	uint8_t valor = celda_leer_valor(grid[fila][columna]);
	if (valor == 0 || valor > 9) return;

	/* Recorrer fila: solo las 9 primeras columnas */
	 for (j = 0; j < 9; ++j) {
		if (j != columna) {
			 celda_eliminar_candidato(&grid[fila][j], valor);
		}
	}
	/* Recorrer columna */
	for (i = 0; i < NUM_FILAS; ++i) {
		if (i != fila) {
			celda_eliminar_candidato(&grid[i][columna], valor);
		}
	}
	/* Recorrer regi锟絥 3x3 */
	init_i = init_region[fila];
	init_j = init_region[columna];
	for (i = init_i; i < init_i + 3; ++i) {
		for (j = init_j; j < init_j + 3; ++j) {
			celda_eliminar_candidato(&grid[i][j], valor);
		}
	}
}

/* *****************************************************************************
 * Calcula todas las listas de candidatos (solo 9x9 activas)
 * retorna el numero de celdas vac锟絘s
 */
int candidatos_actualizar_c(CELDA grid[NUM_FILAS][NUM_COLUMNAS])
{
	uint32_t t_ini = timer2_count();   // tiempo inicio (ms)

	int celdas_vacias = 0;
	    uint8_t i, j, v;

	    /* Borrar bits de candidatos en toda la zona v锟絣ida (9x9) */
	    for (i = 0; i < NUM_FILAS; ++i) {
	        for (j = 0; j < 9; ++j) {  // <--- solo 9 columnas
	        	grid[i][j] &= 0x007F;
	        }
	    }

	    /* Recalcular candidatos */
	    for (i = 0; i < NUM_FILAS; ++i) {
	        for (j = 0; j < 9; ++j) {
	            v = celda_leer_valor(grid[i][j]);
	            if (v == 0) {
	                ++celdas_vacias;  //Si celda vacia no hace anda
	            } else {
	                candidatos_propagar_c(grid, i, j); //Si no esta vacia elimina el candidato
	            }
	        }
	    }

	    uint32_t t_fin = timer2_count();
	    uint32_t duracion = t_fin - t_ini;    // ms de ejecuci贸n
	    cola_depuracion(timer2_count(), EVT_TIEMPO_CALCULO, duracion);

	    return celdas_vacias;
}

/* *****************************************************************************
 * Versi贸n "C-ARM"
 */
int candidatos_actualizar_c_propag_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]) {
    int celdas_vacias = 0;
    uint8_t i, j, v;

    /* limpiar candidatos (zona 9x9) */
    for (i = 0; i < NUM_FILAS; ++i) {
        for (j = 0; j < 9; ++j) {
            cuadricula[i][j] &= 0x007F;
        }
    }

    for (i = 0; i < NUM_FILAS; ++i) {
        for (j = 0; j < 9; ++j) {
            v = celda_leer_valor(cuadricula[i][j]);
            if (v == 0) {
                ++celdas_vacias;
            }
            else {
                candidatos_propagar_arm(cuadricula, i, j);
            }
        }
    }

    return celdas_vacias;
}

/* *****************************************************************************
 * Versi贸n "ARM-C"
 */
int candidatos_actualizar_arm_propag_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]) {
	uint32_t t_ini = timer2_count();   // tiempo inicio (ms)
    int celdas_vacias = 0;
    uint8_t i, j, v;

    /* limpiar candidatos (zona 9x9) */
    for (i = 0; i < NUM_FILAS; ++i) {
        for (j = 0; j < 9; ++j) {
            cuadricula[i][j] &= 0x007F;
        }
    }

    /* recalcular: cuando haya valor != 0 llamamos a la versi贸n C de propagar */
    for (i = 0; i < NUM_FILAS; ++i) {
        for (j = 0; j < 9; ++j) {
            v = celda_leer_valor(cuadricula[i][j]);
            if (v == 0) {
                ++celdas_vacias;
            } else {
                candidatos_propagar_c(cuadricula, i, j);
            }
        }
    }
    uint32_t t_fin = timer2_count();
	uint32_t duracion = t_fin - t_ini;    // ms de ejecuci贸n
	cola_depuracion(timer2_count(), EVT_TIEMPO_CALCULO, duracion);

    return celdas_vacias;
}
int candidatos_actualizar_all(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]){
	uint32_t t_ini = timer2_count();
	candidatos_actualizar_all(cuadricula, i, j);
	uint32_t t_fin = timer2_count();
		    uint32_t duracion = t_fin - t_ini;    // ms de ejecuci髇
		    cola_depuracion(timer2_count(), EVT_TIEMPO_CALCULO, duracion);

}



