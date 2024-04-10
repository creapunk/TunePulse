#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <inttypes.h>

namespace ERROR
{

// A union to manage driver error states through both individual flags and a collective integer.
union DriverError {
    struct ErrorFlags
    {
        uint32_t overCurrent : 1;
        uint32_t overVoltage : 1;
        uint32_t underVoltage : 1;
        uint32_t overTemperature : 1;
        uint32_t initializationFailure : 1;
        // @todo: Future expansion: Add additional error types here.
        uint32_t reserved : 27; // Reserved bits (27 bits) for future use or to align the struct to 32 bits.
    } flags;
    uint32_t error; // Allows reading or writing all error flags as a single 32-bit integer for efficiency.

    // Constructor initializes all error flags to 0 (indicating no error) by default.
    DriverError() : error(0)
    {
    }
};

// A union to manage driver error states through both individual flags and a collective integer.
union MotorError {
    struct ErrorFlags
    {
        uint32_t notSupported : 1;
        uint32_t wrongConnection : 1;
        uint32_t underVoltage : 1;
        uint32_t overTemperature : 1;
        uint32_t initializationFailure : 1;
        // @todo: Future expansion: Add additional error types here.
        uint32_t reserved : 27; // Reserved bits (27 bits) for future use or to align the struct to 32 bits.
    } flags;
    uint32_t error; // Allows reading or writing all error flags as a single 32-bit integer for efficiency.

    // Constructor initializes all error flags to 0 (indicating no error) by default.
    MotorError() : error(0)
    {
    }
};

} // namespace ERROR

#endif