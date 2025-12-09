#ifndef __TIMER1_H__
#define __TIMER1_H__

#include <stdint.h>

/* Inicializa Timer1 para generar 60 interrupciones por segundo (≈16.666 ms) */
void timer1_init_60Hz(void);

/* ISR del Timer1 */
void timer1_ISR(void);

#endif /* __TIMER1_H__ */
