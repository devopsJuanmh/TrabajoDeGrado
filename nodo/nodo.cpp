#include "nodo.h"

/*
*Configuración inicial para el modulo de comunicación y el ADC.
* El modulo de comunicación se inicializa mediante el setup y el ADC
* se inicializa mediante el begin , ademas, se deben inicializar los parametros
* del DRATE y el canal por el cual se van a tomar las muestras.
*/


nodo::nodo(){
}

void nodo::nodoInit(){
  /*
  * Se inicializan los valores respectivos de los pines y se configuran como entrada o salida
  */
  SPI.begin();
  int CS=9; // CHIPSELECT del ADS1265. pin 10 del attiny
  int DRDY=2; // la interrupción del ADC pin 8 del attiny 2 en el arduino
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(SCK, OUTPUT);
  pinMode(sSel,OUTPUT);
  pinMode(CS,OUTPUT); //cs = 10;
  digitalWrite(sSel,HIGH); //se coloca el sSel del modulo xbee en alto.
  digitalWrite(CS,HIGH);
  pinMode(ATTN,INPUT); // se deben definir los pines de las interrupciones para el ATTN y el DRDY como entradas.
  //pinMode(DRDY,INPUT);
  buffer.buff1[90]=0x1A; // esto se debe cambiar dependiendo del nodo 
  mod1.setup();
  #ifdef DEBUGNODO
  Serial.println("Iniciando el procesamiento");
  #endif
  Serial.println("Serial online");
  ADS.begin(CS,DRDY);
  Serial.println("Inicio completo");
  ADS.SetRegisterValue(DRATE,DR_100);
  Serial.println("Rate completo");
  ADS.SendCMD(SELFCAL);
  delay(10);
  unsigned long rate= ADS.GetRegisterValue(DRATE);
  Serial.println(rate,HEX);
  ADS.initCh(P_AIN7);
  Serial.println("canal completo");
  unsigned long mux= ADS.GetRegisterValue(MUX);
  Serial.println(mux,HEX);

}
void nodo::processCh(uint8_t ch){
  /*
  * Se cambia el canal cada vez que hay una interrupción
  * se toma la lectura del canal anterior.
  * se incrementa el valor del puntero por 3 bytes.
  */

    flagFull = buffer.checkBuffFull();
    if( (flagFull==true) ){
      mod1.sendFrame(buffer.env);
      #ifdef DEBUGNODO
      Serial.println("Enviando trama");
      #endif
    }
    //ADS.switchCh(ch);
    //ADS.GetConversion(&buffer);
}
void nodo::process(){
  /*
  * Acá se debe hacer la verificación de las direcciones y el cambio debe ser
  * coherente con enviar una sola vez.
  * Se incrementa el valor del env a 90 cuando se envie y
  */
  processCh(P_AIN5);
}
void nodo::interrupt(){
    #ifdef SINGLECHANNEL
    ADS.GetConversion(&buffer);
    #endif
    #ifndef SINGLECHANNEL
    if(canal==5){
    ADS.switchCh(P_AIN6);
    ADS.GetConversion(&buffer);
    canal=6;
    }
  else if(canal==6){
    ADS.switchCh(P_AIN7);
    ADS.GetConversion(&buffer);
    canal=7;
    }
  else if(canal==7){
    ADS.switchCh(P_AIN5);
    ADS.GetConversion(&buffer);
    canal=5;
    }
    #endif
}
