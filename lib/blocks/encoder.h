#ifndef ENCODER_H
#define ENCODER_H

#include "../../spi_setup.h"
#include "generic_block.h"

class Encoder {
    BLOCK_OUTPUT(uint32_t, output);

public:
    Encoder() {}

    void tick() {
        output_ = SPI1_rx_buffer[1] << 17;
        SPI1_StartTransfer();
    }
};

#endif  // ENCODER_H