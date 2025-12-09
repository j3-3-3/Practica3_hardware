/* =========================================================================
 * candidatos_propagar_arm
 * void candidatos_propagar_arm(CELDA *cuadricula, uint8_t fila, uint8_t columna)
 * Propaga el valor de una celda fija (1–9) eliminando dicho valor como
 * candidato de las celdas vacías en la misma fila, columna y región 3x3.
 *
 * Prólogo/Epílogo: PUSH {r4–r7, r11, lr} / POP {r4–r7, r11, pc}
 * Registros usados:
 *   r0–r2 : parámetros (base, fila, columna)
 *   r3–r6 : temporales / cálculo de direcciones
 *   r7    : valor propagado
 *   r11   : frame pointer
 * ========================================================================= */
 	.text
    .global candidatos_propagar_arm
    .type   candidatos_propagar_arm, %function

candidatos_propagar_arm:
    push    {r4-r7, r11, lr}
    add     r11, sp, #0
    sub     sp, sp, #32

    @ Guardar parámetros en el frame
    str     r0, [r11, #-28]       @ base pointer (word)
    strb    r1, [r11, #-17]       @ fila (byte)
    strb    r2, [r11, #-18]       @ columna (byte)

    @ --- obtener valor = celda_leer_valor( cuadricula[fila][col] )
    ldrb    r3, [r11, #-17]       @ r3 = fila
    lsl     r3, r3, #5
    ldr     r4, [r11, #-28]       @ r4 = base
    add     r3, r4, r3
    ldrb    r4, [r11, #-18]       @ r4 = col
    lsl     r4, r4, #1
    add     r3, r3, r4
    ldrh    r0, [r3]
    bl      celda_leer_valor      @ r0 = valor

    cmp     r0, #0
    beq     _prop_fin             @ si valor==0 nada que propagar
    mov     r7, r0                @ r7 = valor

    @ ---------------------------
    @ Recorrer fila (j = 0..8)
    @ ---------------------------
    movs    r5, #0                @ j = 0
_fila_loop:
    cmp     r5, #9
    beq     _fila_fin
    ldrb    r6, [r11, #-18]       @ col
    cmp     r5, r6
    beq     _fila_inc

    ldrb    r3, [r11, #-17]       @ fila
    lsl     r3, r3, #5
    ldr     r4, [r11, #-28]       @ base
    add     r3, r4, r3
    mov     r4, r5
    lsl     r4, r4, #1
    add     r3, r3, r4            @ r3 = &cuadricula[fila][j]

    mov     r0, r3
    mov     r1, r7
    bl      celda_eliminar_candidato

_fila_inc:
    adds    r5, r5, #1
    b       _fila_loop
_fila_fin:

    @ ---------------------------
    @ Recorrer columna (i = 0..8)
    @ ---------------------------
    movs    r5, #0                @ i = 0
_col_loop:
    cmp     r5, #9
    beq     _col_fin
    ldrb    r6, [r11, #-17]       @ fila original
    cmp     r5, r6
    beq     _col_inc

    mov     r3, r5
    lsl     r3, r3, #5
    ldr     r4, [r11, #-28]       @ base
    add     r3, r4, r3
    ldrb    r4, [r11, #-18]       @ col
    lsl     r4, r4, #1
    add     r3, r3, r4            @ r3 = &cuadricula[i][col]

    mov     r0, r3
    mov     r1, r7
    bl      celda_eliminar_candidato

_col_inc:
    adds    r5, r5, #1
    b       _col_loop
_col_fin:

    @ ---------------------------
    @ Recorrer región 3x3
    @ ---------------------------
    ldr     r0, =init_region
    ldrb    r1, [r11, #-17]       @ fila
    ldrb    r2, [r0, r1]          @ init_i
    ldrb    r1, [r11, #-18]       @ col
    ldrb    r3, [r0, r1]          @ init_j
    add     r2, r2, #3
    add     r3, r3, #3

    strb    r2, [r11, #-21]       @ end_i
    strb    r3, [r11, #-22]       @ end_j
    ldrb    r5, [r11, #-17]       @ i = init_i
    ldrb    r5, [r0, r5]
    mov     r6, r5

_reg_i_loop:
    ldrb    r8, [r11, #-21]       @ end_i
    cmp     r6, r8
    bge     _prop_regions_done

    mov     r9, r3                @ j = init_j
    ldrb    r9, [r11, #-20]       @ init_j
_reg_j_loop:
    ldrb    r8, [r11, #-22]       @ end_j
    cmp     r9, r8
    bge     _reg_next_i

    ldrb    r4, [r11, #-17]
    cmp     r6, r4
    bne     _reg_check
    ldrb    r4, [r11, #-18]
    cmp     r9, r4
    beq     _reg_inc_j

_reg_check:
    mov     r3, r6
    lsl     r3, r3, #5
    ldr     r4, [r11, #-28]
    add     r3, r4, r3
    mov     r4, r9
    lsl     r4, r4, #1
    add     r3, r3, r4            @ r3 = &cuadricula[i][j]

    mov     r0, r3
    bl      celda_leer_valor
    cmp     r0, #0
    bne     _reg_inc_j

    mov     r0, r3
    mov     r1, r7
    bl      celda_eliminar_candidato

_reg_inc_j:
    adds    r9, r9, #1
    b       _reg_j_loop

_reg_next_i:
    adds    r6, r6, #1
    b       _reg_i_loop

_prop_regions_done:
    @ fin propagación

_prop_fin:
    mov     r0, #0
    add     sp, sp, #32
    pop     {r4-r7, r11, pc}

    .section .rodata
    .align 2
init_region:
    .byte 0,0,0,3,3,3,6,6,6
    .ltorg
    .text

/* =========================================================================
 * candidatos_actualizar_arm
 * int candidatos_actualizar_arm(CELDA cuadricula[][NUM_COLUMNAS])
 * Recorre todas las celdas válidas (9x9):
 *   1. Limpia los bits de candidatos.
 *   2. Recalcula y propaga los valores fijos llamando a candidatos_propagar_arm.
 * Registros usados:
 *   r0 : base de la cuadrícula / valor de retorno
 *   r4 : base de la cuadrícula
 *   r5 : índice de fila
 *   r6 : índice de columna
 *   r7–r8 : direcciones temporales y valores intermedios
 *   r11 : contador de celdas vacías
 * ========================================================================= */
    .global candidatos_actualizar_arm
    .type   candidatos_actualizar_arm, %function
candidatos_actualizar_arm:
    PUSH    {r4-r11, lr}        @ preservar registros

    CMP     r0, #0
    BEQ     ca_exit

    MOV     r4, r0              @ r4 = base pointer cuadricula
    MOV     r5, #0              @ i = 0
    MOV     r11, #0             @ contador celdas_vacias en r11

    @ -------------------------------
    @ Borrar bits de candidatos en toda la zona válida (9x9)
    @ máscara KEEP LOW 7 bits = 0x007F
    @ -------------------------------
ca_clear_rows:
    CMP     r5, #9
    BEQ     ca_clear_done
    MOV     r6, #0              @ j = 0

ca_clear_cols:
    CMP     r6, #9
    BEQ     ca_clear_next_row

    LSL     r7, r5, #4
    ADD     r7, r7, r6
    LSL     r7, r7, #1
    ADD     r7, r4, r7          @ &cuadricula[i][j]
    LDRH    r8, [r7]
    AND     r8, r8, #0x007F
    STRH    r8, [r7]

    ADD     r6, r6, #1
    B       ca_clear_cols

ca_clear_next_row:
    ADD     r5, r5, #1
    B       ca_clear_rows

ca_clear_done:

    @ -------------------------------
    @ Recalcular candidatos y contar celdas vacías
    @ -------------------------------
    MOV     r5, #0              @ i = 0

ca_loop_rows:
    CMP     r5, #9
    BEQ     ca_done
    MOV     r6, #0              @ j = 0

ca_loop_cols:
    CMP     r6, #9
    BEQ     ca_next_row

    LSL     r7, r5, #4
    ADD     r7, r7, r6
    LSL     r7, r7, #1
    ADD     r7, r4, r7          @ &cuadricula[i][j]
    LDRH    r1, [r7]

    MOV     r0, r1
    BL      celda_leer_valor    @ r0 = valor
    CMP     r0, #0
    BEQ     ca_inc_empty
    CMP     r0, #9
    BHI     ca_skip_propag

    @ llamar a candidatos_propagar_arm(base, i, j)
    MOV     r0, r4              @ base
    MOV     r1, r5              @ fila
    MOV     r2, r6              @ columna
    BL      candidatos_propagar_arm

    B       ca_after_cell

ca_inc_empty:
    ADD     r11, r11, #1        @ celdas_vacias++
    B       ca_after_cell

ca_skip_propag:
    B       ca_after_cell

ca_after_cell:
    ADD     r6, r6, #1
    B       ca_loop_cols

ca_next_row:
    ADD     r5, r5, #1
    B       ca_loop_rows

ca_done:
    MOV     r0, r11             @ devolver contador en r0

ca_exit:
    POP     {r4-r11, pc}

    .text
    .align 2
    .global candidatos_actualizar_all
    .type   candidatos_actualizar_all, %function

/* =========================================================================
 * candidatos_actualizar_all
 * int candidatos_actualizar_all(CELDA cuadricula[][NUM_COLUMNAS])
 * Combina candidatos_actualizar_arm y candidatos_propagar_arm
 * Devuelve en r0 el número de celdas vacías.
 *
 * Registros usados:
 *   r0 : base de la cuadrícula / valor de retorno
 *   r4 : base (CELDA*)
 *   r5 : fila actual
 *   r6 : columna actual
 *   r7 : desplazamiento temporal
 *   r8 : contador de celdas vacías
 *   r11: frame pointer local
 * ========================================================================= */
candidatos_actualizar_all:
    push    {r4-r7, r11, lr}      @ save callee-saved + lr
    add     r11, sp, #0           @ frame pointer
    sub     sp, sp, #32           @ reserve local stack

    cmp     r0, #0
    beq     ca_all_exit           @ if null pointer, exit with 0

    str     r0, [r11, #-28]       @ save base pointer

    @ =====================================================
    @ 1) Limpiar candidatos: for i=0..8, j=0..8 -> cell &= 0x007F
    @ =====================================================
    movs    r5, #0                @ i = 0
ca_clear_rows_all:
    cmp     r5, #9
    beq     ca_clear_done_all
    movs    r6, #0                @ j = 0

ca_clear_cols_all:
    cmp     r6, #9
    beq     ca_clear_next_row_all

    ldr     r4, [r11, #-28]       @ r4 = base
    lsl     r7, r5, #4            @ r7 = i * 16
    add     r7, r7, r6            @ r7 = i*16 + j
    lsl     r7, r7, #1            @ r7 = (i*16 + j) * 2
    add     r7, r4, r7            @ r7 = &cuadricula[i][j]

    ldrh    r0, [r7]              @ r0 = cell
    and     r0, r0, #0x007F
    strh    r0, [r7]

    add     r6, r6, #1
    b       ca_clear_cols_all

ca_clear_next_row_all:
    add     r5, r5, #1
    b       ca_clear_rows_all

ca_clear_done_all:
    @ =====================================================
    @ 2) Recalcular y propagar — contar celdas vacías
    @ =====================================================
    movs    r5, #0                @ i = 0
    movs    r8, #0                @ contador vacías = 0

ca_loop_rows_all:
    cmp     r5, #9
    beq     ca_done_all
    movs    r6, #0                @ j = 0

ca_loop_cols_all:
    cmp     r6, #9
    beq     ca_next_row_all

    ldr     r4, [r11, #-28]       @ base
    lsl     r7, r5, #4            @ r7 = i * 16
    add     r7, r7, r6            @ r7 = i*16 + j
    lsl     r7, r7, #1            @ byte offset
    add     r7, r4, r7            @ r7 = &cuadricula[i][j]

    ldrh    r0, [r7]              @ raw cell (halfword)
    bl      celda_leer_valor      @ r0 = valor (0..15)

    cmp     r0, #0
    beq     ca_inc_empty_all

    cmp     r0, #9
    bhi     ca_skip_propag_all    @ if value > 9 treat as invalid

    @ llamar a candidatos_propagar_arm(base, i, j)
    ldr     r0, [r11, #-28]       @ r0 = base
    mov     r1, r5                @ fila
    mov     r2, r6                @ columna
    bl      candidatos_propagar_arm

    b       ca_after_cell_all

ca_inc_empty_all:
    add     r8, r8, #1
    b       ca_after_cell_all

ca_skip_propag_all:
    b       ca_after_cell_all

ca_after_cell_all:
    add     r6, r6, #1
    b       ca_loop_cols_all

ca_next_row_all:
    add     r5, r5, #1
    b       ca_loop_rows_all

ca_done_all:
    mov     r0, r8                @ devolver contador

    add     sp, sp, #32
    pop     {r4-r7, r11, pc}

ca_all_exit:
    mov     r0, #0
    add     sp, sp, #32
    pop     {r4-r7, r11, pc}

/* *****************************************************************************
 * void celda_poner_valor(CELDA *celdaptr, uint8_t val)
 * r0 = puntero a celda
 * r1 = valor
 */
celda_poner_valor:
    PUSH    {r4, lr}
    LDRH    r2, [r0]
    LDR     r3, =0xFFF0
    AND     r2, r2, r3
    AND     r3, r1, #0x000F
    ORR     r2, r2, r3
    STRH    r2, [r0]
    POP     {r4, pc}

/* *****************************************************************************
 * uint8_t celda_leer_valor(CELDA celda)
 * r0 = valor de celda (entregado en r0)
 * return (celda & 0x000F)
 */
celda_leer_valor:
    AND     r0, r0, #0x000F
    BX      lr

/* *****************************************************************************
 * uint8_t celda_eliminar_candidato(CELDA *celda, uint8_t val)
 * r0 = valor de celda (entregado en r0)
 * return (celda & 0x000F)
 */
celda_eliminar_candidato:
    PUSH    {lr}
    CMP     r0, #0
    BEQ     cel_done_small
    CMP     r1, #1
    BLO     cel_done_small
    CMP     r1, #9
    BHI     cel_done_small

    LDRH    r2, [r0]           @ r2 = *celda
    SUB     r3, r1, #1         @ r3 = valor - 1
    ADD     r3, r3, #7         @ r3 = BIT_CANDIDATOS + (valor-1)
    MOV     r4, #1
    LSL     r4, r4, r3
    ORR     r2, r2, r4
    STRH    r2, [r0]

cel_done_small:
    POP     {pc}
