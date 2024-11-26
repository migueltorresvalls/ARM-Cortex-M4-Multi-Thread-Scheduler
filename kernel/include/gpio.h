/** @file   gpio.h
 *  @brief  Prototypes for GPIO configuration, set, clear, and read
 *  @date   last modified 11 Sep 2023
 *  @author CMU 14-642
 *  @note   Not for public release, do not share
 **/
#ifndef _GPIO_H_
#define _GPIO_H_

#include <unistd.h>

/** Port for Red LED */
#define RED_LED_PORT 1
/** Port for Red LED */
#define RED_LED_PIN 15

/** Port for Blue LED */
#define BLUE_LED_PORT 1
/** Port for Blue LED */
#define BLUE_LED_PIN 10

/** GPIO mode for port/pin as input */
#define MODE_INPUT (0)
/** GPIO mode for port/pin as output */
#define MODE_OUTPUT (1)

/** Position to shift the drive value in PIN_CNF register */
#define DRIVE_POS 8

/** Position to shift the pull-up/down value in PIN_CNF register */
#define PUPD_POS 2

/** GPIO drive configuration -- standard 0, standard 1 */
#define DRIVE_S0S1 (0)
/** GPIO drive configuration -- high drive 0, standard 1 */
#define DRIVE_H0S1 (1)
/** GPIO drive configuration -- standard 0, high drive 1 */
#define DRIVE_S0H1 (2)
/** GPIO drive configuration -- high drive 0, high drive 1 */
#define DRIVE_H0H1 (3)
/** GPIO drive configuration -- disconnected 0, standard 1 */
#define DRIVE_D0S1 (4)
/** GPIO drive configuration -- disconnected 0, high drive 1 */
#define DRIVE_D0H1 (5)
/** GPIO drive configuration -- standard 0, disconnected 1 */
#define DRIVE_S0D1 (6)
/** GPIO drive configuration -- high drive 0, disconnected 1 */
#define DRIVE_H0D1 (7)

/** GPIO no pull-up or pull-down resistor */
#define PUPD_NONE (0)
/** GPIO pull-down resistor */
#define PUPD_PULL_DOWN (1)
/** GPIO pull-up resistor */
#define PUPD_PULL_UP (3)

/** @brief struct used to map all gpio registers */
typedef struct gpio {
    /** Padding */
    char a[0x504];
    /** GPIO port for writing */
    volatile uint32_t out;
    /** Used to set individual bits in GPIO port */
    volatile uint32_t outset;
    /** Padding */
    char b[0x4];
    /** GPIO port for reading */
    volatile uint32_t in;
    /** Used to define direction of GPIO pins */
    volatile uint32_t dir;
    /** Padding */
    char c[0x1E8]; // 0x700 - 0x518
    /** Array to define pin configuration */
    volatile uint32_t pin_cnf[32];
} gpio_t;

/** @brief GPIO initialization function
 * @param port   0 or 1
 * @param pin    0 to 31 for port 0, 0 to 15 for port 1
 * @param mode   GPIO port mode (input or output)
 * @param drive  GPIO drive config (standard, high, disconnected)
 * @param pupd   GPIO pull up, down, or neither
 */
void gpio_init(uint8_t port, uint8_t pin, uint8_t mode, uint8_t drive, uint8_t pupd);

/** @brief Set specified GPIO port/pin to high */
void gpio_set(uint8_t port, uint8_t pin);

/** @brief Clear specified GPIO port/pin to low */
void gpio_clr(uint8_t port, uint8_t pin);

/** @brief Read all the gpio IN pins from selected port as 32-bit word */
uint32_t gpio_read_all(uint8_t port);

/** @brief Return only a single logic value for the given port/pin */
uint8_t gpio_read(uint8_t port, uint8_t pin);

#endif /* _GPIO_H_ */
