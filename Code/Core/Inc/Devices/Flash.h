/*
 * Flash.h
 *
 *  Created on: Feb 19, 2026
 *      Author: Derek Prince
 */

#ifndef Flash_H
#define Flash_H

#include <cstdint>
#include "main.h"

class Flash {
public:
    Flash();
    
    virtual int deviceInit();
    
    virtual void writeByte(uint8_t byte, uint32_t address);
    virtual uint32_t readByte(uint32_t address);

    virtual uint64_t getUUID();
private:


};
#endif /* Flash_H */

