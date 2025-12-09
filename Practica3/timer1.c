#include "44b.h"
#include "44blib.h"
#include "timer1.h"
#include "led.h"

/* Activa depuración con cola y cronómetro */
#if 1
  #include "cola.h"
  #include "eventos.h"
  #include "timer2.h"
  #define HB_DEBUG 1
#else
  #define HB_DEBUG 0
#endif

/* Contador para obtener 6 Hz a partir de 60 Hz */
static volatile unsigned int hb_div10 = 0;

/* 60 Hz con tick de 8 us */
#define TIMER1_TICKS_60HZ  (2083u)

/* ISR vectorizada (IRQ) */
void timer1_ISR(void) __attribute__((interrupt("IRQ")));

void timer1_ISR(void)
{
//#if HB_DEBUG
    /* Marca de tiempo del evento de Timer1 (siempre que timer2_init() esté hecho) */
  //  cola_depuracion(timer2_count(), EVENTO_TIMER1, hb_div10);
//#endif

    /* Toggle LED derecho a 6 Hz = 1 toggle cada 10 eventos de 60 Hz */
    hb_div10++;
    if (hb_div10 >= 10u) {
        hb_div10 = 0u;
        led2_toggle();
    }

    /* Limpiar la solicitud de interrupción de TIMER1 */
    rI_ISPC |= BIT_TIMER1;
}

void timer1_init_60Hz(void)
{
    /* Prescaler0 = 255 explícitamente (afecta a T0 y T1). No toca Prescaler1 (T2-5). */
    rTCFG0 = (rTCFG0 & ~0xFFu) | 0xFFu;      /* Prescaler0 = 255 -> divisor 256 */

    /* Divisor para Timer1 = 1/2 (bits [5:4]) */
    rTCFG1 = (rTCFG1 & ~(0x3u << 4)) | (0x0u << 4);

    /* Desenmascarar IRQ de Timer1 */
    rINTMSK &= ~(BIT_TIMER1);

    /* Vectorizar ISR de Timer1 */
    pISR_TIMER1 = (unsigned)timer1_ISR;

    /* Cargar el valor de cuenta en TCNTB1 (descendente). Sin compare -> 1 IRQ por evento. */
    rTCNTB1 = TIMER1_TICKS_60HZ;
    rTCMPB1 = 0;

    /* TCON (Timer1 = bits [11:8]): stop -> manual update -> start + auto-reload */
    rTCON &= ~(0xFu << 8);                    /* stop/limpiar control T1 */
    rTCON |=  (0x2u << 8);                    /* manual update */
    rTCON =  (rTCON & ~(0xFu << 8)) | (0x9u << 8); /* start + auto-reload */

    /* Limpieza de pendientes */
    rI_ISPC |= BIT_TIMER1;
}
