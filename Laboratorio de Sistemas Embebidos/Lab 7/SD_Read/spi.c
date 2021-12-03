#include <stdint.h>
#include "spi.h"
#include "sam.h"
#include "myprintf.h"

#define kCMD00 0x40
#define kCMD08 0x48
#define kCMD55 0x77
#define kCMD41 0x69

#define LENGTH_R1 0x03
#define LENGTH_R7 0x07

void spiInit( void ) {
	/* Switch to 8MHz clock (disable prescaler) */
	SYSCTRL->OSC8M.bit.PRESC = 0;

	PM->APBCMASK.bit.SERCOM1_ = 1; //enable the clock for SERCOM1 peripheral
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_SERCOM1_CORE;
	while( GCLK->STATUS.bit.SYNCBUSY ) { }
	
	//structures to configure the SERCOM1 peripheral
	// DATASHEET PAGE 492
	const SERCOM_SPI_CTRLA_Type ctrla = {
		.bit.DORD = 0x0, // MSB first
		.bit.CPHA = 0x0, // Mode 0
		.bit.CPOL = 0x0,
		.bit.FORM = 0x0, // SPI frame
		.bit.DIPO = 0x3, // MISO on PAD[3]
		.bit.DOPO = 0x0, // MOSI on PAD[0], SCK on PAD[1], SS_ on PAD[2]
		.bit.MODE = 0x3  // Master Mode
	};
	SERCOM1->SPI.CTRLA.reg = ctrla.reg;
	const SERCOM_SPI_CTRLB_Type ctrlb = {
		.bit.RXEN = 0x1,   // RX enabled
		.bit.MSSEN = 0x0,  // Manual SC
		.bit.CHSIZE = 0x0  // 8-bit
	};
	SERCOM1->SPI.CTRLB.reg = ctrlb.reg;

	//Formula to configure the desired baudrate
	uint32_t br = ( double )( 8000000 / ( 2 * BAUDRATE ) ) - 1 ;
	SERCOM1->SPI.BAUD.reg = SERCOM_SPI_BAUD_BAUD( ( uint8_t )br );

	//structure to configure multiple PINs
	const PORT_WRCONFIG_Type wrconfig = {
		.bit.WRPINCFG = 1,
		.bit.WRPMUX = 1,
		.bit.PMUX = MUX_PA16C_SERCOM1_PAD0 | MUX_PA17C_SERCOM1_PAD1 | MUX_PA19C_SERCOM1_PAD3,
		.bit.PMUXEN = 1,
		.bit.HWSEL = 1,
		.bit.PINMASK = ( uint16_t )( ( PORT_PA16 | PORT_PA17 | PORT_PA19 ) >> 16 )
	};
	PORT->Group[0].WRCONFIG.reg = wrconfig.reg;

	//SS/CS (Slave Select/Chip Select) PIN 18 configuration
	REG_PORT_DIRSET0 = PORT_PA18;
	REG_PORT_OUTSET0 = PORT_PA18;
	
	//enable the SPI
	SERCOM1->SPI.CTRLA.bit.ENABLE = 1;
	while( SERCOM1->SPI.SYNCBUSY.bit.ENABLE ) { }
}

uint8_t spiSend( uint8_t data ) {
	uint8_t temp;
	while( !SERCOM1->SPI.INTFLAG.bit.DRE ) { } //wait until buffer is empty
	SERCOM1->SPI.DATA.reg = SERCOM_SPI_DATA_DATA( data ); //transmit data
	while( !SERCOM1->SPI.INTFLAG.bit.RXC ) { } //wait until a data is received
	temp = SERCOM1->SPI.DATA.reg; //read data
	while( !SERCOM1->SPI.INTFLAG.bit.TXC ) { } //wait until there is no data to transmit
	//myprintf( " %x", temp ); //printf the value in putty
	return temp;
}

uint32_t spiXchg(const uint8_t * send_buff, uint32_t bc, uint8_t * receive_buff ) {
	uint8_t temp = 0xFF;
	uint32_t i;
	uint8_t temp_cmd = send_buff[0];
	
	REG_PORT_OUTCLR0 = PORT_PA18;
	for(i=0; i< bc; i++) {
		temp = spiSend(*(send_buff++));
	}
	switch(temp_cmd) {
		case kCMD00 :
		for(i=0; i<LENGTH_R1; i++) {
			temp = spiSend(0xFF);
		}
		break;
		case kCMD08 :
		for(i=0; i<LENGTH_R7; i++) {
			temp = spiSend(0xFF);
		}
		break;
		case kCMD41 :
		for(i=0; i<LENGTH_R1-1; i++) {
			temp = spiSend(0xFF);
		}
		spiSend(0xFF);
		break;
		case kCMD55 :
		for(i=0; i<LENGTH_R1; i++) {
			temp = spiSend(0xFF);
		}
		break;
		default :
		myprintf("\n Error in CMD");
	}
	REG_PORT_OUTSET0 = PORT_PA18;
	return(temp);
}