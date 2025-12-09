// Fichero:	main.c

#include "8led.h"
#include "button.h"
#include "led.h"
#include "timer.h"
#include "44blib.h"
#include "44b.h"
#include "timer2.h"
#include "timer1.h"
#include "timer3.h"
#include "sudoku_estados.h"

void Main(void)
{
    sys_init();
    Eint4567_init();
    D8Led_init();
    timer1_init_60Hz();
    timer2_init();
    timer3_init();

    timer2_start();
    sudoku_init();

    leds_off();
    led1_on();

    while (1)
    {
        button_gestion_timer();
        sudoku_tick();
    }
}
