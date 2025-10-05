#include <18f4550 USB.h>

/* TODO: Use usb_cdc_putc() to transmit data to the USB
virtual COM port. Use usb_cdc_kbhit() and usb_cdc_getc() to
receive data from the USB virtual COM port. usb_enumerated()
can be used to see if connected to a host and ready to
communicate. */

#define LED1   PIN_D0
#define LED2   PIN_D1
#define LED3   PIN_D2

char recibido;




void main()
{
   usb_init();
   while(TRUE)
   {
      recibido = usb_cdc_getc();//RECIBE EL CARACTER

      if(recibido == 's')//busca el salto de linea \r\n
      {
         OUTPUT_BIT(LED1,1);
      }//enciende el led1

    else if(recibido == 'n')//busca el salto de linea \r\n
    
      {
         OUTPUT_BIT(LED1,0);
      }

    
}
   }



