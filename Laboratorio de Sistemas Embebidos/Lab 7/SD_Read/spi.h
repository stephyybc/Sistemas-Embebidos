#ifndef SPI_H_
#define SPI_H_

#define BAUDRATE 9600

void spiInit( void );
uint8_t spiSend( uint8_t data );
uint32_t spiXchg(const uint8_t * send_buff, uint32_t bc, uint8_t * receive_buff );
#include "myprintf.h"

#endif /* SPI_H_ */
© 2021 GitHub, Inc.
Terms
P