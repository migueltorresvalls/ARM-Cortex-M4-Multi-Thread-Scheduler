/** @file   printk.h
 *
 *  @brief  formatted string printing at kernel level, wraps RTT
 *  @note   Not for public release, do not share
 *
 *  @date   last modified 5 October 2023
 *  @author CMU 14-642
**/

#ifndef _PRINTK_H_
#define _PRINTK_H_

#include <rtt.h>
#include <stdarg.h>

#define RTT_PRINTK_BUFFER_SIZE	64

#define FORMAT_LEFT_JUSTIFY   (1u << 0)
#define FORMAT_PAD_ZERO       (1u << 1)
#define FORMAT_PRINT_SIGN     (1u << 2)
#define FORMAT_ALTERNATE      (1u << 3)

/** @brief data structure describing printk use with RTT */
typedef struct {
  char* p_buffer;
  uint32_t buffer_size;
  uint32_t count;
  int return_value;
  uint32_t buffer_index;
} rtt_printk_desc_t;

/** @brief wraps rtt_vprintk with variadic args */
int rtt_printk(uint32_t buffer_index, const char * s_fmt, ...);

/** @brief called by rtt_printk using variadic args, writes formatted string to RTT up buffer */
int rtt_vprintk(uint32_t buffer_index, const char * s_fmt, va_list *p_params);

/** @brief simple wrapper for rtt_vprintk bound to terminal 0 */
int printk(const char * s_fmt, ...);

#endif /* _PRINTK_H_ */
