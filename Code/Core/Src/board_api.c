#include <stddef.h>
#include <stdint.h>

// Minimal stub for board_get_unique_id required by tinyusb board API.
// Returns 0 bytes (no unique id available).
size_t board_get_unique_id(uint8_t id[], size_t max_len) {
    (void)id;
    (void)max_len;
    return 0;
}

// Minimal implementation to provide a USB serial string. This writes
// up to `max_len` UTF-16 code units into `buf` and returns the number
// of code units written.
size_t board_usb_get_serial(uint16_t buf[], size_t max_len) {
    const char *s = "00000000"; // fallback placeholder
    size_t i = 0;
    while (i < max_len && s[i]) {
        buf[i] = (uint16_t)s[i];
        ++i;
    }
    return i;
}
