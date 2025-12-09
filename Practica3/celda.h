// Fichero:	celda.h

#ifndef CELDA_H
#define CELDA_H

#include <stdint.h>

/* Codificación de la celda (16 bits)
 * bits [15..7]: 9 bits candidatos (bit=1 => ELIMINADO, bit=0 => candidato disponible)
 * bit 5 (0x0020): error
 * bit 4 (0x0010): pista (no modificable)
 * bits [3..0]: valor (0..9)
 */
enum { BIT_CANDIDATOS = 7 };

/* Bits de flag */
#define CELDA_FLAG_ERROR  (0x0020u)
#define CELDA_FLAG_PISTA  (0x0010u)

typedef uint16_t CELDA;

/* Elimina el candidato 'valor' (1..9) en la celda si aplica */
static inline void celda_eliminar_candidato(CELDA *celdaptr, uint8_t valor)
{
    if (valor >= 1 && valor <= 9)
        *celdaptr |= (uint16_t)(1u << (BIT_CANDIDATOS + valor - 1));
}

/* Pone el valor (0..9) en la celda (conserva flags y candidatos) */
static inline void celda_poner_valor(CELDA *celdaptr, uint8_t val)
{
    *celdaptr = (uint16_t)((*celdaptr & 0xFFF0u) | (val & 0x0Fu));
}

/* Lee el valor (0..9) de la celda */
static inline uint8_t celda_leer_valor(CELDA celda)
{
    return (uint8_t)(celda & 0x000Fu);
}

#endif /* CELDA_LOCAL_H */
