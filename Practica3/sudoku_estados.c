// Fichero: sudoku_estados.c

#include "sudoku_estados.h"
#include "sudoku_adapter.h"
#include "8led.h"
#include "button.h"
#include "44blib.h"

typedef enum {
    INICIO = 0,
    SEL_FILA,
    SEL_COL,
    SEL_VAL,
    APLICAR,
    S_ERROR
} sudoku_state_t;

static sudoku_state_t estado;
static int sel_row, sel_col, sel_val;
static int prev_val;

static void show_F(void) { D8Led_symbol(D8_F); }
static void show_C(void) { D8Led_symbol(D8_C); }
static void show_E(void) { D8Led_symbol(D8_E); }

/* Inicializar recorrido del sudoku */
void sudoku_init(void)
{
	estado = INICIO;
    sel_row = sel_col = 1;
    sel_val = 0;
    prev_val = 0;
    D8Led_symbol(0); /* muestra 0 inicial */
}

/* Interaccion principal con el sudoku, llamar continuamente en el bucle principal */
void sudoku_tick(void)
{
    uint8_t click = button_recibir_click();
    uint8_t inc   = button_recibir_incremento();

    switch (estado)
    {
    	/* Estado inical. Cambia a recorrer filas al pulsar un boton. */
        case INICIO:
            if (click || inc) {
                sa_load_initial_board();
                estado = SEL_FILA;
                sel_row = 1;
                show_F();
            }
            break;

		/* Recorrido de filas. Muestra F al inicio y cambia a recorrer columnas al pulsar
           el boyón izquiedo. */
        case SEL_FILA:
            if (inc) {
                sel_row++;
                if (sel_row > 9) sel_row = 1;
                D8Led_symbol(sel_row);
            } else if (click == 1) {
            	estado = SEL_COL;
                sel_col = 1;
                show_C();
            }
            break;

        case SEL_COL:
            if (inc) {
                sel_col++;
                if (sel_col > 9) sel_col = 1;
                D8Led_symbol(sel_col);
            } else if (click == 1) {
                if (sa_is_pista(sel_row, sel_col)) {
                	estado = SEL_FILA;
                    show_F();
                } else {
                	estado = SEL_VAL;
                    sel_val = sa_get_value(sel_row, sel_col);
                    if (sel_val < 0 || sel_val > 9) sel_val = 0;
                    prev_val = sel_val;
                    D8Led_symbol(sel_val);
                }
            }
            break;

        case SEL_VAL:
            if (inc) {
                sel_val++;
                if (sel_val > 9) sel_val = 0; /* incluye 0 borrar */
                D8Led_symbol(sel_val);
            } else if (click == 1) {
            	estado = APLICAR;
            }
            break;

        case APLICAR: {
            int new_val = sel_val;
            int old_val = sa_get_value(sel_row, sel_col);

            if (new_val != 0 && !sa_is_in_candidates(sel_row, sel_col, new_val)) {
                sa_mark_cell_error(sel_row, sel_col);
                show_E();
                Delay(300);
                show_F();
                estado = SEL_FILA;
                break;
            }

            sa_set_value(sel_row, sel_col, new_val);

            if ((old_val != 0 && new_val == 0) ||
                (old_val != 0 && new_val != old_val) ||
                (new_val == 0)) {
                sa_recalc_all_candidates();
            } else if (new_val != 0) {
                sa_propagate_value(sel_row, sel_col, new_val);
            }

            show_F();
            estado = SEL_FILA;
            break;
        }

        case S_ERROR:
            if (click || inc) {
                show_F();
                estado = SEL_FILA;
            }
            break;

        default:
        	estado = INICIO;
            break;
    }
}
