#include "xbee.h"
#include "PinChangeInterrupt.h"


/*Función ISR que es llamada por la interrupción generada en
* por el pin del ATTN, acá se limpia todo el buffer hasta que
* el ATTN se ponga en alto.
*/


xbee::xbee(){
  /*se define el constructor, los parametros de la comunicación SPI
  * y se completa la primera parte de la trama.
  */
  int cor=56;
  int length = XBEE_BUFSIZE + 14;

  frameStandar[0]=0x7E; //delimitador de la trama.;
  frameStandar[1]=(length >> 8) & 0xff;
  frameStandar[2]=(length) & 0xff;
  frameStandar[3]=0x10; // TX Request;
  frameStandar[4]=0x01;
  for(int i=5;i<=12;i++)
  {
    frameStandar[i]=(ADDR>>cor) & 0xff;
    cor-=8;
  }
  frameStandar[13]=0xFF;
  frameStandar[14]=0xFE;
  frameStandar[15]=0x00; // broadcast radius.;
  frameStandar[16]= 0xC0; // se define una red point-multipoint;
}

void xbee::formCheckSumRFData(uint8_t *ptrRF){
/* Se calcula el checksum para las tramas de RF
*/
  for(int i=3;i<=16;i++){
    checkSumAdd = checkSumAdd + frameStandar[i];
  }
}

void xbee::sendFrame(uint8_t *ptrRF){
  /* Ese metodo permite el envio de las tramas contenidas en la variable
  * RFData de la clase xbee, se realiza el envio al modulo por medio de SPI
  * y el modulo debe responde con un frame status donde el contenido útil es
  * 0x00 indicando que el mensaje ha llegado correctamente, esto es manejado
  *mediante la interrupción del ATTN.
  */

  checkSum = 0;
  checkSumAdd = 0;
  #ifndef DEBUGXBEE
  formCheckSumRFData(ptrRF);
    for(int i=0;i<17;i++)
    {
      Serial.write(frameStandar[i]);
    }
    for(int j = 0; j<XBEE_BUFSIZE;j++){
      checkSumAdd = checkSumAdd + *(ptrRF+j);
      Serial.write(*(ptrRF+j));
    }
    checkSum = 0xFF - (checkSumAdd & 0xFF);
    Serial.write(checkSum);
    #endif
    #ifdef DEBUGXBEE
    formCheckSumRFData(ptrRF);
      for(int i=0;i<17;i++)
      {
        Serial.print(frameStandar[i],HEX);
      }
      for(int j = 0; j<XBEE_BUFSIZE;j++){
        checkSumAdd = checkSumAdd + *(ptrRF+j);
        Serial.print(*(ptrRF+j),HEX);
      }
      checkSum = 0xFF - (checkSumAdd & 0xFF);
      Serial.print(checkSum,HEX);
      Serial.println();
      #endif
    frameStandar[4]++;
}

void xbee::setup(){
/* Este metodo inicia la comunicación con el modulo mediante
* el puerto SPI, el modulo al detectar que el SS ha sido
*afirmado (cambio de alto a bajo), comienza a utilizar el
*puerto SPI, de lo contrario, utilizara por defecto la UART.
*/
  //attachPCINT(digitalPinToPCINT(ATTN),ATTN_interrupt,FALLING);
}
