#include <18F4550.h>
#device ADC=10

#FUSES NOWDT                    //No Watch Dog Timer

#use delay(clock=48Mhz,crystal=20000000,USB_FULL)
//#use delay(clock=48Mhz,crystal=20000000,USB_FULL)


#define USB_CONFIG_BUS_POWER 500
#include <usb_cdc.h>

