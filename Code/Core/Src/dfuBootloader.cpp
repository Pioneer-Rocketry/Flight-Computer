/*
 * dfuBootloader.cpp
 *
 * STM32F4 DFU Bootloader Jump Implementation
 */

#include "dfuBootloader.h"
#include "stm32f4xx_hal.h"

// STM32F4 System Memory (bootloader) address
#define BOOTLOADER_START_ADDR   0x1FFF0000

// Magic value location in RAM (adjust based on your RAM size)
// For STM32F405/407: Use 0x2001FF00 (128KB RAM)
// For STM32F411: Use 0x2001FF00 (128KB RAM)
// For STM32F446: Use 0x2001FF00 (128KB RAM)
#define DFU_MAGIC_ADDR          0x2001FF00
#define DFU_MAGIC_VALUE         0xDEADBEEF

/**
 * @brief Jump to the STM32 system bootloader
 */
void dfuJumpToBootloader(void)
{
  uint32_t i = 0;
  void (*SysMemBootJump)(void);

  // Bootloader address
  volatile uint32_t BootAddr = BOOTLOADER_START_ADDR;

  // Disable all interrupts
  __disable_irq();

  // Disable Systick timer
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;

  // Clear Interrupt Enable Register & Interrupt Pending Register
  for (i = 0; i < 5; i++)
  {
    NVIC->ICER[i] = 0xFFFFFFFF;
    NVIC->ICPR[i] = 0xFFFFFFFF;
  }

  // Re-enable all interrupts
  __enable_irq();

  // Set up the jump to bootloader address + 4
  SysMemBootJump = (void (*)(void)) (*((uint32_t *)(BootAddr + 4)));

  // Set the main stack pointer to the bootloader stack
  __set_MSP(*(uint32_t *)BootAddr);

  // Call the function to jump to bootloader location
  SysMemBootJump();

  // Should never reach here
  while (1);
}

/**
 * @brief Check if bootloader entry was requested and jump if needed
 * @retval true if jumping to bootloader, false otherwise
 */
bool dfuCheckAndJumpBootloader(void)
{
  volatile uint32_t *magic_ptr = (volatile uint32_t *)DFU_MAGIC_ADDR;

  // Check if magic value is set
  if (*magic_ptr == DFU_MAGIC_VALUE)
  {
    // Clear the magic value
    *magic_ptr = 0;

    // Small delay to ensure clear
    for (volatile uint32_t i = 0; i < 100000; i++);

    // Jump to bootloader (never returns)
    dfuJumpToBootloader();

    // Should never reach here
    return true;
  }

  return false;
}

/**
 * @brief Request bootloader entry on next reset
 */
void dfuRequestBootloaderEntry(void)
{
  volatile uint32_t *magic_ptr = (volatile uint32_t *)DFU_MAGIC_ADDR;

  // Set magic value in RAM
  *magic_ptr = DFU_MAGIC_VALUE;

  // Perform system reset
  NVIC_SystemReset();

  // Should never reach here
  while (1);
}

/**
 * @brief Force reboot to application (used after DFU upload)
 * @note Call this if the bootloader doesn't automatically exit
 */
void dfu_reboot_to_application(void)
{
  // Simple system reset - will boot to application
  NVIC_SystemReset();
}