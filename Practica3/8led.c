/*********************************************************************************************
* Fichero:	8led.c
* Autor:
* Descrip:	Funciones de control del display 8-segmentos
* Version:
*********************************************************************************************/

#include "8led.h"
#include "44b.h"
#include "44blib.h"

static int
Symbol[size_8led] = { cero, uno, dos, tres, cuatro, cinco, seis, siete,
		              ocho, nueve, A, B, C, D, E, F, blank};

/* Estado inicial del display */
void D8Led_init(void)
{
	LED8ADDR = (unsigned char) F;
}

/* Muestra el valor indicado en el display  */
void D8Led_symbol(int value)
{
	if ((value >= 0) && (value < size_8led)) {
		LED8ADDR = (unsigned char) Symbol[value];
	}
}
