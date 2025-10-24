/*
 * main.c
 */

#include <zephyr/kernel.h>
#include "BTN.h"
#include "LED.h"

#define SLEEP_MS 1

int main(void) {

    if (0 > BTN_init()) {
        return 0;
    }
    if (0 > LED_init()) {
        return 0;
    }

    int counter = 0;          // internal 4-bit counter
    int prev_state[4] = {0};  // track LED states (0 = off, 1 = on)

    while (1) {
        if (BTN_check_clear_pressed(BTN0)) {

            // increment counter and wrap around at 16 (4-bit)
            counter = (counter + 1) % 16;

            // update LEDs to match counter bits
            for (int i = 0; i < 4; i++) {
                int bit = (counter >> i) & 1;

                // if the LED's desired state differs from its previous one, toggle it
                if (bit != prev_state[i]) {
                    LED_toggle(i);
                    prev_state[i] = bit;
                }
            }

            printk("Counter = %d\n", counter);
        }

        k_msleep(SLEEP_MS);
    }


    return 0;
}