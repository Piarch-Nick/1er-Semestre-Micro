#include <Comunicacion por PWM.h>

//// configuraciones ////
#fuses nowdt    //desactiva perro guardian. es un temporizador que reinicia el microcontrolador si detecta que el programa se ha quedado atascado en un bucle infinito
#fuses hs         // para configurar el pic a frecuencias de clk mas de 8M Hz. 
#fuses noprotect  //permite que el código almacenado en el microcontrolador pueda ser leído por dispositivos externos, lo cual puede ser útil durante el desarrollo y la depuración, pero no es recomendable para productos finales donde se desea proteger la propiedad intelectual.
//#fuses nomclr     //El fusible NOMCLR desactiva la función de Master Clear (MCLR) del microcontrolador. Cuando NOMCLR está activado, el pin MCLR se convierte en una entrada digital normal en lugar de ser utilizado para reiniciar el microcontrolador1. Esto puede ser útil si necesitas más pines de entrada/salida y no necesitas la función de reinicio externo.
//#fuses novregen   // se utiliza para desactivar la función de Voltage Regulator Enable (VREGEN) en algunos microcontroladores PIC. Esta función controla el regulador de voltaje interno del microcontrolador.. Al usar #FUSES NOVREGEN, se desactiva esta función, lo que puede ser útil en situaciones donde se utiliza una fuente de alimentación externa regulada o cuando se desea reducir el consumo de energía
////


// Manejo de Puertos //

#use standard_io(b)

////


//// inicializacion ////
int datos_a_enviar = 0b00000000;
int1 bit_a_enviar = 0;
////////////////////////


void main()
{   
   while(TRUE)
   {
   output_bit(pin_b1,0);
   delay_ms(1000);
       //Comunicacion TX para enviar 4 bit//
       
       //Enmascaramos el numero de 8bit a uno de 1bit menos significatico//
       // y vamos desplazando...//
       
         
         ////////////COMENZAMOS A ENVIAR//////////
              
              output_bit(pin_b1, 1);
              delay_ms(1);                // primer bit de inicio
              output_bit(pin_b1,0);
              delay_ms(5);
         /////////////////////////////////////////////////////////////////////////////////
         datos_a_enviar = 0b00001011;     //definimos el dato a enviar
         ////////////////////////////////////////////////////////////////////////////////////
         
         for (int k=0 ; k<4 ; k++)
         {
              
              bit_a_enviar = (datos_a_enviar & 0b00000001);    // Enmascarar
              
              if(bit_a_enviar)
              {
                  output_bit(pin_b1, 1);  //si el bit es 1 mandamos el bit por 2ms
                  delay_ms(2);
                  output_bit(pin_b1, 0);
                  delay_ms(4);
              }
              else
              {
                  output_bit(pin_b1, 1);  //si el bit es 0 mandamos el bit por 4ms
                  delay_ms(4);
                  output_bit(pin_b1, 0);
                  delay_ms(2);
              }
              
              datos_a_enviar = datos_a_enviar >> 1;
         }
        
              output_bit(pin_b1, 1);
              delay_ms(1);                // ultimo bit de parada
              output_bit(pin_b1,0);
              delay_ms(5);
   }

}
