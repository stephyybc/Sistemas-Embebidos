#include "sam.h"
#include "uart.h"
#include "myprintf.h"
#include "spi.h"

#define SIZE_SD_CMD 0x06
#define RXBUFSIZE 0x400

void initCycles(void);

const uint8_t CMD00[SIZE_SD_CMD]  ={0x40, 0x00, 0x00, 0x00, 0x00, 0x95};
const uint8_t CMD08[SIZE_SD_CMD]  ={0x48, 0x00, 0x00, 0x01, 0xAA, 0x87};
uint8_t CMD17[SIZE_SD_CMD]  ={0x51, 0x00, 0x00, 0x00, 0x00, 0x01};
uint8_t CMD172[SIZE_SD_CMD]  ={0x51, 0x00, 0x00, 0x08, 0x00, 0x01};
const uint8_t CMD18[SIZE_SD_CMD]  ={0x52, 0x00, 0x00, 0x00, 0x00, 0x01};
const uint8_t CMD55[SIZE_SD_CMD]  ={0x77, 0x00, 0x00, 0x00, 0x00, 0x65};
const uint8_t CMD41[SIZE_SD_CMD] = {0x69, 0x40, 0x00, 0x00, 0x00, 0x77};

uint8_t RxBuffer[RXBUFSIZE];

void rcvr_datablock(const uint8_t * send_buff, uint32_t lba, uint8_t * receive_buff, uint32_t bs );

int main(void)
{
  
	uint32_t temp = 0xFF;
	/* Initialize the SAM system */
	SystemInit();

	UARTInit();
	spiInit();
	myprintf("\nSTART CYCLES\n");
	initCycles();
	myprintf("\nEND CYCLES\nCMD00\n");
	do
	{
		spiXchg(CMD00, SIZE_SD_CMD, RxBuffer);
	} while (RxBuffer[1] != 1);
	myprintf("RxBuffer: %x\n", RxBuffer[1]);
	spiXchg(CMD08, SIZE_SD_CMD, RxBuffer);
	if (RxBuffer[1] != 1 && RxBuffer[4] != 1 && RxBuffer[5] != 0xAA)
	{
		myprintf("Error from SD card");
		return;
	}
	myprintf("RxBuffer: %x\n", RxBuffer[1]);
	do 
	{
		spiXchg(CMD55, SIZE_SD_CMD, RxBuffer);
		spiXchg(CMD41, SIZE_SD_CMD, RxBuffer);
	} while (RxBuffer[1] != 0);
  

	rcvr_datablock(CMD17, 0x00, RxBuffer, 512);
  
	return 0;
}

void initCycles(void){
	uint32_t i;
	REG_PORT_OUTSET0 = PORT_PA18;
	for(i=0;i<74;i++)
	spiSend(0xFF);
}

void rcvr_datablock(const uint8_t * send_buff, uint32_t lba, uint8_t * receive_buff, uint32_t bs ) {
	uint8_t temp = 0xFF;
	uint32_t i;
	
	REG_PORT_OUTCLR0 = PORT_PA18;
	myprintf("\n\n");

	temp = send_buff[0];
	temp = spiSend(temp);
	myprintf(" %x", temp);

	temp = send_buff[1] | (lba >> 24);
	temp = spiSend(temp);
	myprintf(" %x", temp);
	
	temp = send_buff[2] | ((lba >> 16 ) & 0xFF);
	temp = spiSend(temp);
	myprintf(" %x", temp);
	
	temp = send_buff[3] | ((lba >> 8 ) & 0xFF);
	temp = spiSend(temp);
	myprintf(" %x", temp);
	
	temp = send_buff[4] | (lba & 0xFF);
	temp = spiSend(temp);
	myprintf(" %x", temp);

	temp = send_buff[5];
	temp = spiSend(temp);
	myprintf(" %x", temp);

	// Reading to find the beginning of the sector

	temp = spiSend(0xFF);
	while(temp != 0xFE){
		temp = spiSend(0xFF);
	}
	
	// Receiving the memory sector/block
	
	myprintf("\n\n");
	for(i=0; i< bs; i++) {
		while(SERCOM1->SPI.INTFLAG.bit.DRE == 0);
		SERCOM1->SPI.DATA.reg = 0xFF;
		while(SERCOM1->SPI.INTFLAG.bit.TXC == 0);
		while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
		temp = SERCOM1->SPI.DATA.reg;
		*(receive_buff++) = temp;
		myprintf(" %x", temp);
	}
	REG_PORT_OUTSET0 = PORT_PA18;
	myprintf("\n\n");
}