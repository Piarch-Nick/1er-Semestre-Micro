#include <MUCHOS PWM CON TIMER0.h>

//// configuraciones ////
#fuses nowdt    //desactiva perro guardian. es un temporizador que reinicia el microcontrolador si detecta que el programa se ha quedado atascado en un bucle infinito
#fuses hs         // para configurar el pic a frecuencias de clk mas de 8M Hz. 
#fuses noprotect  //permite que el código almacenado en el microcontrolador pueda ser leído por dispositivos externos, lo cual puede ser útil durante el desarrollo y la depuración, pero no es recomendable para productos finales donde se desea proteger la propiedad intelectual.
//#fuses nomclr     //El fusible NOMCLR desactiva la función de Master Clear (MCLR) del microcontrolador. Cuando NOMCLR está activado, el pin MCLR se convierte en una entrada digital normal en lugar de ser utilizado para reiniciar el microcontrolador1. Esto puede ser útil si necesitas más pines de entrada/salida y no necesitas la función de reinicio externo.
//#fuses novregen   // se utiliza para desactivar la función de Voltage Regulator Enable (VREGEN) en algunos microcontroladores PIC. Esta función controla el regulador de voltaje interno del microcontrolador.. Al usar #FUSES NOVREGEN, se desactiva esta función, lo que puede ser útil en situaciones donde se utiliza una fuente de alimentación externa regulada o cuando se desea reducir el consumo de energía
////

#use standard_io(b)

//// prototipos de funciones ////
void elegir_estado_siguiente();
void ejecutar_estado_actual();
////

//// inicializacion ////
int count1=0;
int16 count2 =0;
float read0=0;
float read1=0;
float read2=0;
float read3=0;
int DC0=0;
int DC1=0;
int16 DC2=0;
int16 DC3=0;
////



#INT_TIMER0
void interrupt()   // cada 5us entra
{
      if(count1 == 200)      //perido 10ms
      {
            output_bit(pin_b0,1);
            output_bit(pin_b1,1);
            count1=0;
      }
      else if (count1 < 200)
      {
             if(count1 == DC1)  //25% de DC
             {
                  output_bit(pin_b1,0);
             }
             if(count1 == DC0) //50% de DC
             {
                  output_bit(pin_b0,0);
             }
              count1++;
      }
      
      if(count2 == 400)      //perido 20ms
      {
            output_bit(pin_b3,1);
            output_bit(pin_b2,1);
            count2=0;
      }
      else if (count2 < 400)
      {
             if(count2 == DC3)  //25% de DC
             {
                  output_bit(pin_b3,0);
             }
             if(count2 == DC2) //50% de DC
             {
                  output_bit(pin_b2,0);
             }
              count2++;
      }
      set_timer0(87);  //en realidad es 6. pero ajustando por los tiempos nos da 87.
}



void main()
{
   
   setup_timer_0(T0_DIV_1);
   set_timer0(6);                  // desborda en 50us
   
   enable_interrupts(GLOBAL);
   enable_interrupts(INT_TIMER0);
   
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_adc_ports(AN0_AN1_AN2_AN3_AN4);
   
   while(TRUE)
   {
         elegir_estado_siguiente();
         ejecutar_estado_actual();
   }

}

void elegir_estado_siguiente()
{
     
}

void ejecutar_estado_actual()
{
      set_adc_channel(0);
      delay_us(100);
      read0 = read_adc(ADC_START_AND_READ);
      DC0 = (read0 * (200./1024.));
      
      
      set_adc_channel(1);
      delay_us(100);
      read1 = read_adc(ADC_START_AND_READ);
      DC1 = (read1 * (200./1024.));
      
      
      set_adc_channel(2);
      delay_us(100);
      read2 = read_adc(ADC_START_AND_READ);
      DC2 = (read2 * (400./1024.));
      
      set_adc_channel(3);
      delay_us(100);
      read3 = read_adc(ADC_START_AND_READ);
      DC3 = (read3 * (400./1024.));
}
