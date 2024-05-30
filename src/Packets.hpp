// Radio/Packets.hpp
#ifndef Packets_hpp
#define Packets_hpp

#include <Arduino.h>


class GPS_Packet {
    private:
        int32_t latitude;
        int32_t longitude;

        int8_t numberOfSatellites;

        int16_t speed;
        uint16_t altitude;

    public:
    
};

typedef union {
    GPS_Packet packet;
    byte bytes[sizeof(GPS_Packet)];
} gps_packet_bytes;


class Data_Packet {
    private:
        uint8_t state;
        uint8_t pyro;

        int16_t accX;
        int16_t accY;
        int16_t accZ;

        int16_t gyroX;
        int16_t gyroY;
        int16_t gyroZ;

        int16_t magX;
        int16_t magY;
        int16_t magZ;

        int16_t pressure;
        int16_t altiude;

        int16_t roll;
        int16_t pitch;
        int16_t yaw;

        int16_t positionX;
        int16_t positionY;
        int16_t positionZ;

        int16_t rollRate;
        int16_t pitchRate;
        int16_t yawRate;

    public:
};

typedef union {
    Data_Packet packet;
    byte bytes[sizeof(Data_Packet)];
} data_packet_bytes;

#endif 