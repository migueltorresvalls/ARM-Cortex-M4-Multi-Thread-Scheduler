/** @file   gpio.c
 *  @note   replace this with your gpio.c from lab 2 / 3
**/

#include <gpio.h>

void gpio_init(uint8_t port, uint8_t pin, uint8_t mode, uint8_t drive, uint8_t pupd) {
    (void)port; (void)pin; (void)mode; (void)drive; (void)pupd;
}

void gpio_set(uint8_t port, uint8_t pin) {
    (void)port; (void)pin;
}

void gpio_clr(uint8_t port, uint8_t pin) {
    (void)port; (void)pin;
}

uint32_t gpio_read_all(uint8_t port) {
    (void)port;
    return 0UL;
}

uint8_t gpio_read(uint8_t port, uint8_t pin) {
    (void)port; (void)pin;
    return 0;
}
