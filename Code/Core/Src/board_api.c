#include <stddef.h>
#include <stdint.h>

// Minimal stub for board_get_unique_id required by tinyusb board API.
// Returns 0 bytes (no unique id available).
size_t board_get_unique_id(uint8_t id[], size_t max_len) {
    (void)id;
    (void)max_len;
    return 0;
}
