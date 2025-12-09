.global _ISR_STARTADDRESS
.global DoUndef
.global DoSWI
.global DoDabort
.global screen

.equ 	_ISR_STARTADDRESS,	0xc7fff00		/* GCS6:64M DRAM/SDRAM 	*/
.equ    UserStack,   _ISR_STARTADDRESS-0xf00         /* c7ff000 */
.equ    SVCStack,    _ISR_STARTADDRESS-0xf00+256     /* c7ff100 */
.equ    UndefStack,  _ISR_STARTADDRESS-0xf00+256*2   /* c7ff200 */
.equ    AbortStack,  _ISR_STARTADDRESS-0xf00+256*3   /* c7ff300 */
.equ    IRQStack,    _ISR_STARTADDRESS-0xf00+256*4   /* c7ff400 */
.equ    FIQStack,    _ISR_STARTADDRESS-0xf00+256*5   /* c7ff500 */

.equ    HandleReset,    _ISR_STARTADDRESS
.equ    HandleUndef,    _ISR_STARTADDRESS+4
.equ    HandleSWI,      _ISR_STARTADDRESS+4*2
.equ    HandlePabort,   _ISR_STARTADDRESS+4*3
.equ    HandleDabort,   _ISR_STARTADDRESS+4*4
.equ    HandleReserved, _ISR_STARTADDRESS+4*5
.equ    HandleIRQ,      _ISR_STARTADDRESS+4*6
.equ    HandleFIQ,      _ISR_STARTADDRESS+4*7

/* S’mbolos para facilitar la codificaci—n
de los modos de ejecuci—n */
.equ 	USERMODE,	0x10
.equ 	FIQMODE,	0x11
.equ 	IRQMODE,	0x12
.equ 	SVCMODE,	0x13
.equ 	ABORTMODE,	0x17
.equ 	UNDEFMODE,	0x1b
.equ 	MODEMASK,	0x1f

.equ 	NOINT,		0xc0   /* Mascara para deshabilitar interrupciones */
.equ    IRQ_MODE,	0x40   /* deshabilitar interrupciones en modo IRQ  */
.equ    FIQ_MODE,	0x80   /* deshabilitar interrupciones en modo FIQ  */

.equ 	INTMSK,		0x01e0000c
.equ 	WTCON,		0x01d30000


.extern       Image_RO_Limit    /* End of ROM code (=start of ROM data) */
.extern       Image_RW_Base     /* Base of RAM to initialize */           
.extern       Image_ZI_Base     /* Base and limit of area */              
.extern       Image_ZI_Limit    /* to zero initialise */       

.macro HANDLER HandleLabel
    sub	    sp,sp,#4	    /* decrement sp(to store jump address) */							
    stmfd   sp!,{r0}	    /* PUSH the work register to stack(lr does't push because it return to original address) */
    ldr	    r0,=\HandleLabel/* load the address of HandleXXX to r0 */
    ldr	    r0,[r0]	    	/* load the contents(service routine start address) of HandleXXX */
    str	    r0,[sp,#4]	    /* store the contents(ISR) of HandleXXX to stack */
    ldmfd   sp!,{r0,pc}	    /* POP the work register and pc(jump to ISR) */
.endm

.arm
start:
    b ResetHandler			/* for debug            */
    b HandlerUndef      	/* handlerUndef         */
    b HandlerSWI        	/* SWI interrupt handler*/
    b HandlerPabort     	/* handlerPAbort        */
    b HandlerDabort     	/* handlerDAbort        */
    b .                 	/* handlerReserved      */
    b HandlerIRQ
    b HandlerFIQ

   .align

HandlerFIQ:		HANDLER HandleFIQ
HandlerIRQ:		HANDLER HandleIRQ
HandlerUndef:	HANDLER HandleUndef
HandlerSWI:		HANDLER HandleSWI
HandlerDabort:	HANDLER HandleDabort
HandlerPabort:	HANDLER HandlePabort


ResetHandler:
    ldr	    r0,=WTCON	    	/* watch dog disable*/
    ldr	    r1,=0x0 		
    str	    r1,[r0]

    ldr	    r0,=INTMSK
    ldr	    r1,=0x07ffffff  	/* all interrupt disable */
    str	    r1,[r0]

    ldr	    r0,=(SMRDATA-0xc000000)
    ldmia   r0,{r1-r13}
    ldr	    r0,=0x01c80000  	/* BWSCON Address */
    stmia   r0,{r1-r13}

	LDR r0,=0x0
	LDR r1,=Image_RO_Base
	LDR r3,=Image_ZI_Base
LoopRw:
    cmp         r1, r3
	ldrcc       r2, [r0], #4
	strcc       r2, [r1], #4
	bcc         LoopRw

/* código nuevo (Darío) */
        LDR r0, =Image_ZI_Base
        LDR r1, =Image_ZI_Limit
        mov r3, #0
LoopZI:
        cmp r0, r1
        strcc r3, [r0], #4
        bcc LoopZI
/* fin código nuevo (Darío) */

	ldr pc,=Init

Init:
	nop
	nop
	nop
    /* Si comenzamos con un reset
     el procesador esta en modo supervisor */
    bl InitStacks
	
    /* Seguimos en modo supervisor, configuramos
       las direcciones de las rutinas de tratamiento
       de excepciones */
    bl InitISR

    /* Pasamos a modo usuario, inicializamos su pila
      y ponemos a cero el frame pointer*/
    mrs	    r0,cpsr
   	bic	r0,r0,#MODEMASK
    orr	r1,r0,#USERMODE
    msr	cpsr_cxsf,r1 	    /* SVCMode */
    ldr sp,=UserStack
    mov fp,#0

    /* Saltamos a Main */
    .extern Main

    ldr r0,=Main
    mov lr,pc
    mov pc,r0
	b .
InitStacks:
    /* Codigo de la primera parte */
    mrs	    r0,cpsr
    bic	    r0,r0,#MODEMASK
    orr	    r1,r0,#UNDEFMODE
    msr	    cpsr_cxsf,r1		/* UndefMode */
    ldr	    sp,=UndefStack
	
    orr	    r1,r0,#ABORTMODE|NOINT
    msr	    cpsr_cxsf,r1 	    /* AbortMode */	
    ldr	    sp,=AbortStack

    orr	    r1,r0,#IRQMODE|FIQ_MODE
    msr	    cpsr_cxsf,r1 	    /* IRQMode */
    ldr	    sp,=IRQStack
	
    orr	    r1,r0,#FIQMODE|IRQ_MODE
    msr	    cpsr_cxsf,r1 	    /* FIQMode */
    ldr	    sp,=FIQStack

    bic	    r0,r0,#MODEMASK
    orr	    r1,r0,#SVCMODE
    msr	    cpsr_cxsf,r1 	    /* SVCMode */
    ldr	    sp,=SVCStack
    
    mov pc, lr

InitISR:
    /* Codigo de la primera parte */
    ldr r0,=ISR_Undef
    ldr r1,=HandleUndef
    str r0,[r1]
    
    ldr r0,=ISR_Dabort
    ldr r1,=HandleDabort
    str r0,[r1]
    
    ldr r0,=ISR_Pabort
    ldr r1,=HandlePabort
    str r0,[r1]

    ldr r0,=ISR_IRQ
    ldr r1,=HandleIRQ
    str r0,[r1]
    
    ldr r0,=ISR_SWI
    ldr r1,=HandleSWI
    str r0,[r1]
    
    ldr r0,=ISR_FIQ
    ldr r1,=HandleFIQ
    str r0,[r1]

	mov pc,lr

DoSWI:
	swi
	mov pc,lr

DoUndef:
	.word 0xE6000010
	mov pc,lr

DoDabort:
	ldr r0,=0x0a333333
	str r0,[r0]
	mov pc,lr

SMRDATA:
	.word 0x11110102	@ BWSCON
	.word 0x00000600	@ BANKCON0
	.word 0x00007FFC	@ BANKCON1
	.word 0x00007FFC	@ BANKCON2
	.word 0x00007FFC	@ BANKCON3
	.word 0x00007FFC	@ BANKCON4
	.word 0x00007FFC	@ BANKCON5
	.word 0x00018000	@ BANKCON6
	.word 0x00018000	@ BANKCON7
	.word 0x00860459	@ REFRESH
	.word 0x00000010	@ BANKSIZE
	.word 0x00000020	@ MRSRB6
	.word 0x00000020	@ MRSRB7

screen:
	.space 1024

.end
