/** @file   syscall.c
 *  @note   put a better file header here
**/

#include <syscall.h>
#include <unistd.h>
#include <gpio.h>
#include <rtt.h>
#include <printk.h>
#include <arm.h>
#include <timer.h>

/** linker label pointing to start of heap*/
extern uint32_t __heap_base;
/** linker label pointing to max value heap can take*/
extern uint32_t __heap_limit;

/** pointer to current heap*/
uint32_t current_heap = (uint32_t)&__heap_base;

/** @brief will return the minimum value out of the two parameters passed
 * @param a used to compute the minimum
 * @param b used to compute the minimum
 * @return minimum value of a and b*/
uint32_t min(uint32_t a, uint32_t b) {
    return a > b? b : a;
}

/** @brief breakfunction used to change location of the program break
 * @param incr indicated how big the program break's increment must be*/
void* sys_sbrk(int incr) {    
    uint32_t previous_heap = current_heap;
    if (current_heap + incr < (uint32_t)&__heap_limit){
        current_heap += incr;
        return (void *)previous_heap;
    } else {
        return (void *)-1;
    }
}

/** @brief used to read data from a file descriptor
 * @param file file descriptor. During this lab we will accept 1
 * @param ptr pointer the buffer where the read info will be stored
 * @param len number of bytes to read
 * @return number of bytes read*/
int sys_read(int file, char* ptr, int len) {
    uint32_t to_read = rtt_has_data(0);
    int return_value;
    
    if (file == 1){
        if (to_read > 0) {
            return_value = rtt_read(0, ptr, min(len, to_read));
        } else {
            return_value = 0;
        }
    } else {
        return_value = -1; 
    }
    return return_value;
}

/** @brief used to write data to a file descriptor
 * @param file file descriptor. During this lab we will only use 1
 * @param ptr pointer to buffer that stores data to be written
 * @param len total size of data to write
 * @return number of bytes written*/
int sys_write(int file, char* ptr, int len) {
    int return_value;
    if (file == 1){
        return_value = rtt_write(0, ptr, len);
    } else {
        return_value = -1; 
    }
    return return_value;
}

/** @brief used to terminate a process
 * @param status to terminate the process with*/
void sys_exit(int status) {
    printk("\nExit code=%d", status);
    systick_stop();
    if (status){
        gpio_set(RED_LED_PORT, RED_LED_PIN);
    } else {
        gpio_set(BLUE_LED_PORT, BLUE_LED_PIN);
    }
    BUSY_LOOP(1);
    return;
}