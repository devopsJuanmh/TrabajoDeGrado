/******************************************************************************
* Elaborado por: Juan Manuel Hoyos Alvarez.
* Librería para el envio básico de tramas API1 de los módulos Xbee-PRO 900HP
* utilizando el puerto SPI, el proyecto puede ser escalado para la
* implementación  de la UART y el modo API2. Se requiere hacer uso de Arduino.h
* y SPI.h para el correcto funcionamiento de los métodos aquí implementados.
* Si desea, se puede cambiar el tamaño de la variable RFData dependiendo de los
* requerimientos de envio, recuerde que el máximo de carga útil para los módulos
* XBEE-PRO 900HP es de 100 bytes. En caso de que quiera tener acceso a los
* comandos AT, descomente la línea #define ATCMD. En caso de querer ver mensajes
* de depuración, descomente la línea #define DEBUG.

* Para la implementación en el IDE de Arduino incluya la librería, cree un
* objeto (ejemplo: xbee mod1(0x22))de la clase xbee asegurandose de incluir
* el tamaño de la trama a enviar, después cree una instancia del setup
* (mod1.setup)y modifique el valor del RFData (mod1.RFData = ...),
* para enviar la trama implemente del metodo sendFrame (mod1.sendFrame).
*******************************************************************************/

#ifndef xbee_h
#define xbee_h
#define NODO1 0x1A;
#define NODO2 0x2B;
//#define ATCMD
//#define DEBUGXBEE

#define XBEE_BUFSIZE 91 /* tamaño del buffer o carga útil, 91 Bytes para este
                          * caso son 91 bytes */
/*definitions of frames for API1 mode
*API frame names and IDs sent to the module:
*/

/*API frame names and IDs received from the device
*/
//Definiciones para la trama Transmission request
#define ADDR 0x0013A200416D9AB8 //Dirección del nodo central.

// Se definen los parametros de SPI.
#define sSel 10 //9 en attiny 9 en el arduino
#define ATTN 7 //7 en attiny //3 en el arduino

#include "SPI.h"
#include "Arduino.h"
#include "stdint.h"


void ATTN_interrupt();


class xbee {
public:
  xbee();
  void sendFrame(uint8_t *ptrRF);
  void formCheckSumRFData(uint8_t *ptrRF);
#ifdef ATCOMMAD
  uint8_t formCheckSumAT();
  void sendAT(unsigned char AT[2]);
#endif
  void setup();
#ifdef ATCOMMAD
  unsigned char ATCmd[2];
#endif
  uint8_t frameID;
  uint8_t checkSum = 0; //privado
  uint16_t checkSumAdd = 0;//privado
  void xbee::ATTN_interrupt();
private:
  uint16_t _length;
  uint8_t frameStandar[17];
};

#endif
