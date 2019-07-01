#include "ads12xx.h"
#ifndef PinChangeInterrupt.h
#define PinChangeInterrupt.h
#include "PinChangeInterrupt.h"
#endif

bool DRDY_state = 1;

// Waits untill DRDY Pin is falling (see Interrupt setup).
// Some commands like WREG, RREG need the DRDY to be low.
bool ads12xx::waitforDRDY() {
	while(digitalRead(_DRDY));
}

// ads12xx setup
ads12xx::ads12xx() {}

void ads12xx::begin(int CS, int DRDY) {
	pinMode(CS, OUTPUT);              // set the slaveSelectPin as an output:
	digitalWrite(CS, HIGH); // CS HIGH = nothing selected
	pinMode(DRDY, INPUT_PULLUP);             // DRDY read
	_CS = CS;
	_DRDY = DRDY;
}

void ads12xx::GetConversion(buff *buffer){
	uint32_t regData=0;
	SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1));
	digitalWrite(_CS, LOW); //Pull SS Low to Enable Communications with ADS1247
	noInterrupts();
	//SPI.transfer(WAKEUP);
	delayMicroseconds(10); // RD: Wait 25ns for ADC12xx to get ready
	SPI.transfer(RDATA); //Issue RDATA
	delayMicroseconds(7);
	buffer->saveData(SPI.transfer(NOP));
	buffer->saveData(SPI.transfer(NOP));
	buffer->saveData(SPI.transfer(NOP));
	delayMicroseconds(10);
	//SPI.transfer(STANDBY);
	interrupts();
	digitalWrite(_CS, HIGH);
	SPI.endTransaction();
	#ifdef DEBUGADC
	Serial.println("Tomando muestra");
	#endif
}


void ads12xx::switchCh(byte ch){
	ads12xx::SetRegisterValue(MUX,ch);
	delayMicroseconds(5);
	ads12xx::SendCMD(SYNC);
	delayMicroseconds(4);
	ads12xx::SendCMD(WAKEUP);
}
void ads12xx::initCh(byte ch){
	waitforDRDY();
	ads12xx::SetRegisterValue(MUX,ch);
	delayMicroseconds(5);
	ads12xx::SendCMD(SYNC);
	delayMicroseconds(4);
	ads12xx::SendCMD(WAKEUP);
}
// function to get a 3byte conversion result from the adc
long ads12xx::GetConversion() {
	int32_t regData=0;
	waitforDRDY();
	SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1));
	digitalWrite(_CS, LOW); //Pull SS Low to Enable Communications with ADS1247
	noInterrupts();
	//delayMicroseconds(10); // RD: Wait 25ns for ADC12xx to get ready
	SPI.transfer(RDATA); //Issue RDATA
	delayMicroseconds(7);
	regData |= SPI.transfer(NOP);
	//delayMicroseconds(10);
	regData <<= 8;
	regData |= SPI.transfer(NOP);
	//delayMicroseconds(10);
	regData <<= 8;
	regData |= SPI.transfer(NOP);
	//delayMicroseconds(10);
	interrupts();
	digitalWrite(_CS, HIGH);
	SPI.endTransaction();
	return regData;
}

// function to write a register value to the adc
// argumen: adress for the register to write into, value to write
void ads12xx::SetRegisterValue(uint8_t regAdress, uint8_t regValue) {

	uint8_t regValuePre = ads12xx::GetRegisterValue(regAdress);
	if (regValue != regValuePre) {
		SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1)); // initialize SPI with SPI_SPEED, MSB first, SPI Mode1
		digitalWrite(_CS, LOW);
		SPI.transfer(WREG | regAdress); // send 1st command byte, address of the register
		SPI.transfer(0x00);		// send 2nd command byte, write only one register
		SPI.transfer(regValue);         // write data (1 Byte) for the register
		digitalWrite(_CS, HIGH);
		#ifdef DEBUGADC
		if (regValue != ads12xx::GetRegisterValue(regAdress)) {   //Check if write was succesfull
			Serial.print("Write to Register 0x");
			Serial.print(regAdress, HEX);
			Serial.println(" failed!");
		}
		#endif
		SPI.endTransaction();
	}

}


//function to read a register value from the adc
//argument: adress for the register to read
unsigned long ads12xx::GetRegisterValue(uint8_t regAdress) {
	uint8_t bufr;
	//digitalWriteFast(_START, HIGH);
	SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1)); // initialize SPI with 4Mhz clock, MSB first, SPI Mode0
	digitalWrite(_CS, LOW);
	delayMicroseconds(10);
	SPI.transfer(RREG | regAdress); // send 1st command byte, address of the register
	SPI.transfer(0x00);			// send 2nd command byte, read only one register
	delayMicroseconds(10);
	bufr = SPI.transfer(NOP);	// read data of the register
	delayMicroseconds(10);
	digitalWrite(_CS, HIGH);
	//digitalWriteFast(_START, LOW);
	SPI.endTransaction();
	return bufr;
}

/*
Sends a Command to the ADC
Like SELFCAL, GAIN, SYNC, WAKEUP
*/
void ads12xx::SendCMD(uint8_t cmd) {
	//attachPCINT(digitalPinToPCINT(_DRDY),DRDY_Interuppt,FALLING);//attachInterrupt(_DRDY, DRDY_Interuppt, FALLING);
	SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1)); // initialize SPI with 4Mhz clock, MSB first, SPI Mode0
	digitalWrite(_CS, LOW);
	delayMicroseconds(10);
	SPI.transfer(cmd);
	delayMicroseconds(10);
	digitalWrite(_CS, HIGH);
	SPI.endTransaction();
}


// function to reset the adc
void ads12xx::Reset() {
	SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE1)); // initialize SPI with  clock, MSB first, SPI Mode1
	digitalWrite(_CS, LOW);
	delayMicroseconds(10);
	SPI.transfer(RESET); //Reset
	delay(2); //Minimum 0.6ms required for Reset to finish.
	SPI.transfer(SDATAC); //Issue SDATAC
	delayMicroseconds(100);
	digitalWrite(_CS, HIGH);
	SPI.endTransaction();
}
