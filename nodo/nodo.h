#ifndef nodo_h
#define nodo_h
#include "xbee.h"
#include "ads12xx.h"
#include "buffers.h"
#include "Arduino.h"
#include "PinChangeInterrupt.h"

#define SINGLECHANNEL
//#define DEBUGNODO
#define NOSEND;
class nodo{
  public:
    nodo();
    ads12xx ADS;
    buff buffer;
    xbee mod1;
    volatile bool envDone=false;
    int cont = 0;
    volatile bool flagFull=false;
    volatile bool flagCambio=false;
    int canal=6;

    void process();
    void processCh(uint8_t ch);
    void nodoInit();
    void bufferCheck();
    void interrupt();
    void sendRdy();
};
#endif
