/*
 * timer2.h
 *
 *  Created on: 04/11/2025
 *      Author: jorge
 */

#ifndef _TIMER2_H_
#define _TIMER2_H_

/*
  Biblioteca de medida de tiempo con Timer2 (S3C44B0X, MCLK=64MHz).

  API:
    - void timer2_init(): configura timer2 con máxima precisión y su IRQ.
    - void timer2_start(): reinicia la cuenta desde cero.
    - unsigned int timer2_count(): microsegundos desde el último start().

  Requisitos/Asunciones:
    - sys_init() ya habilitó las IRQ globales.
    - timer_init() dejó rTCFG0=255 y rTCFG1 con divisor 1/2 (para 8 μs/tick), o ajusta el cálculo.
*/
void timer2_init(void);
void timer2_start(void);
unsigned int timer2_count(void);

#endif /* _TIMER2_H_ */
