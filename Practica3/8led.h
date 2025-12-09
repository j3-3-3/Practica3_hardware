/*********************************************************************************************
* Fichero:	8led.h
* Autor:
* Descrip:	Funciones de control del display 8-segmentos
* Version:
*********************************************************************************************/

#ifndef _8LED_H_
#define _8LED_H_

/* Mapa de bits de cada segmento
  (valor que se debe escribir en el display para que se ilumine el segmento) */
enum {
    cero      = ~0xED,
    uno	      = ~0x60,
    dos       = ~0xCE,
    tres      = ~0xEA,
    cuatro    = ~0x63,
    cinco     = ~0xAB,
    seis      = ~0x2F,
    siete     = ~0xE0,
    ocho      = ~0xEF,
    nueve     = ~0xE3,
    A         = ~0xE7,
    B         = ~0x2F,
    C         = ~0x8D,
    D         = ~0x6E,
    E         = ~0x8F,
    F         = ~0x87,
    blank     = ~0x00,
    size_8led = 17 };


/* Índices (0..16) para usar con D8Led_symbol */
#define D8_0      0
#define D8_1      1
#define D8_2      2
#define D8_3      3
#define D8_4      4
#define D8_5      5
#define D8_6      6
#define D8_7      7
#define D8_8      8
#define D8_9      9
#define D8_A     10
#define D8_B     11
#define D8_C     12
#define D8_D     13
#define D8_E     14
#define D8_F     15
#define D8_BLANK 16

void D8Led_init(void);

/* value = índice (0..16) */
void D8Led_symbol(int value);

/* OPCIONAL: escribir patrón directamente (bitmap) */
void D8Led_pattern(int pattern);

#endif /* _8LED_H_ */
