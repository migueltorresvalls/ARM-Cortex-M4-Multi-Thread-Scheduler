/** @file   rtt.c
 *  @note   put a better file header here
**/

#include <arm.h>
#include <rtt.h>
#include <printk.h>

/** linker label pointing to start of RTT control block */
extern rtt_control_t __rtt_start;
/** up buffers pointed to by the RTT control block */
static char up_buffer[BUFFER_SIZE_UP];
/** down buffers pointed to by the RTT control block */
static char down_buffer[BUFFER_SIZE_DOWN];

/** @name   rtt_init
 *  @brief  Initializes the control block
 *  @note   Must be called before any other rtt functions
**/
void rtt_init() {
    rtt_control_t* p;
    p = &__rtt_start;
    p->num_up_buffers = RTT_MAX_UP_BUFFERS;
    p->num_down_buffers = RTT_MAX_DOWN_BUFFERS;

    p->up_buffers[0].name = "Terminal";
    p->up_buffers[0].p_buffer = up_buffer;
    p->up_buffers[0].buffer_size = BUFFER_SIZE_UP;
    p->up_buffers[0].pos_rd = 0;
    p->up_buffers[0].pos_wr = 0;
    p->up_buffers[0].flags = 2;

    p->down_buffers[0].name = "Terminal";
    p->down_buffers[0].p_buffer = down_buffer;
    p->down_buffers[0].buffer_size = BUFFER_SIZE_DOWN;
    p->down_buffers[0].pos_rd = 0;
    p->down_buffers[0].pos_wr = 0;
    p->down_buffers[0].flags = 2;

    p->id[7] = 'R'; p->id[8] = 'T'; p->id[9] = 'T';
    data_mem_barrier();
    p->id[0] = 'S'; p->id[1] = 'E'; p->id[2] = 'G'; p->id[3] = 'G'; p->id[4] = 'E'; p->id[5] = 'R';
    data_mem_barrier();
    p->id[6] = ' ';
    data_mem_barrier();
    p->id[10] = '\0'; p->id[11] = '\0'; p->id[12] = '\0'; p->id[13] = '\0'; p->id[14] = '\0'; p->id[15] = '\0';
    data_mem_barrier();
}

/** @brief	Writes bytes into RTT up buffer (device -> host)
 *  - Updates the RTT up buffer with the given content
 *  - Ensures the host can determine there is new content
 *  @param buffer_index index of the buffer to write to in the list of buffers
 *  @param p_buffer the buffer to write from
 *  @param num_bytes the number of bytes in the given "from" buffer
 *  @return Number of bytes written
**/
uint32_t rtt_write(uint32_t buffer_index, const void* p_buffer, uint32_t num_bytes) {
    rtt_control_t* p;
    p = &__rtt_start;

    char *buff_start = p->up_buffers[buffer_index].p_buffer; 
    uint32_t write_pos = p->up_buffers[buffer_index].pos_wr;
    uint32_t read_pos = p->up_buffers[buffer_index].pos_rd;
    uint32_t buffer_size = p->up_buffers[buffer_index].buffer_size;
    char* tx_string = (char*)p_buffer; 

    uint32_t written = 0; 

    while (written < num_bytes){
        while ((write_pos + 1)%buffer_size == read_pos) {
            read_pos = p->up_buffers[buffer_index].pos_rd;
        }

        buff_start[write_pos++] = tx_string[written++];
        write_pos %= buffer_size;

        p->up_buffers[buffer_index].pos_wr = write_pos;
    }

    return written;
}

/** @brief Reads bytes from RTT down buffer (host -> device)
 * - Copies content from the RTT down buffer to a given buffer
 * - ensure the host knows which content has been read successfully
 *  @param buffer_index index of the buffer to read from in the list of buffers
 *  @param p_buffer the buffer to write to
 *  @param buffer_size the length of the buffer that should be read
 * @return Number of bytes read
**/
uint32_t rtt_read(uint32_t buffer_index, void* p_buffer, uint32_t buffer_size) {
    rtt_control_t* p;
    p = &__rtt_start;

    char *buff_start = p->down_buffers[buffer_index].p_buffer;
    uint32_t read_pos = p->down_buffers[buffer_index].pos_rd;
    uint32_t write_pos = p->down_buffers[buffer_index].pos_wr;
    uint32_t down_buffer_size = p->down_buffers[buffer_index].buffer_size;
    char *tx_string = (char *)p_buffer;

    uint32_t read = 0;

    while (read < buffer_size) {

        while (read_pos == write_pos) {
            write_pos = p->down_buffers[buffer_index].pos_wr;
        }

        tx_string[read++] = buff_start[read_pos++];
        read_pos %= down_buffer_size;
        p->down_buffers[buffer_index].pos_rd = read_pos;
    }

    return read;
}

/** @brief	Checks if there are available bytes to read from in the RTT down buffer (host -> device)
 * It counts how much data can be read but doesn't read or change anything.
 *  @param buffer_index index of the buffer to read from in the list of buffers
 *  @return Number of bytes that can be read without waiting
**/
uint32_t rtt_has_data(uint32_t buffer_index) {
    rtt_control_t* p;
    p = &__rtt_start;

    uint32_t read_pos = p->down_buffers[buffer_index].pos_rd;
    uint32_t write_pos = p->down_buffers[buffer_index].pos_wr;
    uint32_t buffer_size = p->down_buffers[buffer_index].buffer_size;

    if (write_pos > read_pos)
        return (write_pos - read_pos);
    else if (write_pos < read_pos)
        return (buffer_size - read_pos) + write_pos;
    return 0UL;
}
