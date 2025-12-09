#include "44b.h"
#include "44blib.h"
#include "timer3.h"
#include "eventos.h"
#include "cola.h"
#include "timer2.h"
#include "button.h"

/* Estado local del timer3 (solo para saber si estaba activo) */
static volatile uint8_t timer3_activo = 0;

/* ISR: NO accede a variables internas de button.c porque son estáticas allí.
   Solo marca el evento y delega la lógica a button_gestion_timer(). */
void timer3_ISR(void) __attribute__((interrupt("IRQ")));
void timer3_ISR(void)
{
    cola_depuracion(timer2_count(), EVENTO_TIMER3, 0);
    button_gestion_timer();   /* delega avance de rebote / liberación */
    timer3_stop();
    rI_ISPC |= BIT_TIMER3;
}

void timer3_init(void)
{
    /* Desenmascarar IRQ de Timer3 */
    rINTMSK &= ~(BIT_TIMER3);

    /* Vectorizar ISR */
    pISR_TIMER3 = (unsigned)timer3_ISR;

    /* Prescaler1 = 255 para T2..T5; divisor 1/2 para T3 */
    rTCFG0 = (rTCFG0 & 0x00FF) | (0xFFu << 8);
    rTCFG1 = (rTCFG1 & ~(0xFu << 12)) | (0x0u << 12);

    /* Limpiar control y registros */
    rTCON &= ~(0xF << 12);
    rTCNTB3 = 0xFFFF;
    rTCMPB3 = 0;

    /* Limpiar pendiente */
    rI_ISPC |= BIT_TIMER3;
    timer3_activo = 0;
}

void timer3_start(uint32_t usec)
{
    unsigned int presc1 = ((rTCFG0 >> 8) & 0xFF) + 1;
    unsigned int mux = (rTCFG1 >> 12) & 0x3;
    unsigned int div = (mux == 0) ? 2 : (1u << (mux + 1));
    unsigned int tick_us = (presc1 * div) / 64; /* MCLK=64MHz */

    unsigned int ticks = usec / tick_us;
    if (ticks > 0xFFFF) ticks = 0xFFFF;

    rTCNTB3 = ticks;

    /* Secuencia: stop -> manual update -> start + auto-reload */
    rTCON = (rTCON & ~(0xF << 12)) | (0x0 << 12);
    rTCON = (rTCON & ~(0xF << 12)) | (0x2 << 12);
    rTCON = (rTCON & ~(0xF << 12)) | (0x9 << 12);

    timer3_activo = 1;
}

void timer3_stop(void)
{
    rTCON &= ~(0x1 << 12);
    timer3_activo = 0;
}
