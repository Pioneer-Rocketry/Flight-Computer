#include "usbHelper.h"

/**
 * @brief Sends a string over the TinyUSB CDC interface.
 * * @param str Pointer to the character array (string) to send.
 * @param len Length of the string (in bytes).
 * @retval The number of bytes successfully written to the USB buffer.
 */
size_t cdcSendMessage(const char* str, size_t len)
{
  // 1. Check if the USB CDC interface is connected to the host
  if (!tud_cdc_connected())
  {
    return 0; // Not connected, nothing sent
  }

  // 2. Check if the USB transmit buffer has enough space for the whole string
  if (tud_cdc_write_available() < len)
  {
    // Not enough space, we can either:
    // a) Return 0 (what we do here, for simplicity)
    // b) Write only what fits (more complex, but better for high throughput)
    return 0;
  }

  // 3. Write the string to the USB buffer
  size_t written_len = tud_cdc_write(str, len);

  // 4. Flush the buffer to ensure the data is sent immediately
  // NOTE: For better performance, you might only flush periodically,
  // but for simple messages, flushing immediately is often best.
  tud_cdc_write_flush();

  return written_len;
}