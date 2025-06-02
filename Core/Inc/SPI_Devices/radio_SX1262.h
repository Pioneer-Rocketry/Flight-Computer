#ifndef RADIO_SX1262_H
#define RADIO_SX1262_H

#include "stm32f4xx_hal.h" /* Needed for SPI */

#include "SPI_Devices/SPI_Device.hpp"
#include "data.h"

// Registers
#define SX1262_HOPPING_ENABLE           0x0385
#define SX1262_PACKET_LENGTH            0x0386
#define SX1262_NB_HOPPING_BLOCKS        0x0387
#define SX1262_NB_SYMBOLS_0             0x0388  // 2 bytes: 0x0388 - 0x0389
#define SX1262_FREQ_0                   0x038A  // 4 bytes: 0x038A - 0x038D
// ...
#define SX1262_NB_SYMBOLS_15            0x03E2  // 2 bytes: 0x03E2 - 0x03E3
#define SX1262_FREQ_15                  0x03E4  // 4 bytes: 0x03E4 - 0x03E7
#define SX1262_DIOX_OUTPUT_ENABLE       0x0580
#define SX1262_DIOX_INPUT_ENABLE        0x0583
#define SX1262_DIOX_PULL_UP             0x0584
#define SX1262_DIOX_PULL_DOWN           0x0585
#define SX1262_WHITENING_INIT_MSB       0x06B8
#define SX1262_WHITENING_INIT_LSB       0x06B9
#define SX1262_CRC_MSB_INIT_VALUE_0     0x06BC
#define SX1262_CRC_LSB_INIT_VALUE_1     0x06BD
#define SX1262_CRC_MSB_POLY_VALUE_0     0x06BE
#define SX1262_CRC_LSB_POLY_VALUE_1     0x06Bf
#define SX1262_SYNC_WORD_0              0x06C0
#define SX1262_SYNC_WORD_1              0x06C1
#define SX1262_SYNC_WORD_2              0x06C2
#define SX1262_SYNC_WORD_3              0x06C3
#define SX1262_SYNC_WORD_4              0x06C4
#define SX1262_SYNC_WORD_5              0x06C5
#define SX1262_SYNC_WORD_6              0x06C6
#define SX1262_SYNC_WORD_7              0x06C7
#define SX1262_NODE_ADDRESS             0x06CD
#define SX1262_BROADCAST_ADDRESS        0x06CE
#define SX1262_IQ_POLARITY_SETUP        0x0736
#define SX1262_LORA_SYNC_WORD_MSB       0x0740
#define SX1262_LORA_SYNC_WORD_LSB       0x0741
#define SX1262_LORA_CODING_RATE_RX      0x0749
#define SX1262_LORA_CRC_CONFIG_RX       0x076B
#define SX1262_DCC_CTRL                 0x0805
#define SX1262_MIX_CTRL                 0x0806
#define SX1262_MIX_MODE                 0x0818
#define SX1262_IF_FREQ_0                0x088F
#define SX1262_IF_FREQ_1                0x0890
#define SX1262_IF_FREQ_2                0x0891
#define SX1262_RNG_0                    0x0819
#define SX1262_RNG_1                    0x081A
#define SX1262_RNG_2                    0x081B
#define SX1262_RNG_3                    0x081C
#define SX1262_TX_MODULATION            0x0889
#define SX1262_RX_GAIN                  0x08AC
#define SX1262_TX_CLAMP_CONF            0x08D8
#define SX1262_OCP_CONFIG               0x08E7
#define SX1262_RTC_CONTOL               0x0902
#define SX1262_XTA_TRIM                 0x0911
#define SX1262_XTB_TRIM                 0x0912
#define SX1262_DIO3_OUTPUT_VOLT_CTRL    0x0920
#define SX1262_EVENT_MASK               0x0944


class Radio_SX1262 : public SPI_Device
{
private:

    void writeRegister(uint16_t reg, uint8_t value);
    uint8_t readRegister(uint16_t reg);

public:
    Radio_SX1262(Data *data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin);

    bool begin() override;
    void loop() override;
};

#endif /* RADIO_SX1262_H */
