/* *************************************************************************************************
 * file:        clock.c
 *
 *          The clock module.
 *
 * author:      Armin
 * date:        28.07.2014
 * version:     0.4   worky, but not ready
 *
 * file history:
 *          28.07.2014  Armin   file created
 *          26.09.2014  Mr.L.   moved and renamed data structures, nicified
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
void clock_displayTime(const clockType time);


/* ************************************ E O F *************************************************** */
#endif /* _CLOCK_H_ */
