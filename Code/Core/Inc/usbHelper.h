#ifndef INC_USBHELPER_H_
#define INC_USBHELPER_H_

#include <stdio.h>

#include "tusb.h"

/**
 * @brief Sends a string over the TinyUSB CDC interface.
 * * @param str Pointer to the character array (string) to send.
 * @param len Length of the string (in bytes).
 * @retval The number of bytes successfully written to the USB buffer.
 */
size_t cdcSendMessage(char* str, size_t len);

#endif /* INC_USBHELPER_H_ */