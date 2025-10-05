#include <pwm_para_cervo.h>
//// configuraciones ////
#fuses nowdt    //desactiva perro guardian. es un temporizador que reinicia el microcontrolador si detecta que el programa se ha quedado atascado en un bucle infinito
#fuses hs         // para configurar el pic a frecuencias de clk mas de 8M Hz. 
#fuses noprotect  //permite que el código almacenado en el microcontrolador pueda ser leído por dispositivos externos, lo cual puede ser útil durante el desarrollo y la depuración, pero no es recomendable para productos finales donde se desea proteger la propiedad intelectual.
//#fuses nomclr     //El fusible NOMCLR desactiva la función de Master Clear (MCLR) del microcontrolador. Cuando NOMCLR está activado, el pin MCLR se convierte en una entrada digital normal en lugar de ser utilizado para reiniciar el microcontrolador1. Esto puede ser útil si necesitas más pines de entrada/salida y no necesitas la función de reinicio externo.
//#fuses novregen   // se utiliza para desactivar la función de Voltage Regulator Enable (VREGEN) en algunos microcontroladores PIC. Esta función controla el regulador de voltaje interno del microcontrolador.. Al usar #FUSES NOVREGEN, se desactiva esta función, lo que puede ser útil en situaciones donde se utiliza una fuente de alimentación externa regulada o cuando se desea reducir el consumo de energía
////
int T=0;



#int_timer0   //100uS
void interrupcion()
{
if (T==200)
{
      OUTPUT_HIGH(PIN_B7); //0grados
      output_high(pin_b6); // 180 grados
      T=0;
}
else if (T < 200)
{
      if (T == 5)   // 500us
      {
         output_low(pin_b7);
      }
      if (T == 24)  // 2400US
      {
         OUTPUT_LOW(pin_b6);
      }
      T++;

}
set_timer0(6);
}






void main()
{
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_2|RTCC_8_BIT);      //100 us overflow
enable_interrupts(INT_TIMER0);
enable_interrupts(GLOBAL);
SET_TIMER0(6); 
 

   while(TRUE)
   {

      
   }

}
