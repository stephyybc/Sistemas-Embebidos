#include <bcm2835.h> // Library of module
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define DIR_RTC 104 // Direccion modulo RTC 
#define TC74_ADDR 77 // Direccion modulo TC74
#define LECTURA 0 // Constante de Lectura
#define ESCRITURA 1 // Constante de Escritura
#define CLK_DIV 2500 // Frecuencia reloj
#define LONGITUD_MAX 32 // Definimos maxima longitud
#define TRUE 1 // Definimos constante true
#define FALSE 0 // Definimos constante false

char buffer[LONGITUD_MAX]; // Definimos el maximo de datos a guardar

// Fecha inicial 01/01/01 Mon 12:00:00 AM	
uint8_t rtc_config[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01}; // Brindamos la fecha Inicial
// Data: Seg, Min, Hora, Dia_De_La_Semana, Dia, Mes, Año
uint8_t rtc_buff[7];
uint8_t tc74_buff[1];

typedef enum {
    NO_ACTION,
    I2C_BEGIN,
    I2C_END
} i2c_init;

// Configuration
uint8_t  init = NO_ACTION;
uint16_t clk_div = BCM2835_I2C_CLOCK_DIVIDER_148;
uint8_t slave_address = 0x00; 
uint32_t len = 0;
uint8_t  mode = LECTURA;
char buf[LONGITUD_MAX];
int i;
uint8_t data;
int guardados = 1;
char Archivo_Buffer[3][42];
char Archivo_Buffer_Temp[3][28];

int Begin_RTC(); // Inicializamos el modulo RTC
int RTC_get(); // Obtenemos datos del modulo RTC
int TC74_get(); // Obtenemos datos del modulo TC74
void delay_ms(); // Generar delay
char* weekday(int); // Convertimos el numero de la semana a dia de la semana
void Edit_Archivo(); // Escribimos en el .txt

int main(int argc, char **argv) {

  Begin_RTC(); // Inicializamos Modulo
  while(guardados <= 3 ){
    RTC_get(); // Obtenemos informacion Modulo RTC
    TC74_get(); // Obtenemos informacion modulo TC74
    // Desplegar informacion en Terminal
    sprintf(Archivo_Buffer_Temp[guardados%3], "RECEIVER> Temperature: %d°C\n", tc74_buff[0]); 
    printf("RECEIVER> Temperature: %d°C\n", tc74_buff[0]);
    sprintf(Archivo_Buffer[guardados%3], "RECEIVER> Record %d: %02X/%02X/%02X %s %02X:%02X:%02X\n", guardados, rtc_buff[4], rtc_buff[5], rtc_buff[6], weekday(rtc_buff[3]), rtc_buff[2], rtc_buff[1], rtc_buff[0]);
    Edit_Archivo(); // Escribimos en archivo de Texto
    guardados++;
    delay_ms(); // Generamos Retardo
  }
    return 0;
}
// Rutina Inicializacion modulo
int Begin_RTC(){ 

  if (!bcm2835_init())
  {
    printf("bcm2835_init failed. Are you running as root??\n");
    return 1;
  }

  if (!bcm2835_i2c_begin())
  {
    printf("bcm2835_i2c_begin failed. Are you running as root??\n");
    return 1;
  }

  bcm2835_i2c_setSlaveAddress(DIR_RTC);
  bcm2835_i2c_setClockDivider(CLK_DIV);

  data = bcm2835_i2c_write(rtc_config, 8);

  bcm2835_i2c_end();
  bcm2835_close();

  return data;

}
// Obtenemos Datos del modulo RTC
int RTC_get(){
  if (!bcm2835_init())
  {
    printf("bcm2835_init failed. Are you running as root??\n");
    return 1;
  }

  if (!bcm2835_i2c_begin())
  {
    printf("bcm2835_i2c_begin failed. Are you running as root??\n");
    return 1;
  }

  bcm2835_i2c_setSlaveAddress(DIR_RTC);
  bcm2835_i2c_setClockDivider(CLK_DIV);

  data = bcm2835_i2c_write(&rtc_config[0], 1);

  data = bcm2835_i2c_read(rtc_buff, 7);

  bcm2835_i2c_end();
  bcm2835_close();
  printf("RECEIVER> Record %d: %02X/%02X/%02X %s %02X:%02X:%02X\n", guardados, rtc_buff[4], rtc_buff[5], rtc_buff[6], weekday(rtc_buff[3]), rtc_buff[2], rtc_buff[1], rtc_buff[0]);
  return data;
}
// Obtenemos Datos del modulo TC74
int TC74_get(){
  if (!bcm2835_init())
  {
    printf("bcm2835_init failed. Are you running as root??\n");
    return 1;
  }

  if (!bcm2835_i2c_begin())
  {
    printf("bcm2835_i2c_begin failed. Are you running as root??\n");
    return 1;
  }

  bcm2835_i2c_setSlaveAddress(TC74_ADDR);
  bcm2835_i2c_setClockDivider(CLK_DIV);

  data = bcm2835_i2c_read(tc74_buff, 1);

  bcm2835_i2c_end();
  bcm2835_close();

  return tc74_buff[0];
}
// Rutina generar delay
void delay_ms()
{
    int milli_seconds = 1000 * 10000;
    clock_t start_time = clock();
    while (clock() < start_time + milli_seconds){
      if(TC74_get()>30)
        break;
    }
}
// Conversion de numero de dia a Dia
char* weekday(int day){
  switch (day)
  {
  case 0:
    return "Mon";
  case 1:
    return "Tue";
  case 2:
    return "Wed";
  case 3:
    return "Thu";
  case 4:
    return "Fri";
  case 5:
    return "Sat";
  case 6:
    return "Sun";
  default:
    return "Error"; // Caso de error
  }
}

char date_log[20], temp_log[28];
// Escribimos en archivo TXT
void Edit_Archivo(){
  FILE *fp;
  fp = fopen("output.txt","w");
  if(fp){
    for(int i=1; i<=3; i++){
      snprintf(date_log, 42, "%s", &Archivo_Buffer[i]);
      snprintf(temp_log, 28, "%s", &Archivo_Buffer_Temp[i]);
      fprintf(fp, "%s\n", temp_log);
      fprintf(fp, "%s\n", date_log);
    }
    fclose(fp);
  }
}