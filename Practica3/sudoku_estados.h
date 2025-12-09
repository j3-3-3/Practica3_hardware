//fichero: sudoku_estados.h


#ifndef SUDOKU_FSM_H_
#define SUDOKU_FSM_H_

/* Inicializa estado de juego y display */
void sudoku_init(void);

/* Interaccion principal con el sudoku, llamar continuamente en el bucle principal */
void sudoku_tick(void);

#endif /* SUDOKU_FSM_H_ */
