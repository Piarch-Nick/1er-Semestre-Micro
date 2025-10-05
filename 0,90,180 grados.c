#include <0,90,180 grados.h>
//// configuraciones ////
#fuses nowdt    //desactiva perro guardian. es un temporizador que reinicia el microcontrolador si detecta que el programa se ha quedado atascado en un bucle infinito
#fuses hs         // para configurar el pic a frecuencias de clk mas de 8M Hz. 
#fuses noprotect  //permite que el c�digo almacenado en el microcontrolador pueda ser le�do por dispositivos externos, lo cual puede ser �til durante el desarrollo y la depuraci�n, pero no es recomendable para productos finales donde se desea proteger la propiedad intelectual.
//#fuses nomclr     //El fusible NOMCLR desactiva la funci�n de Master Clear (MCLR) del microcontrolador. Cuando NOMCLR est� activado, el pin MCLR se convierte en una entrada digital normal en lugar de ser utilizado para reiniciar el microcontrolador1. Esto puede ser �til si necesitas m�s pines de entrada/salida y no necesitas la funci�n de reinicio externo.
//#fuses novregen   // se utiliza para desactivar la funci�n de Voltage Regulator Enable (VREGEN) en algunos microcontroladores PIC. Esta funci�n controla el regulador de voltaje interno del microcontrolador.. Al usar #FUSES NOVREGEN, se desactiva esta funci�n, lo que puede ser �til en situaciones donde se utiliza una fuente de alimentaci�n externa regulada o cuando se desea reducir el consumo de energ�a
//////////////////////////

// Manejo de Puertos //
#use standard_io(a)
#use standard_io(b)
#use standard_io(c)
#use standard_io(d)
/////////////////////////


#define PWM   PIN_D2


INT16 count1 =0;
INT duty_servo_Ma=0;


#INT_TIMER0
void interrupt()       // cada 100us entra
{

      if(count1 == 200)      //perido 20ms  //frecuencia 50hz
      {
            output_bit(PWM,1); 
         //   output_bit(servo_MT,1); 
            count1=0;
      }
      
      else if (count1 < 200)
      {
           if(count1 == duty_servo_Ma) 
           {
             output_bit(PWM,0);
           }
      //     if(count1 == duty_servo_MT) 
      //    {
      //       output_bit(servo_MT,0);
       //    }
              count1++;
      }
      //set_timer0(6);
      set_timer0(9); // colocamos esta carga quiza para cuadrar los retardos
      
}




void main()
{

   setup_timer_0(RTCC_INTERNAL | RTCC_8_BIT |RTCC_DIV_2);
    set_timer0(6);                  // desborda en 100us // cada tic es de 400ns
    enable_interrupts(GLOBAL);
    enable_interrupts(INT_TIMER0);   
   duty_servo_Ma = 5;
   
   
    while(TRUE)
   {
   
      duty_servo_Ma = 3;
      DELAY_MS(300);

       duty_servo_Ma = 20;
      DELAY_MS(300);
      
      
      
   }
}
