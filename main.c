#include "pico/stdlib.h"
#include "hardware/gpio.h"


int main(void)
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, true);
    
    for(;;){
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(500);
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(500);
    }

    return 0;
}
