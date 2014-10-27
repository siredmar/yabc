#include "std_types.h"

#include "gpio/gpio.h"
#include "clock/clock.h"


int main(void)
{
   cli();
   gpio_init();
   clock_init();
   sei();
//   gpio_WritePort(GPIO_PORTA, 0b1001001);
//   gpio_WritePort(GPIO_PORTB, 0b01010101);
   while(1)
   {
   }

   return 0;
}

