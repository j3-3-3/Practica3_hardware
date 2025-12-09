#include "timer2.h"
#include "44b.h"
#include "44blib.h"
#include "eventos.h"
#include "cola.h"

#define T2_PERIOD_TICKS     65536u
#define T2_TCON_SHIFT       12

/* Overflows acumulados (periodos completos) */
static volatile unsigned int timer2_numero_int = 0;

static void timer2_ISR(void) __attribute__((interrupt("IRQ")));

/* Calcula �s por tick seg�n configuraci�n real */
static unsigned int t2_tick_us_from_regs(void)
{
    /* prescaler1 = (rTCFG0[15:8] + 1) */
    unsigned int presc1 = ((rTCFG0 >> 8) & 0xFF) + 1;

    /* MUX T2 (rTCFG1[9:8]): 00->2, 01->4, 10->8, 11->16  */
    unsigned int mux = (rTCFG1 >> 8) & 0x3;
    unsigned int div = (mux == 0) ? 2 : (1u << (mux + 1));

    /* MCLK = 64 MHz -> divide por 64 para pasar a microsegundos */
    return (presc1 * div) / 64u; /* normalmente 8 us */
}

static void timer2_ISR(void)
{
    timer2_numero_int++;
    cola_depuracion(
    				timer2_count(),        /* instante exacto  */
    				EVENTO_TIMER2,         /* ID del evento ) */
    				timer2_numero_int      /* dato auxiliar, por ejemplo numero de overflow */
    );

    rI_ISPC |= BIT_TIMER2;
}

void timer2_init(void)
{
    /* Desenmascara IRQ de Timer2 y vectoriza ISR */
    rINTMSK &= ~(BIT_TIMER2);
    pISR_TIMER2 = (unsigned)timer2_ISR;

    /* Prescaler1 (grupo T2..T5) = 255 sin tocar prescaler0 (T0/T1) */
    rTCFG0 = (rTCFG0 & 0x00FF) | (0xFFu << 8);

    /* MUX T2 = 00 (1/2). Solo nibble de T2 en rTCFG1 */
    rTCFG1 = (rTCFG1 & ~(0xFu << 8)) | (0x0u << 8);

    /* Carga valor inicial y limpia pendientes */
    rTCNTB2 = 65535u;
    rTCMPB2 = 0u;
    rI_ISPC |= BIT_TIMER2;

    /* stop -> manual update -> start+auto para T2 (nibble [15:12]) */
    rTCON = (rTCON & ~(0xFu << T2_TCON_SHIFT)) | (0x0u << T2_TCON_SHIFT);
    rTCON = (rTCON & ~(0xFu << T2_TCON_SHIFT)) | (0x2u << T2_TCON_SHIFT);
    rTCON = (rTCON & ~(0xFu << T2_TCON_SHIFT)) | (0x9u << T2_TCON_SHIFT);
}

void timer2_start(void)
{
    timer2_numero_int = 0;
    rTCNTB2 = 65535u;
    rI_ISPC |= BIT_TIMER2;

    /* stop -> manual update -> start+auto para T2 */
    rTCON = (rTCON & ~(0xFu << T2_TCON_SHIFT)) | (0x0u << T2_TCON_SHIFT);
    rTCON = (rTCON & ~(0xFu << T2_TCON_SHIFT)) | (0x2u << T2_TCON_SHIFT);
    rTCON = (rTCON & ~(0xFu << T2_TCON_SHIFT)) | (0x9u << T2_TCON_SHIFT);
}

unsigned int timer2_count(void)
{
    unsigned int hi1, hi2, lo_ticks, total_ticks;

    /* Lectura robusta para evitar carrera en overflow */
    do {
        hi1 = timer2_numero_int;
        lo_ticks = (unsigned int)(65535u - rTCNTO2); /* ticks transcurridos en periodo actual */
        hi2 = timer2_numero_int;
    } while (hi1 != hi2);

    total_ticks = (hi1 * T2_PERIOD_TICKS) + lo_ticks;
    return total_ticks * t2_tick_us_from_regs(); /* �s */
}


