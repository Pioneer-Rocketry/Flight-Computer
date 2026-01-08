/*
 * GPS.h
 *
 *  Created on: Jan 08, 2026
 *      Author: colin
 */

#ifndef SRC_DEVICES_GPS_H_
#define SRC_DEVICES_GPS_H_

#include "main.h"
#include "DataContainer.h"
#include <string.h>

#define GPS_DMA_BUFFER_SIZE 512
#define GPS_UBX_MAX_PAYLOAD 256

class GPS {
public:
    GPS(DataContainer* data, UART_HandleTypeDef* uart, uint8_t* dmaBuffer);
    
    int deviceInit();
    int updateDevice();
    
private:
    DataContainer* data;
    UART_HandleTypeDef* uart;
    uint8_t* dmaBuffer;
    
    uint16_t currentBufferPos;
    uint16_t lastDmaBufferPos;
    
    // UBX message buffer
    uint8_t ubxBuffer[GPS_UBX_MAX_PAYLOAD];
    uint16_t ubxIndex;
    
    // UBX-NAV-PVT structure (92 bytes payload)
    struct __attribute__((packed)) UBX_NAV_PVT {
        uint32_t iTOW;          // GPS time of week (ms)
        uint16_t year;          // Year (UTC)
        uint8_t month;          // Month (1-12)
        uint8_t day;            // Day (1-31)
        uint8_t hour;           // Hour (0-23)
        uint8_t min;            // Minute (0-59)
        uint8_t sec;            // Second (0-60)
        uint8_t valid;          // Validity flags
        uint32_t tAcc;          // Time accuracy estimate (ns)
        int32_t nano;           // Fraction of second (-1e9 to 1e9)
        uint8_t fixType;        // GPS fix type (0=none, 3=3D fix)
        uint8_t flags;          // Fix status flags
        uint8_t flags2;         // Additional flags
        uint8_t numSV;          // Number of satellites
        int32_t lon;            // Longitude (1e-7 deg)
        int32_t lat;            // Latitude (1e-7 deg)
        int32_t height;         // Height above ellipsoid (mm)
        int32_t hMSL;           // Height above MSL (mm)
        uint32_t hAcc;          // Horizontal accuracy (mm)
        uint32_t vAcc;          // Vertical accuracy (mm)
        int32_t velN;           // North velocity (mm/s)
        int32_t velE;           // East velocity (mm/s)
        int32_t velD;           // Down velocity (mm/s)
        int32_t gSpeed;         // Ground speed (mm/s)
        int32_t headMot;        // Heading of motion (1e-5 deg)
        uint32_t sAcc;          // Speed accuracy (mm/s)
        uint32_t headAcc;       // Heading accuracy (1e-5 deg)
        uint16_t pDOP;          // Position DOP (0.01)
        uint8_t reserved1[6];   // Reserved
        int32_t headVeh;        // Heading of vehicle (1e-5 deg)
        int16_t magDec;         // Magnetic declination (1e-2 deg)
        uint16_t magAcc;        // Magnetic declination accuracy (1e-2 deg)
    };
    
    // UBX parser state machine
    enum UBXParseState {
        WAIT_SYNC1,
        WAIT_SYNC2,
        GET_CLASS,
        GET_ID,
        GET_LENGTH1,
        GET_LENGTH2,
        GET_PAYLOAD,
        GET_CHECKSUM1,
        GET_CHECKSUM2
    };
    
    UBXParseState parseState;
    uint8_t msgClass;
    uint8_t msgId;
    uint16_t payloadLength;
    uint16_t payloadIndex;
    uint8_t ckA, ckB;
    
    // Private methods
    void processDMAData();
    bool parseUBXByte(uint8_t byte);
    void processUBXMessage();
    void sendUBXCommand(const uint8_t* cmd, uint16_t len);
    void calculateChecksum(const uint8_t* data, uint16_t len, uint8_t* ckA, uint8_t* ckB);
    void configureGPS();
};

#endif /* SRC_DEVICES_GPS_H_ */