#include "std_types.h"

#include "gpio/gpio.h"
#include "clock/clock.h"


int main(void)
{
   cli();
   gpio_init();
   clock_init();
   sei();
   while(1)
   {
   }

   return 0;
}

