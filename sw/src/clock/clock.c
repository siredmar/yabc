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
 *          26.09.2014  Mr.L.   changed way of calculating date, edit timer reoutines, nicified,
 *                                  moved and renamed data structures, added CONSTs, ...
 *
 * notes:
 *          - none -
 *
 * copyright:   http://creativecommons.org/licenses/by-nc-sa/3.0/
 **************************************************************************************************/

/* ------------------------------------ INCLUDES ------------------------------------------------ */
#include "clock.h"



/* ------------------------------------ DEFINES ------------------------------------------------- */
#define CLOCK_HOUR_PORT GPIO_PORTA
#define CLOCK_MINUTE_PORT GPIO_PORTB
/* ------------------------------------ TYPE DEFINITIONS ---------------------------------------- */

enum
{
   ZERO,
   ONE,
   TWO,
   THREE,
   FOUR,
   FIVE,
   SIX,
   SEVEN,
   EIGHT,
   NINE,
   DOT
};


/* ------------------------------------ GLOBAL VARIABLES ---------------------------------------- */


/* ------------------------------------ PRIVATE VARIABLES --------------------------------------- */
static clockType dateAndTime;

//!!! <--- moved from .h

static uint8 timerStoppedCounter;   // prevention of accidently restarting timer2



/* ------------------------------------ PROTOTYPES ---------------------------------------------- */
static void clock_delay(uint16 ms);
static void updateDateAndTime(void);


/* ------------------------------------ GLOBAL FUNCTIONS ---------------------------------------- */

/* ---------------------------------------------------------------------------------------------- *\
 * T.B.F.
\* ---------------------------------------------------------------------------------------------- */
void clock_init(void)
{
   dateAndTime.second = 0;                    /* set initial time and date  */
   dateAndTime.minute = 0;
   dateAndTime.hour   = 0;


   /* --- timer init ---  */

   TIMSK2 = 0;                             // disable all timer2 interrupts

   ASSR   |= (1 << AS2);                   // enable asynchronous mode
   clock_delay(1000);                      // wait for oscillator swinging safely

   TCNT2   = 0;                            // set counter register
   OCR2A   = 128 - 1;                      // 32768 / 256 / 1 = 128, interval = 1s
   //    OCR2A   = 256 - 1;                      // adjust interval for testing, #rat
   TCCR2A  = (1 << WGM21);                 // set CTC mode, TOP value = OCRA
   TCCR2B |= (1 << CS22) | (1 << CS21);    // set prescaler = 256 and start timer

   while (ASSR & ((1 << OCR2AUB)|(1 << TCR2AUB)|(1 << TCR2AUB)));  // wait for registers to update

   TIFR2   = 0;                            //clear pending interrupts
   TIMSK2 |= (1<<OCIE2A);                  // Enable Compare Interrupt

   EIMSK = 0x03;

   clock_displayTime(dateAndTime);
}

/* ---------------------------------------------------------------------------------------------- *\
 * T.B.F.
\* ---------------------------------------------------------------------------------------------- */
void clock_halt(void)
{
   if (timerStoppedCounter == 0)
   {
      TCCR2B = 0;                     // stop timer
      while (ASSR & (1 << TCR2AUB));  // wait for register to update
   }

   timerStoppedCounter++;
}

/* ---------------------------------------------------------------------------------------------- *\
 * T.B.F.
\* ---------------------------------------------------------------------------------------------- */
void clock_restart(void)
{
   if (timerStoppedCounter == 1)
   {
      TCCR2B |= (1 << CS22) | (1 << CS21);    // set prescaler = 256 and start timer
      while (ASSR & (1 << TCR2AUB));          // wait for register to update
   }

   if (timerStoppedCounter > 0)
   {
      timerStoppedCounter--;
   }

}


/* ---------------------------------------------------------------------------------------------- *\
 * T.B.F.
\* ---------------------------------------------------------------------------------------------- */
void clock_displayTime(const clockType time)
{
   /* TODO yabc: replace second with hour */
      gpio_WritePort(CLOCK_HOUR_PORT, dateAndTime.hour);
      gpio_WritePort(CLOCK_MINUTE_PORT, dateAndTime.minute);
}

/* ------------------------------------ PRIVATE FUNCTIONS --------------------------------------- */

/* ---------------------------------------------------------------------------------------------- *\
 * T.B.F.
\* ---------------------------------------------------------------------------------------------- */
static void clock_delay(uint16 ms)
{
   for (; ms > 0; ms--)
   {
      _delay_ms(1);
   }
}

/* ---------------------------------------------------------------------------------------------- *\
 * T.B.F.
\* ---------------------------------------------------------------------------------------------- */
static void updateDateAndTime(void)
{
   //   boolean timeChanged = FALSE;
   //
   //   timeChanged = TRUE;                         //!!! #rat

   if (dateAndTime.second > 59)
   {
      dateAndTime.second = 0;
      dateAndTime.minute++;
      //      timeChanged = TRUE;
   }
   if (dateAndTime.minute > 59)
   {
      dateAndTime.minute = 0;
      dateAndTime.hour++;
   }
   if (dateAndTime.hour > 23)
   {
      dateAndTime.hour = 0;
   }
   //   if (timeChanged == TRUE)
   //   {
   clock_displayTime(dateAndTime);
   //   }
}

/* ---------------------------------------------------------------------------------------------- *\
 * T.B.F.
\* ---------------------------------------------------------------------------------------------- */
ISR(TIMER2_COMPA_vect)
{
   //   static cnt = 0;
   dateAndTime.second++;
   //   gpio_WritePort(GPIO_PORTA, cnt++);
   updateDateAndTime();

   while(ASSR & ((1<<TCN2UB) | (1<<OCR2AUB) | (1<<OCR2BUB) |
         (1<<TCR2AUB) | (1<<TCR2BUB)));

}

ISR(INT0_vect)
{
   dateAndTime.minute++;
   updateDateAndTime();
   _delay_ms(333);
}

ISR(INT1_vect)
{
   dateAndTime.hour++;
   updateDateAndTime();
   _delay_ms(333);
}
/* ************************************ E O F *************************************************** */
