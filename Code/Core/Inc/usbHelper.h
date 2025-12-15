#ifndef INC_USBHELPER_H_
#define INC_USBHELPER_H_

#include <stdio.h>

#include "tusb.h"

#define MAX_TICK_MSG_LEN 40
static char usb_tx_buffer[MAX_TICK_MSG_LEN];

/**
 * @brief Sends a string over the TinyUSB CDC interface.
 * * @param str Pointer to the character array (string) to send.
 * @param len Length of the string (in bytes).
 * @retval The number of bytes successfully written to the USB buffer.
 */
size_t cdcSendMessage(const char* str, size_t len);

#endif /* INC_USBHELPER_H_ */