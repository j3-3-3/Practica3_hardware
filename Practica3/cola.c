// Fichero:	cola.c

#include "cola.h"

/* Cola circular de depuración, alineada a 16 bytes */
static EventoDebug cola[COLA_TAM] __attribute__((aligned(16)));

/* Índice de inserción */
static volatile uint32_t indice = 0;

/* Inserta un nuevo evento en la cola */
void cola_depuracion(uint32_t instant, uint8_t ID_evento, uint32_t auxData)
{
    cola[indice].instant = instant;
    cola[indice].ID_evento = ID_evento;
    cola[indice].auxData = auxData;

    /* Avance circular del índice */
    indice++;
    if (indice >= COLA_TAM)
        indice = 0;
}

/* Devuelve base de la cola */
EventoDebug* cola_get_base(void)
{
    return cola;
}

/* Devuelve tamaño máximo de la cola */
uint32_t cola_get_tam(void)
{
    return COLA_TAM;
}
