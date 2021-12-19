#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"

static const int64_t delay_us = 43200000000;  // 12 hours.

// Returns true if BOOTSEL button is currently pressed. Taken from 
// https://github.com/raspberrypi/pico-examples/blob/master/picoboard/button/button.c.
bool __no_inline_not_in_flash_func(get_bootsel_button)() {
    const uint CS_PIN_INDEX = 1;

    // Must disable interrupts, as interrupt handlers may be in flash, and we
    // are about to temporarily disable flash access!
    uint32_t flags = save_and_disable_interrupts();

    // Set chip select to Hi-Z
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    // Note we can't call into any sleep functions in flash right now
    for (volatile int i = 0; i < 1000; ++i);

    // The HI GPIO registers in SIO can observe and control the 6 QSPI pins.
    // Note the button pulls the pin *low* when pressed.
    bool button_state = !(sio_hw->gpio_hi_in & (1u << CS_PIN_INDEX));

    // Need to restore the state of chip select, else we are going to have a
    // bad time when we return to code in flash!
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    restore_interrupts(flags);

    return button_state;
}

int main() {
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    // At boot, turn the light on.
    volatile bool light_state = true;
    gpio_put(PICO_DEFAULT_LED_PIN, light_state);    
    absolute_time_t last_on_time = get_absolute_time();
    
    while (true) {
        if (light_state) {
            while(true) {
                if (get_bootsel_button() ^ PICO_DEFAULT_LED_PIN_INVERTED) {
                    light_state = false;
                    gpio_put(PICO_DEFAULT_LED_PIN, light_state);
                    break;
                }
            }
        } else {
            // If light has been off for delay_us, turn it on.
            if (absolute_time_diff_us(last_on_time, get_absolute_time()) >= delay_us) {
                light_state = true;
                gpio_put(PICO_DEFAULT_LED_PIN, light_state);
                last_on_time = get_absolute_time();
            }
        }
        sleep_ms(1000);
    }
}
