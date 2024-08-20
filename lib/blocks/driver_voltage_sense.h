#ifndef DRIVER_VOLTAGE_SENSE_H
#define DRIVER_VOLTAGE_SENSE_H

#include "generic_block.h"

struct VoltageContainer
{
    int16_t voltg_norm;
    int32_t voltg_mv;
    int32_t max_sup_voltage;
};


#endif  // DRIVER_VOLTAGE_SENSE_H