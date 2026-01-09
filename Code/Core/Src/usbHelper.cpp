#include "usbHelper.h"

/**
 * @brief Sends a string over the TinyUSB CDC interface.
 * * @param str Pointer to the character array (string) to send.
 * @param len Length of the string (in bytes).
 * @retval The number of bytes successfully written to the USB buffer.
 */
size_t cdcSendMessage(char* str, size_t len)
{
  // 1. Check if the USB CDC interface is connected to the host
  if (!tud_cdc_connected())
  {
    return 0; // Not connected, nothing sent
  }

  // 2. Write as much as the buffer can hold
  // This allows partial writes when the buffer is full, which is better
  // for high throughput and prevents silent failures
  size_t written_len = tud_cdc_write(str, len);

  // 3. Flush the buffer to ensure the data is sent immediately
  // NOTE: For better performance, you might only flush periodically,
  // but for simple messages, flushing immediately is often best.
  tud_cdc_write_flush();

  return written_len;
}