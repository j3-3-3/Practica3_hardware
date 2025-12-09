// Fichero:	button.c

#include "button.h"
#include "44b.h"
#include "44blib.h"
#include "cola.h"
#include "eventos.h"
#include "timer2.h"
#include "timer3.h"

/* Parámetros de configuración */
#define TIEMPO_REBOTE              20000u    /* 20 ms */
#define ESPERA_REPETICION          600000u   /* 600 ms */
#define RETARDO_REPETICION         300000u   /* 300 ms */

/* Estado interno */
static volatile boton_estado_t estado = BOTON_ESPERA;
static volatile uint8_t ultimo_boton = 0;
static volatile uint8_t last_click   = 0;

/* Auto-repetición */
static uint8_t  boton_repetido   = 0;
static uint8_t  repeticiones_generadas = 0;
static uint32_t rep_next   = 0;

static volatile uint8_t incrementos = 0;

static uint8_t leer_botones(void);
static uint8_t lectura_doble_estable(uint8_t esperado);

/* ISR EINT6/EINT7: Gestión de los pulsadores y sus rebotes de pulsación */
void Eint4567_ISR(void) __attribute__((interrupt("IRQ")));
void Eint4567_ISR(void)
{
    rINTMSK |= BIT_EINT4567;  /* mientras rebote, deshabilita nuevas externas */

    uint32_t t_us = timer2_count();
    uint32_t pend = rEXTINTPND;

    if (pend & (1u << 2)) {                          /* EINT6 */
        ultimo_boton = 1;
        cola_depuracion(t_us, EVT_BOTON_PRES, 1);
    } else if (pend & (1u << 3)) {                   /* EINT7 */
        ultimo_boton = 2;
        cola_depuracion(t_us, EVT_BOTON_PRES, 2);
    } else {
        ultimo_boton = 0;
    }

    if (ultimo_boton != 0) {
        estado = BOTON_REBOTE_PRES;
        timer3_start(TIEMPO_REBOTE);
    } else {
        estado = BOTON_ESPERA;
        rINTMSK &= ~(BIT_EINT4567);
    }

    rEXTINTPND = (1u << 2) | (1u << 3);
    rI_ISPC   |= BIT_EINT4567;
}

/* Llamada en main. Gestión de estados de los pulsadores. */
void button_gestion_timer(void)
{
    uint32_t ahora = timer2_count();

    switch (estado)
    {
        case BOTON_REBOTE_PRES:
            /* Ejecutado únicamente al finalizar el rebote.
               Confirmamos pulsación estable con lectura doble. */
            if (lectura_doble_estable(ultimo_boton)) {
                estado = BOTON_PULSADO;
                cola_depuracion(ahora, EVT_BOTON_ESTABLE, ultimo_boton);
                if (ultimo_boton == 2) {
                	boton_repetido    = 2;
                	repeticiones_generadas = 0;
                	rep_next   = ahora + ESPERA_REPETICION;
                }
                /* Rehabilitamos interrupciones externas para detectar la liberación posterior */
                rINTMSK &= ~(BIT_EINT4567);
            } else {
                /* Si falla el rebote, volver a espera */
                estado = BOTON_ESPERA;
                ultimo_boton = 0;
                rINTMSK &= ~(BIT_EINT4567);
            }
            break;

        case BOTON_PULSADO: {
            uint8_t lec = leer_botones();
            if (lec == 0) {
                /* Inicio rebote de liberación */
                estado = BOTON_REBOTE_LIB;
                timer3_start(TIEMPO_REBOTE);
            } else {
                /* Auto-repetición para botón derecho */
                if (boton_repetido == 2 && lec == 2) {
                    if ((int32_t)(ahora - rep_next) >= 0) {
                        if (incrementos < 255)
                        	incrementos++;
                        repeticiones_generadas++;
                        rep_next += RETARDO_REPETICION;
                    }
                }
            }
            break;
        }

        case BOTON_REBOTE_LIB:
            /* Se procesa al expirar Timer3 */
            if (lectura_doble_estable(0)) {
                cola_depuracion(ahora, EVT_BOTON_LIBERA, ultimo_boton);
                cola_depuracion(ahora, EVT_BOTON_LISTO,  ultimo_boton);

                if (ultimo_boton == 1) {
                    last_click = 1;
                } else if (ultimo_boton == 2) {
                    /* Pulsación corta (sin repetir) => un incremento */
                    if (repeticiones_generadas == 0 && incrementos < 255)
                    	incrementos++;
                }

                ultimo_boton = 0;
                boton_repetido = 0;
                repeticiones_generadas = 0;
                estado = BOTON_ESPERA;
                rINTMSK &= ~(BIT_EINT4567);
            } else {
                /* Ruido: volver a considerar pulsado */
                estado = BOTON_PULSADO;
                rINTMSK &= ~(BIT_EINT4567);
            }
            break;

        default:
            break;
    }
}

/* Consumo de eventos */
uint8_t button_recibir_click(void)
{
    uint8_t c = last_click;
    last_click = 0;
    return c;
}

/* Consumo de eventos */
uint8_t button_recibir_incremento(void)
{
    if (incrementos) {
    	incrementos--;
        return 1;
    }
    return 0;
}

/* Lecturas físicas de los pulsadores */
static uint8_t leer_botones(void)
{
    uint32_t v = rPDATG;
    if (!(v & (1<<6))) return 1;    // 1 -> botón izquierdo
    if (!(v & (1<<7))) return 2;    // 2 -> botón derecho
    return 0;
}

/* Comprueba dos veces seguidas el estado del pulsador y solo acepta el valor
 * si ambas lecturas coinciden */
static uint8_t lectura_doble_estable(uint8_t esperado)
{
    uint8_t a = leer_botones();
    volatile int i;
    for (i = 0; i < 120; ++i) { __asm__ __volatile__(""); }
    uint8_t b = leer_botones();
    return (a == esperado) && (b == esperado);
}

/* Inicialización */
void Eint4567_init(void)
{
    rI_ISPC     |= BIT_EINT4567;
    rEXTINTPND   = (1u << 2) | (1u << 3);

    pISR_EINT4567 = (unsigned)Eint4567_ISR;

    rPCONG |= 0xFF00;
    rPUPG  = 0x0;
    rEXTINT = (rEXTINT & ~(0x7FFF << 16)) | (0x2AAA << 16);

    rINTMSK &= ~(BIT_EINT4567);

    rEXTINTPND = (1u << 2) | (1u << 3);
    rI_ISPC   |= BIT_EINT4567;

    estado = BOTON_ESPERA;
    ultimo_boton = 0;
    last_click = 0;
    boton_repetido = 0;
    repeticiones_generadas = 0;
    incrementos = 0;
}
