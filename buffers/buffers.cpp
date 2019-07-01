#include "buffers.h"

buff::buff(){
}

void buff::incLec(int inc)
{
  cont += inc;
}

void buff::saveData(uint8_t data)
{
  lec[cont] = data;
  cont++;
}

/*
* Este metodo verifica la posicion del puntero y cambia el buffer
*/

bool buff::checkEnvDone(){
  if(send == 1){
    return true;
  }
  else{
    return false;
  }
}

bool buff::checkBuffFull(){
  if(cont == buffsize){
    uint8_t *aux = lec;
    lec=env;
    env=aux;

    cont = 0;

    return true;
  }
  else{
    return false;
  }
}

bool buff::cambiarBuff(){
    if(lec == &buff1[buffsize]){
      lec=buff2;
      env=buff1;
      return true;
    }
    else if(lec == &buff2[buffsize]){
      lec=buff1;
      env=buff2;
      return true;
    }
    else {
      return false;
    }

}
void buff::InsertBytes(){

}
void buff::resetBuff1(){
  int i=0;
  for (i=0;i<buffsize;i++){
    buff1[i]=0x0;
  }
}
void buff::resetBuff2(){
  int i=0;
  for (i=0;i<buffsize;i++){
    buff2[i]=0x0;
  }
}
