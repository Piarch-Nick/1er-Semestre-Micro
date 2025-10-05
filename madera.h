#include <16F877.h>
#device ADC=10
#use delay(crystal=4MHz)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,stream=PORT1)

