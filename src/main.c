/* Copyright 2022, Laboratorio de Microprocesadores
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * http://www.microprocesadores.unt.edu.ar/
 * Copyright 2022, Facundo Quiroga <facundoqga@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** \brief Simple sample of use LPC HAL gpio functions
 **
 ** \addtogroup samples Sample projects
 ** \brief Sample projects to use as a starting point
 ** @{ */

/* === Headers files inclusions =============================================================== */


#include <stdbool.h>
#include "bsp.h"
#include "clock.h"
#include "pantalla.h"

/* === Macros definitions ====================================================================== */

/* === Private data type declarations ========================================================== */
typedef enum {
    HORA_SIN_AJUSTAR,
    MOSTRANDO_HORA,
    AJUSTANDO_MINUTOS_ACTUAL,
    AJUSTANDO_HORAS_ACTUAL,
    AJUSTANDO_MINUTOS_ALARMA,
    AJUSTANDO_HORAS_ALARMA
} modo_t;

/* === Private variable declarations =========================================================== */
static board_t board;
static modo_t modo;
static clock_t reloj;

static const uint8_t LIMITE_MINUTOS[] = {6,0};

static const uint8_t LIMITE_HORAS[] = {2,4};
/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

void CambiarModo(modo_t valor) {
    modo = valor;

    switch (modo){
    case HORA_SIN_AJUSTAR:
        DisplayFlashDigits(board->display, 0, 3, 250);
        break;
    case MOSTRANDO_HORA:
        DisplayFlashDigits(board->display, 0, 0, 0);
        break;
    case AJUSTANDO_MINUTOS_ACTUAL:
        DisplayFlashDigits(board->display, 2, 3, 250);
        break;
    case AJUSTANDO_HORAS_ACTUAL:
        DisplayFlashDigits(board->display, 0, 1, 250);
        break;
    case AJUSTANDO_MINUTOS_ALARMA:
        DisplayFlashDigits(board->display, 2, 3, 250);
        break;
    case AJUSTANDO_HORAS_ALARMA:
        DisplayFlashDigits(board->display, 0, 1, 250);
        break;
    
    default:
        break;
    }
}


void AlarmaActivada(clock_t clock, bool state){

}

void IncrementarBCD(uint8_t numero[2], const uint8_t limite[2]){
    numero[1]++;
    if(numero[1] > 9){
        numero[1] = 0;
        numero[0]++;
    }
    if((numero[0] == limite[0]) && (numero[1] == limite[1])){
        numero[0] = 0;
        numero[1] = 0;
    }
}

void DecrementarBCD(uint8_t numero[2], const uint8_t limite[2]){
    numero[1]--;
    if(numero[1] > 9){
        numero[1] = 0;
        numero[0]--;
    }
    if((numero[0] == limite[0]) && (numero[1] == limite[1])){
        numero[0] = 0;
        numero[1] = 0;
    }
}
/* === Public function implementation ========================================================= */

int main(void) {

    uint8_t entrada[4];
    board = BoardCreate();
    reloj = ClockCreate(10, AlarmaActivada);

    SisTick_Init(1000);
    CambiarModo(HORA_SIN_AJUSTAR);

    DisplayFlashDigits(board->display, 0, 1, 250);
    DisplayToggleDots(board->display,1 ,2);

    while (true) {

        if (DigitalInputHasActivated(board->accept))
        {
            if(modo == MOSTRANDO_HORA)
            {
                if(!ClockGetAlarm(reloj, entrada, sizeof(entrada))){
                    ClockToggleAlarm(reloj);
                }
            }else if(modo == AJUSTANDO_MINUTOS_ACTUAL)
            {
                CambiarModo(AJUSTANDO_HORAS_ACTUAL);
            }else if(modo == AJUSTANDO_HORAS_ACTUAL)
            {
                ClockSetupTime(reloj, entrada, sizeof(entrada));
                CambiarModo(MOSTRANDO_HORA);
            }else if(modo == AJUSTANDO_MINUTOS_ALARMA)
            {
                CambiarModo(AJUSTANDO_HORAS_ALARMA);
            }else if(modo == AJUSTANDO_HORAS_ALARMA)
            {
                ClockSetupAlarm(reloj, entrada, sizeof(entrada));
                CambiarModo(MOSTRANDO_HORA);
            }

        }
        if (DigitalInputHasActivated(board->cancel))
        {
            if(modo == MOSTRANDO_HORA)
            {
                if(ClockGetAlarm(reloj, entrada, sizeof(entrada))){
                    ClockToggleAlarm(reloj);
                }
            }else
            {
                if(ClockGetTime(reloj, entrada, sizeof(entrada))){
                    CambiarModo(MOSTRANDO_HORA);
                }else{
                    CambiarModo(HORA_SIN_AJUSTAR);
                }
            }
        }
        if (DigitalInputHasActivated(board->set_time))
        {
            CambiarModo(AJUSTANDO_MINUTOS_ACTUAL);
            ClockGetTime(reloj, entrada, sizeof(entrada));
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
        }
        if (DigitalInputHasActivated(board->set_alarm))
        {
            
        }
        if (DigitalInputHasActivated(board->decrement))
        {
            if((modo == AJUSTANDO_MINUTOS_ACTUAL) || (modo == AJUSTANDO_MINUTOS_ALARMA))
            {
                DecrementarBCD(&entrada[2], LIMITE_MINUTOS);
                DisplayWriteBCD(board->display, entrada, sizeof(entrada));
            }else if ((modo == AJUSTANDO_HORAS_ACTUAL) || (modo == AJUSTANDO_HORAS_ALARMA))
            {
                DecrementarBCD(&entrada[2], LIMITE_HORAS);
                DisplayWriteBCD(board->display, entrada, sizeof(entrada));
            }
            
        }
        if (DigitalInputHasActivated(board->increment))
        {
            if((modo == AJUSTANDO_MINUTOS_ACTUAL) || (modo == AJUSTANDO_MINUTOS_ALARMA))
            {
                IncrementarBCD(&entrada[2], LIMITE_MINUTOS);
                DisplayWriteBCD(board->display, entrada, sizeof(entrada));
                
            }else if ((modo == AJUSTANDO_HORAS_ACTUAL) || (modo == AJUSTANDO_HORAS_ALARMA))
            {
                IncrementarBCD(entrada, LIMITE_HORAS);
                DisplayWriteBCD(board->display, entrada, sizeof(entrada));
            }
            
        }
        
        
        for (int delay = 0; delay < 2500; delay++) {
            __asm("NOP");
        }
    }
}

void SysTick_Handler(void)
{
    uint8_t hora[4];
    DisplayRefresh(board->display);
    ClockNewTick(reloj);

    if (modo <= MOSTRANDO_HORA)
    {
    ClockGetTime(reloj, hora, sizeof(hora));
    DisplayWriteBCD(board->display, hora, sizeof(hora));
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
