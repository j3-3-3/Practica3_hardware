// Fichero:	cola.h

#ifndef __COLA_H__
#define __COLA_H__

#include <stdint.h>
#include "eventos.h"

#define COLA_TAM 64

/* Estructura de cada entrada de depuración */
typedef struct {
    uint32_t instant;    // 4 bytes
    uint8_t  ID_evento;  // 1 byte
    uint8_t  padding[3]; // 3 bytes
    uint32_t auxData;    // 4 bytes
    uint32_t reserved;   // 4 bytes de relleno para total = 16
} __attribute__((packed, aligned(16))) EventoDebug;

/* Inserta un nuevo evento en la cola */
void cola_depuracion(uint32_t instant, uint8_t ID_evento, uint32_t auxData);

/* Devuelve puntero a la cola y su tamaño */
EventoDebug* cola_get_base(void);
uint32_t cola_get_tam(void);


#endif /* __COLA_H__ */
