#ifndef __TIMER3_H__
#define __TIMER3_H__

#include <stdint.h>

/* Inicializa Timer3 con interrupciones deshabilitadas inicialmente */
void timer3_init(void);

/* Configura un retardo en microsegundos y arranca el timer */
void timer3_start(uint32_t usec);

/* Detiene Timer3 */
void timer3_stop(void);

/* Rutina de servicio de interrupción del Timer3 */
void timer3_ISR(void);

#endif /* __TIMER3_H__ */
