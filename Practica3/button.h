// Fichero:	button.h

#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

/* Estados del botón */
typedef enum {
    BOTON_ESPERA = 0,
    BOTON_REBOTE_PRES,
    BOTON_PULSADO,
    BOTON_REBOTE_LIB
} boton_estado_t;

/* Inicialización general */
void Eint4567_init(void);

/* ISR EINT6/EINT7 */
void Eint4567_ISR(void) __attribute__((interrupt("IRQ")));

/* Llamar cada iteración en el bucle principal */
void button_gestion_timer(void);

/* Obtiene y consume el click  */
uint8_t button_recibir_click(void);

/* Obtiene y consume un evento de incremento:
   - Pulsación corta => 1 evento
   - Pulsación larga => repetidos */
uint8_t button_recibir_incremento(void);

#endif
