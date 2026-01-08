/*
 * dfuBootloader.h
 *
 * STM32F4 DFU Bootloader Jump Functions
 */

#ifndef DFU_BOOTLOADER_H
#define DFU_BOOTLOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Check if DFU bootloader entry was requested and jump if so
 * @note Call this at the very beginning of main() before any initialization
 * @retval true if jumping to bootloader (never returns), false if normal boot
 */
bool dfuCheckAndJumpBootloader(void);

/**
 * @brief Request a reboot into DFU bootloader mode
 * @note This function performs a system reset
 */
void dfuRequestBootloaderEntry(void);

/**
 * @brief Jump directly to the bootloader without reset
 * @note This is a low-level function, prefer using dfuRequestBootloaderEntry()
 */
void dfuJumpToBootloader(void);

/**
 * @brief Force reboot to application after DFU
 * @note Only needed if bootloader doesn't auto-exit after programming
 */
void dfuRebootToApplication(void);

#ifdef __cplusplus
}
#endif

#endif /* DFU_BOOTLOADER_H */