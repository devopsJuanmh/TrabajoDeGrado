#ifndef ads12xx_H
#define ads12xx_H // Definiciones para la libreria del ADC.

//#define ADS1248
#define ADS1256 // Se define el modelo 1256
//#define DEBUGADC

/*Serial Template */

#ifdef ADS1248
#include "ads1248.h"
#endif // ADS1248

#ifdef ADS1256
#include "ads1256.h"
#endif // ADS1256

#define CS_ 10; // CHIPSELECT del ADS1265. pin 10 del attiny
#define  DRDY_ 8; // 8  en el attiny 2 en el arduino

#include "SPI.h"
#include "buffers.h"


class ads12xx {
public:
	ads12xx();
	void begin(
		int CS,
		int DRDY
		);
	void Reset();
	unsigned long  GetRegisterValue(uint8_t regAdress);
	void SendCMD(uint8_t cmd);
	void SetRegisterValue(uint8_t regAdress,uint8_t regValue);
	long  GetConversion();
	void GetConversion(int flag);
	void GetConversion(buff *buffer);
	void switchCh(byte ch);
	void initCh(byte ch);
	void GetConversion(char *muestras);
	uint8_t HiB =0, MedB =0, LowB = 0 ;
	bool waitforDRDY();
	private:
	int _CS;
	int _DRDY;
	int _START;
	volatile int DRDY_state;
};
#endif
