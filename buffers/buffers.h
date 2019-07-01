#ifndef buffers_h
#define buffers_h

#define buffsize 91

#include "Arduino.h"

class buff{
  public:
    volatile int send = 1;
    int cont= 0;
    buff();
    uint8_t buff1[buffsize];
    uint8_t buff2[buffsize];
    uint8_t *lec = buff1; //buffer de lectura
    uint8_t *env = buff2; //buffer de envio

    bool checkBuffFull();
    bool cambiarBuff();
    bool checkEnvDone();
    void saveData(uint8_t data);
    void InsertBytes();
    void incLec(int inc);
    void incEnv(int inc);
    void resetBuff1();
    void resetBuff2();
};

#endif
