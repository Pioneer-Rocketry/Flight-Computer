/*
 * RFM95.cpp
 *
 *  Created on: Jan 9, 2026
 *      Author: colin
 */

#include "Devices/SPI_Devices/RFM95.h"

RFM95::RFM95(DataContainer* data, SPI_HandleTypeDef *spi, GPIO_TypeDef *port, uint16_t pin)
    : SPIDevice(data, spi, port, pin)
{
}

int RFM95::deviceInit()
{
    readSPI(RFM95_VERSION, &read, 1);

    if (read != 0x18)
    {
        return -1;
    }

    /* Device Configuration */

    // Put the device into sleep mode
	command  = 0b10000000;
	writeSPI(RFM95_OP_MODE | 0x80, &command);

	// RegOpMode page 102
	command  = 0;
    command |= 0b1   << 7; // LongRangeMode, enables LoRa
    command |= 0b00  << 5;
    command |= 0b0   << 4; // LowFrequencyModeOn. enables High Frequency Mode
    command |= 0b000 << 0;
	writeSPI(RFM95_OP_MODE | 0x80, &command);

    // Set the frequency
    setFreq(915000000);

	// RegPaConfig page 103
	command  = 0;
    command |= 0b1    << 7; // PaSelect, enables TX Power of +20 dBm
    command |= 0b111  << 4; // MaxPower, set the max power pMax = 10.8 + 0.6 * MaxPower
    command |= 0b1111 << 0; // OutputPower, sets the output power to Pout = 17 - (15 - OutputPower)
	writeSPI(RFM95_PA_CONFIG | 0x80, &command);
    readSPI(RFM95_PA_CONFIG, &read);

    // RegOcp
    command = 0;
    command |= 0b00    << 6; // Unused
    command |= 0b1     << 5; // OcpOn, enables the Overload Current Protection for the PA 
    command |= 0b10010 << 0; // OcpTrim, sets the max current Imax = -30 + 10 * OcpTrim
	writeSPI(RFM95_OCP | 0x80, &command);

    // RegLna
    command = 0;
    command |= 0b001 << 5; // LnaGain, sets the LNA's gain set to max
    command |= 0b00  << 3; // LnaBoostLf, sets the LF LNA current set to default
    command |= 0b0   << 2; // Reserved
    command |= 0b00  << 0; // LnaBoostHf, sets the HF LNA current set to boost
	writeSPI(RFM95_LNA | 0x80, &command);

    // RegModemConfig
    command = 0;
    command |= bandwidth << 4; // BW
    command |= coding    << 1; // CodingRate
    command |= 0b1       << 0; // ImplicitHeaderMode, 0 = Explicity 1 = Implicit Header mode
	writeSPI(RFM95_MODEM_CONFIG| 0x80, &command);

    // RegModemConfig2
    command = 0;
    command |= spreadfactor << 4; // SpreadFactor
    command |= 0b0          << 3; // TxContinuousMode, sets it up in normal more, a single packet is sent
    command |= 0b0          << 2; // RxPayloadCrcOn, 
    command |= 0b00         << 0; // SymbTimeout, 
	writeSPI(RFM95_MODEM_CONFIG_2 | 0x80, &command);

    // RegMaxPayloadLength
    command = MAX_PAYLOAD_LENGTH;
	writeSPI(RFM95_MAX_PAYLOAD_LENGTH | 0x80, &command);

    // RegHopPeriod
    command = 0; // Symbol periods between frequency hops. (0 = disabled). 1st hop always happen after the 1st header symbol
	writeSPI(RFM95_HOP_PERIOD | 0x80, &command);

    // RegModemConfig
    command = 0;
    command |= 0b0000 << 4; // Unused
    command |= 0b1    << 3; // MobileNode, 0 = Static Node, 1 = Mobile Node
    command |= 0b0    << 2; // AgcAutoOn, 0 = LNA gain from LnaGain, 1 = LNA gain from AGC loop
    command |= 0b00   << 0; // Reserved
	writeSPI(RFM95_MODEM_CONFIG_3 | 0x80, &command);

    command = 0b10000001;
	writeSPI(RFM95_OP_MODE | 0x80, &command);

    HAL_Delay(1);

    lastTransmittion = HAL_GetTick();

    return 0; // Return 0 on success
}

int RFM95::updateDevice()
{
    now = HAL_GetTick();

    if (now - lastTransmittion >= TRANSMISSION_INTERVAL)
    {
        compilePacket();
        sendPacket();

        lastTransmittion = HAL_GetTick();
    }
    

    return 0; // Return 0 on success
}

void RFM95::setFreq(uint32_t freqHz)
{
    frf = freqHz / RFM95_FSTEP;
    frfMSB = (frf >> 16) & 0xFF;
    frfMID = (frf >> 8) & 0xFF;
    frfLSB = frf & 0xFF;

    writeSPI(RFM95_FR_MSB | 0x80, &frfMSB);
    writeSPI(RFM95_FR_MID | 0x80, &frfMID);
    writeSPI(RFM95_FR_LSB | 0x80, &frfLSB);

    return;
}

void RFM95::sendPacket()
{
    command = 0x00;
    writeSPI(RFM95_FIFO_TX_BASE_ADDR | 0x80, &command);
    writeSPI(RFM95_FIFO_ADDR_PTR | 0x80, &command);

    
    payloadLength = sizeof(payload);

    writeSPI(RFM95_PAYLOAD_LENGTH | 0x80, &payloadLength);

    for (int i = 0; i < payloadLength; i++) {
        writeSPI(RFM95_FIFO | 0x80, &payload[i]);
    }

    command = 0xFF;
    writeSPI(RFM95_IRQ_FLAGS | 0x80, &command);

    command = 0x80 | 0x03;
    writeSPI(RFM95_OP_MODE | 0x80, &command); // LoRa + TX

    
    // bool waiting = true;
    // while(waiting)
    // {
    //     readSPI(RFM95_IRQ_FLAGS, &read, 1);
    //     waiting = !(read & 0x08 == 0);
    // }

}

void RFM95::compilePacket()
{
    for (int i=0; i < MAX_PAYLOAD_LENGTH; i++)
    {
        payload[i] = i;
    }

}