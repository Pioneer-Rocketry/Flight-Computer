#ifndef BSP_BOARD_API_H
#define BSP_BOARD_API_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t board_get_unique_id(uint8_t id[], size_t max_len);

// Return serial as UTF-16 characters, copied into `buf` and return
// number of UTF-16 code points written (not including terminating null).
size_t board_usb_get_serial(uint16_t buf[], size_t max_len);

#ifdef __cplusplus
}
#endif

#endif /* BSP_BOARD_API_H */
