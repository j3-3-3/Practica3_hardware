/*********************************************************************************************
* File	main.c
* Author: embest
* Description: main entry
* History:	
*********************************************************************************************/

/*--- include files ---*/
#include "44blib.h"
#include "44b.h"
#include "tp.h"

	char yn;
/*--- function declare ---*/
void Main(void);

/*--- extern function ---*/
extern void Lcd_Test();

/*--- function code ---*/
/*********************************************************************************************
* name:		main
* func:		c code entry
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Main(void)
{
    sys_init();        /* Initial 44B0X's Interrupt, Port and UART */
    _Link();           /* Print Misc info */
        /******************/
	/* user interface */
	/******************/
	Lcd_Test();
	TS_Test();
		
	while(1)
	 {
	   yn = Uart_Getch();
	   
	   if(yn == 0x52) TS_Test();// R to reset the XY REC
	   //else break;
	 }
	
	TS_close();
}
