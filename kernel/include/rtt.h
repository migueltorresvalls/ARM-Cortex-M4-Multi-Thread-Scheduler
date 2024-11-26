/** @file   rtt.h
 *
 *  @brief  constants and prototypes for real-time transfer (RTT)
 *  @note   adapted from SEGGER implementation
 *  @note   not for public release, do not share
 *
 *  @date   Last modified 5 October 2023
 *  @author CMU 14-642
 */

#ifndef _RTT_H_
#define _RTT_H_

#include <unistd.h>

#define RTT_MAX_UP_BUFFERS    1
#define RTT_MAX_DOWN_BUFFERS  1
#define BUFFER_SIZE_UP        1024
#define BUFFER_SIZE_DOWN      16

/** @brief ring buffer for device->host "up" buffer */
typedef struct {
    const char* name;          // name, e.g., "Terminal"
    char* p_buffer;            // start of buffer
    uint32_t buffer_size;      // buffer size (bytes), not including spacer between end/start
    uint32_t pos_wr;           // position of next write
    volatile uint32_t pos_rd;  // position of next read by host (volatile because host modifies)
    uint32_t flags;            // config flags
} rtt_buffer_up_t;

/** @brief ring buffer for host->device "down" buffer */
typedef struct {
    const char* name;          // name, e.g., "Terminal"
    char* p_buffer;            // start of buffer
    uint32_t buffer_size;      // buffer size (bytes), not including spacer between end/start
    volatile uint32_t pos_wr;  // position of next write by host (volatile because host modifies)
    uint32_t pos_rd;           // position of next read by target
    uint32_t flags;            // config flags
} rtt_buffer_down_t;

/** @brief RTT control block with config details */
typedef struct {
    char id[16];                              		// identifies control block
    uint32_t num_up_buffers;           			// initialized to RTT_MAX_UP_BUFFERS
    uint32_t num_down_buffers;         			// initialized to RTT_MAX_DOWN_BUFFERS
    rtt_buffer_up_t up_buffers[RTT_MAX_UP_BUFFERS];	// buffers to send info "up" : device -> host
    rtt_buffer_down_t down_buffers[RTT_MAX_DOWN_BUFFERS]; // buffers to send info "down" : host -> device
} rtt_control_t;

/** @brief initialize control block before using RTT (do not change) */
void rtt_init();

/** @brief stores given number of bytes into ring buffer and updates write pointer */
uint32_t rtt_write(uint32_t buffer_index, const void* p_buffer, uint32_t num_bytes);

/** @brief reads from RTT down buffer, returns number of bytes read */
uint32_t rtt_read(uint32_t buffer_index, void* p_buffer, uint32_t num_bytes);

/** @brief counts number of bytes available in RTT down buffer (0 if empty) */
uint32_t rtt_has_data(uint32_t buffer_index);

#endif /* _RTT_H_ */

