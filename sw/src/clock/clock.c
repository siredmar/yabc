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

/* ------------------------------------ INCLUDES ------------------------------------------------ */
#include "clock.h"

/* ------------------------------------ DEFINES ------------------------------------------------- */
#define CLOCK_HOUR_PORT GPIO_PORTA
#define CLOCK_MINUTE_PORT GPIO_PORTB
#define CLOCK_PWM_PERIOD (100U)
#define CLOCK_PWM_DUTY (20U)
#define CLOCK_PWM_DUTY_INC (20U)

/* ------------------------------------ TYPE DEFINITIONS ---------------------------------------- */
static uint16 pwmPeriod = CLOCK_PWM_PERIOD;
static uint16 pwmDuty = CLOCK_PWM_DUTY;

typedef enum
{
   setMinute = 0,
   setHour,
   setPWM
}clock_menuStateType;

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
static uint8 timerStoppedCounter;   // prevention of accidently restarting timer2
static clock_menuStateType clock_menuState = setMinute;
/* ------------------------------------ PROTOTYPES ---------------------------------------------- */
static void clock_delay(uint16 ms);
static void updateDateAndTime(void);
static void startBlinkTimer(void);
static void stopBlinkTimer(void);

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
   startBlinkTimer();
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
   //   uint8 timeChanged = 0;
   if (dateAndTime.second > 59)
   {
      dateAndTime.second = 0;
      dateAndTime.minute++;
      //      timeChanged = 1;
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

}

/* ---------------------------------------------------------------------------------------------- *\
 * T.B.F.
\* ---------------------------------------------------------------------------------------------- */
void startBlinkTimer(void)
{
   /* Timer 1 for blinking */
   // 0xF42; // preload with 0x04E2 = 125 = 0.01 s bei 8 MHz, 1 Clockdiv
   TCCR1B = 0x01; //CPU-Takt/1
   TCCR1A = 0x00; //Register zuruecksetzen
   TCCR1B |= (1<<WGM12); //Zuruecksetzen des Counters aktivieren

   OCR1A = 125;
   TIMSK1 |= (1<<OCIE1A); //Interrupt an Kanal A aktivieren
}

/* ---------------------------------------------------------------------------------------------- *\
 * T.B.F.
\* ---------------------------------------------------------------------------------------------- */
void stopBlinkTimer(void)
{
   TIMSK1 = 0;             /* switch off all timer0 interrupts */
   TCCR1B = 0;             /* switch timer off */
   TCCR1A = 0x00;          /* reset timer mode */
   TCNT1 = 0;              /* reset counter register */
}


/* -------------------------------------------------------------------------------------------- *\
 * T.B.F.
\* ---------------------------------------------------------------------------------------------- */
ISR(TIMER2_COMPA_vect)
{
   dateAndTime.second++;
   updateDateAndTime();

   while(ASSR & ((1<<TCN2UB) | (1<<OCR2AUB) | (1<<OCR2BUB) |
         (1<<TCR2AUB) | (1<<TCR2BUB)));

}

ISR(TIMER1_COMPA_vect)
{
   static uint16 timerCount = 0;
   timerCount++;
   if(timerCount >= pwmPeriod)
   {
      timerCount = 0;
   }
   else
   {
      if(timerCount < pwmDuty)
      {
         gpio_WritePort(CLOCK_HOUR_PORT, dateAndTime.hour);
         gpio_WritePort(CLOCK_MINUTE_PORT, dateAndTime.minute);
      }
      else
      {
         gpio_WritePort(CLOCK_HOUR_PORT, 0x00);
         gpio_WritePort(CLOCK_MINUTE_PORT, 0x00);
      }
   }
}

ISR(INT0_vect)
{
   switch(clock_menuState)
   {
   case setMinute:

      clock_menuState = setHour;
      break;

   case setHour:

      clock_menuState = setPWM;
      break;

   case setPWM:

      clock_menuState = setMinute;
      break;

   default:

      break;

   }
   _delay_ms(333);
}

ISR(INT1_vect)
{
   clock_halt();

   switch(clock_menuState)
   {
   case setMinute:
      gpio_WritePort(CLOCK_MINUTE_PORT, dateAndTime.minute);
      gpio_WritePort(CLOCK_HOUR_PORT, dateAndTime.hour);
      dateAndTime.minute++;
      updateDateAndTime();
      break;

   case setHour:
      gpio_WritePort(CLOCK_MINUTE_PORT, dateAndTime.minute);
      gpio_WritePort(CLOCK_HOUR_PORT, dateAndTime.hour);
      dateAndTime.hour++;
      updateDateAndTime();
      break;

   case setPWM:

      pwmDuty = pwmDuty + CLOCK_PWM_DUTY_INC;

      if(pwmDuty > CLOCK_PWM_PERIOD)
         pwmDuty = CLOCK_PWM_DUTY;


      break;

   default:

      break;

   }
   clock_restart();
   _delay_ms(333);
}
/* ************************************ E O F *************************************************** */
