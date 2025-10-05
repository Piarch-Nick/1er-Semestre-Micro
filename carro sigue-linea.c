#include <carro sigue-linea.h>

//// configuraciones ////
#fuses nowdt    //desactiva perro guardian. es un temporizador que reinicia el microcontrolador si detecta que el programa se ha quedado atascado en un bucle infinito
#fuses hs         // para configurar el pic a frecuencias de clk mas de 8M Hz. 
#fuses noprotect  //permite que el c�digo almacenado en el microcontrolador pueda ser le�do por dispositivos externos, lo cual puede ser �til durante el desarrollo y la depuraci�n, pero no es recomendable para productos finales donde se desea proteger la propiedad intelectual.
//#fuses nomclr     //El fusible NOMCLR desactiva la funci�n de Master Clear (MCLR) del microcontrolador. Cuando NOMCLR est� activado, el pin MCLR se convierte en una entrada digital normal en lugar de ser utilizado para reiniciar el microcontrolador1. Esto puede ser �til si necesitas m�s pines de entrada/salida y no necesitas la funci�n de reinicio externo.
//#fuses novregen   // se utiliza para desactivar la funci�n de Voltage Regulator Enable (VREGEN) en algunos microcontroladores PIC. Esta funci�n controla el regulador de voltaje interno del microcontrolador.. Al usar #FUSES NOVREGEN, se desactiva esta funci�n, lo que puede ser �til en situaciones donde se utiliza una fuente de alimentaci�n externa regulada o cuando se desea reducir el consumo de energ�a
////

#use standard_io(a)
#use standard_io(b)

//// prototipos de funciones ////
void rampa_creciente_derecha();
void rampa_decreciente_derecha();

////

//// inicializacion ////
int count1=0;
int duty_derecha=0;
////



#INT_TIMER0
void  TIMER0_isr(void)     // entra cada 1.2us
{
      if(count1 == 50)      //perido 60us  // f=16.66khz
      {
            output_bit(pin_b0,1);
            count1=0;
      }
      else if (count1 < 50)
      {
             if(count1 == duty_derecha)
             {
                  output_bit(pin_b0,0);
             }
              count1++;
      }
      set_timer0(250); //ajustar
}

void main()
{
   setup_timer_0(RTCC_INTERNAL | RTCC_8_BIT |RTCC_DIV_1);
   set_timer0(250);   // desborda en 1.2us // cada tic es de 200ns (256 tics)
   
   enable_interrupts(INT_TIMER0);
   enable_interrupts(GLOBAL);
   
 //  disable_interrupts(Global); // no queremos el pwm por ahora...
   
   while(TRUE)
   {
            
            while(input(pin_a0) == 0)     // mientras el sensor este desactivado
            {
                  rampa_creciente_derecha();
            }
            
            while(input(pin_a0) == 1) 
            {
                  rampa_decreciente_derecha();
            }
            
            disable_interrupts(Global); // no queremos el pwm por ahora...
            output_bit(pin_b0,0);
            //break;
            
         
   }

}

void rampa_creciente_derecha()
{
   if(duty_derecha == 49)
   {
         ;
   }
   else if (duty_derecha < 49)
   {
        duty_derecha++;
        delay_ms(10);
   }
}
void rampa_decreciente_derecha()
{
   if(duty_derecha == 49)
   {
        duty_derecha--;
        delay_ms(10); ;
   }
   else if (duty_derecha == 0)
   {
        ;
   }
}
