/*
 * GPS.cpp
 *
 *  Created on: Jan 08, 2026
 *      Author: colin
 */

#include "Devices/GPS.h"
#include <stdio.h>

GPS::GPS(DataContainer* data, UART_HandleTypeDef* uart, uint8_t* dmaBuffer)
{
    this->uart = uart;
    this->dmaBuffer = dmaBuffer;

    this->currentBufferPos = 0;
    this->lastDmaBufferPos = 0;

    this->ubxIndex = 0;
    this->parseState = WAIT_SYNC1;
    this->payloadIndex = 0;
    
    // Initialize GPS data in DataContainer
    this->data = data;
    data->GPSLatitude = 0.0f;
    data->GPSLongitude = 0.0f;
    data->GPSAltitude_m = 0.0f;
    data->GPSFix = 0;
    data->GPSNumSatellites = 0;
    memset(data->GPSUTCTime, 0, sizeof(data->GPSUTCTime));
}

int GPS::deviceInit()
{
    // Wait for GPS to boot up
    HAL_Delay(500);
    
    // Configure GPS for UBX protocol
    configureGPS();
    
    return 0;
}

void GPS::calculateChecksum(const uint8_t* data, uint16_t len, uint8_t* ckA, uint8_t* ckB)
{
    *ckA = 0;
    *ckB = 0;
    
    for (uint16_t i = 0; i < len; i++)
    {
        *ckA += data[i];
        *ckB += *ckA;
    }
}

void GPS::sendUBXCommand(const uint8_t* cmd, uint16_t len)
{
    HAL_UART_Transmit(uart, (uint8_t*)cmd, len, 100);
}

void GPS::configureGPS()
{
    // Change baud rate from 9600 to 115200
    uint8_t cfgPrtBaud[] = {
        0xB5, 0x62,       // Sync chars
        0x06, 0x00,       // CFG-PRT
        0x14, 0x00,       // Length = 20
        0x01,             // PortID = UART1
        0x00,             // Reserved
        0x00, 0x00,       // txReady (not used)
        0xD0, 0x08, 0x00, 0x00, // Mode: 8N1
        0x00, 0xC2, 0x01, 0x00, // Baud: 115200 (0x0001C200)
        0x07, 0x00,       // inProtoMask: UBX + NMEA + RTCM
        0x01, 0x00,       // outProtoMask: UBX only
        0x00, 0x00,       // flags
        0x00, 0x00,       // Reserved
        0x00, 0x00        // Checksum placeholder
    };
    
    // Calculate checksum for CFG-PRT (skip sync chars)
    calculateChecksum(&cfgPrtBaud[2], 24, &cfgPrtBaud[26], &cfgPrtBaud[27]);
    sendUBXCommand(cfgPrtBaud, sizeof(cfgPrtBaud));
    
    HAL_Delay(100);
    
    // Now reconfigure UART on MCU side to 115200
    uart->Init.BaudRate = 115200;
    if (HAL_UART_Init(uart) != HAL_OK)
    {
        Error_Handler();
    }
    
    HAL_Delay(100);
    
    // Disable NMEA 0183 output (UBX only)
    uint8_t cfgPrt[] = {
        0xB5, 0x62,       // Sync
        0x06, 0x00,       // CFG-PRT
        0x14, 0x00,       // Length = 20
        0x01,             // PortID = UART1
        0x00,             // Reserved
        0x00, 0x00,       // txReady
        0xD0, 0x08, 0x00, 0x00, // Mode: 8N1
        0x00, 0xC2, 0x01, 0x00, // Baud: 115200
        0x01, 0x00,       // inProtoMask: UBX only
        0x01, 0x00,       // outProtoMask: UBX only
        0x00, 0x00,       // flags
        0x00, 0x00,       // Reserved
        0x00, 0x00        // Checksum
    };
    calculateChecksum(&cfgPrt[2], 24, &cfgPrt[26], &cfgPrt[27]);
    sendUBXCommand(cfgPrt, sizeof(cfgPrt));
    
    HAL_Delay(100);
    
    // Set navigation rate to 10 Hz
    uint8_t cfgRate[] = {
        0xB5, 0x62,       // Sync
        0x06, 0x08,       // CFG-RATE
        0x06, 0x00,       // Length = 6
        0x64, 0x00,       // measRate = 100 ms (10 Hz)
        0x01, 0x00,       // navRate = 1
        0x01, 0x00,       // timeRef = UTC
        0x00, 0x00        // Checksum
    };
    calculateChecksum(&cfgRate[2], 10, &cfgRate[12], &cfgRate[13]);
    sendUBXCommand(cfgRate, sizeof(cfgRate));
    
    HAL_Delay(100);
    
    // Enable UBX-NAV-PVT message
    uint8_t cfgMsg[] = {
        0xB5, 0x62,       // Sync
        0x06, 0x01,       // CFG-MSG
        0x03, 0x00,       // Length = 3
        0x01, 0x07,       // NAV-PVT (Class 0x01, ID 0x07)
        0x01,             // Rate: 1 (every solution)
        0x00, 0x00        // Checksum
    };
    calculateChecksum(&cfgMsg[2], 7, &cfgMsg[9], &cfgMsg[10]);
    sendUBXCommand(cfgMsg, sizeof(cfgMsg));
    
    HAL_Delay(100);
    
    // Save configuration to flash (optional but recommended)
    uint8_t cfgSave[] = {
        0xB5, 0x62,       // Sync
        0x06, 0x09,       // CFG-CFG (save configuration)
        0x0D, 0x00,       // Length = 13
        0x00, 0x00, 0x00, 0x00, // Clear mask
        0xFF, 0xFF, 0x00, 0x00, // Save mask (all)
        0x00, 0x00, 0x00, 0x00, // Load mask
        0x17,             // Device mask: BBR, Flash, EEPROM, SPI Flash
        0x00, 0x00        // Checksum
    };
    calculateChecksum(&cfgSave[2], 17, &cfgSave[19], &cfgSave[20]);
    sendUBXCommand(cfgSave, sizeof(cfgSave));
    
    HAL_Delay(500); // Wait for save to complete
}

int GPS::updateDevice()
{
    processDMAData();
    return 0;
}

void GPS::processDMAData()
{
    // Get current DMA write position
    currentBufferPos = GPS_DMA_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(uart->hdmarx);
    
    // Process new data
    if (currentBufferPos != lastDmaBufferPos)
    {
        if (currentBufferPos > lastDmaBufferPos)
        {
            // Normal case: no wrap-around
            for (uint16_t i = lastDmaBufferPos; i < currentBufferPos; i++)
            {
                if (parseUBXByte(dmaBuffer[i]))
                {
                    processUBXMessage();
                }
            }
        }
        else
        {
            // Wrap-around case
            for (uint16_t i = lastDmaBufferPos; i < GPS_DMA_BUFFER_SIZE; i++)
            {
                if (parseUBXByte(dmaBuffer[i]))
                {
                    processUBXMessage();
                }
            }
            for (uint16_t i = 0; i < currentBufferPos; i++)
            {
                if (parseUBXByte(dmaBuffer[i]))
                {
                    processUBXMessage();
                }
            }
        }
        
        lastDmaBufferPos = currentBufferPos;
    }
}

bool GPS::parseUBXByte(uint8_t byte)
{
    switch (parseState)
    {
        case WAIT_SYNC1:
            if (byte == 0xB5)
            {
                parseState = WAIT_SYNC2;
                ubxIndex = 0;
            }
            break;
            
        case WAIT_SYNC2:
            if (byte == 0x62)
            {
                parseState = GET_CLASS;
                ckA = 0;
                ckB = 0;
            }
            else
            {
                parseState = WAIT_SYNC1;
            }
            break;
            
        case GET_CLASS:
            msgClass = byte;
            ubxBuffer[ubxIndex++] = byte;
            ckA += byte;
            ckB += ckA;
            parseState = GET_ID;
            break;
            
        case GET_ID:
            msgId = byte;
            ubxBuffer[ubxIndex++] = byte;
            ckA += byte;
            ckB += ckA;
            parseState = GET_LENGTH1;
            break;
            
        case GET_LENGTH1:
            payloadLength = byte;
            ubxBuffer[ubxIndex++] = byte;
            ckA += byte;
            ckB += ckA;
            parseState = GET_LENGTH2;
            break;
            
        case GET_LENGTH2:
            payloadLength |= (byte << 8);
            ubxBuffer[ubxIndex++] = byte;
            ckA += byte;
            ckB += ckA;
            payloadIndex = 0;
            
            if (payloadLength > 0 && payloadLength < GPS_UBX_MAX_PAYLOAD)
                parseState = GET_PAYLOAD;
            else if (payloadLength == 0)
                parseState = GET_CHECKSUM1;
            else
                parseState = WAIT_SYNC1; // Payload too large
            break;
            
        case GET_PAYLOAD:
            ubxBuffer[ubxIndex++] = byte;
            ckA += byte;
            ckB += ckA;
            payloadIndex++;
            
            if (payloadIndex >= payloadLength)
                parseState = GET_CHECKSUM1;
            break;
            
        case GET_CHECKSUM1:
            if (byte == ckA)
            {
                parseState = GET_CHECKSUM2;
            }
            else
            {
                parseState = WAIT_SYNC1; // Checksum A error
            }
            break;
            
        case GET_CHECKSUM2:
            parseState = WAIT_SYNC1;
            if (byte == ckB)
            {
                return true; // Valid message received!
            }
            // Checksum B error
            break;
    }
    
    return false;
}

void GPS::processUBXMessage()
{
    // Check if this is NAV-PVT (Class 0x01, ID 0x07)
    if (msgClass == 0x01 && msgId == 0x07 && payloadLength == 92)
    {
        // Cast payload to UBX_NAV_PVT structure (skip class, id, length bytes)
        UBX_NAV_PVT* pvt = (UBX_NAV_PVT*)&ubxBuffer[4];
        
        // Update DataContainer with GPS data
        data->GPSLatitude = pvt->lat * 1e-7f;
        data->GPSLongitude = pvt->lon * 1e-7f;
        data->GPSAltitude_m = pvt->hMSL * 0.001f; // mm to meters
        data->GPSFix = pvt->fixType;
        data->GPSNumSatellites = pvt->numSV;
        
        // Format UTC time as "HHMMSS.sss"
        snprintf(data->GPSUTCTime, sizeof(data->GPSUTCTime),
                 "%02d%02d%02d.%03d",
                 pvt->hour, pvt->min, pvt->sec, 
                 (int)(pvt->nano / 1000000));
    }
}