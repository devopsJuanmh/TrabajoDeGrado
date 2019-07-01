/* ADS1256, datasheet: http://www.ti.com/lit/ds/sbas288j/sbas288j.pdf
  compare: https://github.com/Flydroid/ADS12xx-Library/blob/master/ads12xx.cpp
    connections to Atmega328 (UNO)

    CLK  - pin 13
    DIN  - pin 11 (MOSI)
    DOUT - pin 12 (MISO)
    CS   - pin 10
    DRDY - pin 9
    RESET- pin 8 (or tie HIGH?)
    DVDD - 3V3
    DGND - GND
*/

#define cs 10 // chip select
#define rdy 9 // data ready, input
#define rst 8 // may omit

#define SPISPEED 1250000
              // 1700000

#include <SPI.h>

void setup()
{
  Serial.begin(115200);

  pinMode(cs, OUTPUT);
  digitalWrite(cs, LOW); // tied low is also OK.
  pinMode(rdy, INPUT);
  pinMode(rst, OUTPUT);
  digitalWrite(rst, LOW);
  delay(1); // LOW at least 4 clock cycles of onboard clock. 100 microsecons is enough
  digitalWrite(rst, HIGH); // now reset to deafult values

  delay(500);
  SPI.begin(); //start the spi-bus
  delay(500);

  //init

  // digitalWrite(cs, LOW);
  while (digitalRead(rdy)) {}  // wait for ready_line to go low
  SPI.beginTransaction(SPISettings(SPISPEED, MSBFIRST, SPI_MODE1)); // start SPI
  delayMicroseconds(10);

  //Reset to Power-Up Values (FEh)
  SPI.transfer(0xFE);
  delayMicroseconds(100);

  byte status_reg = 0 ;  // address (datasheet p. 30)
  //PGA SETTING
  //1 ±5V
  //2 ±2.5V
  //4 ±1.25V
  //8 ±0.625V
  //16 ±312.5mV
  //32 ±156.25mV
  //64 ±78.125mV
  byte status_data = 0x01; //status: Most Significant Bit First, Auto-Calibration Disabled, Analog Input Buffer Disabled
  SPI.transfer(0x50 | status_reg);
  SPI.transfer(0x00);   // 2nd command byte, write one register only
  SPI.transfer(status_data);   // write the databyte to the register
  delayMicroseconds(10);


  byte adcon_reg = 2; //A/D Control Register (Address 02h)
  byte adcon_data = 0x20; // 0 01 00 000 => Clock Out Frequency = fCLKIN, Sensor Detect OFF, gain 1
  SPI.transfer(0x50 | adcon_reg);
  SPI.transfer(0x00);   // 2nd command byte, write one register only
  SPI.transfer(adcon_data);   // write the databyte to the register
  delayMicroseconds(10);

// digitalWrite(cs, HIGH);

  Serial.println("configured, starting");
}


void loop()
{
  unsigned long adc_val =0; // store reading

  // digitalWrite(cs, LOW);
  SPI.beginTransaction(SPISettings(SPISPEED, MSBFIRST, SPI_MODE1)); // start SPI
  delayMicroseconds(10);
  //The most efficient way to cycle through the inputs is to
  //change the multiplexer setting (using a WREG command
  //to the multiplexer register MUX) immediately after DRDY
  //goes low. Then, after changing the multiplexer, restart the
  //conversion process by issuing the SYNC and WAKEUP
  //commands, and retrieve the data with the RDATA
  //command.
  while (digitalRead(rdy)) {} ;

  byte channel = 0;
  byte data = (channel << 4) | (1 << 3); //AIN-channel and AINCOM
  SPI.transfer(0x50 | 1); // MUX register
  SPI.transfer(0x00);   // 2nd command byte, write one register only
  SPI.transfer(data);   // write the databyte to the register
  delayMicroseconds(10);

  //SYNC command 1111 1100
  SPI.transfer(0xFC);
  delayMicroseconds(10);

  //WAKEUP 0000 0000
  SPI.transfer(0x00);
  delayMicroseconds(10);

  SPI.transfer(0x01); // Read Data 0000  0001 (01h)
  delayMicroseconds(10);

  adc_val = SPI.transfer(0);
  adc_val <<= 8; //shift to left
  adc_val |= SPI.transfer(0);
  adc_val <<= 8;
  adc_val |= SPI.transfer(0);

  delayMicroseconds(10);

  // digitalWrite(cs, HIGH);
  SPI.endTransaction();

  //The ADS1255/6 output 24 bits of data in Binary Two's
  //Complement format. The LSB has a weight of
  //2VREF/(PGA(223 − 1)). A positive full-scale input produces
  //an output code of 7FFFFFh and the negative full-scale
  //input produces an output code of 800000h.
  if(adc_val > 0x7fffff){ //if MSB == 1
    adc_val = (16777215ul - adc_val) + 1; //do 2's complement
  }
  Serial.println(adc_val);

  delay(500);
  // digitalWrite(cs, HIGH);
}
