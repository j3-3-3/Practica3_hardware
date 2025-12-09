#ifndef __EVENTOS_H__
#define __EVENTOS_H__

#include <stdint.h>

/* Identificadores únicos de eventos */
typedef enum {
    EVENTO_TIMER2 = 1,
    EVENTO_PULSADOR = 2,
    EVENTO_LED = 3,
    EVENTO_GENERAL = 4,
    EVT_BOTON_PRES = 5,
    EVT_BOTON_ESTABLE = 6,
    EVT_BOTON_LIBERA = 7,
    EVT_BOTON_LISTO = 8,
    EVENTO_TIMER3 = 9,
    //EVENTO_TIMER1 = 10,
    EVT_TIEMPO_CALCULO = 11
} ID_EVENTO_t;

#endif /* __EVENTOS_H__ */
