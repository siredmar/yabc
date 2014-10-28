/* *************************************************************************************************
 * file:        clock.c
 *
 *          The clock module.
 *
 * author:      Armin Schlegel, Mr. L.
 * date:        28.10.2014
 * version:     0.1   worky
 *
 * file history:
 *          28.10.2014  Armin   forked from https://github.com/Mister-L/yalc.git
 *                              edited to work with binary clock https://github.com/siredmar/yabc.git
 *
 * notes:
 *          - none -
 *
 * copyright:   http://creativecommons.org/licenses/by-nc-sa/3.0/
 **************************************************************************************************/
#ifndef _CLOCK_H_
#define _CLOCK_H_
/* ============================================================================================== */


/* ------------------------------------ INCLUDES ------------------------------------------------ */
#include <avr/io.h>
#include "std_types.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../gpio/gpio.h"


/* ------------------------------------ DEFINES ------------------------------------------------- */


/* ------------------------------------ TYPE DEFINITIONS ---------------------------------------- */
typedef struct
{
   uint8 second;
   uint8 minute;
   uint8 hour;
}clockType;

typedef enum
{
    CT_SECOND = 0U,
    CT_MINUTE,
    CT_HOUR,
    CT_NONE
}clockType_e;

/* ------------------------------------ GLOBAL VARIABLES ---------------------------------------- */
/* ------------------------------------ PROTOTYPES ---------------------------------------------- */

void clock_init(void);
void clock_halt(void);
void clock_restart(void);
/* ************************************ E O F *************************************************** */
#endif /* _CLOCK_H_ */
