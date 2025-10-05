#include <PROYECTO_HEATER_INDUCTION.h>
#fuses nowdt    //desactiva perro guardian. es un temporizador que reinicia el microcontrolador si detecta que el programa se ha quedado atascado en un bucle infinito
#fuses hs         // para configurar el pic a frecuencias de clk mas de 8M Hz. 
#fuses noprotect  //permite que el código almacenado en el microcontrolador pueda ser leído por dispositivos externos, lo cual puede ser útil durante el desarrollo y la depuración, pero no es recomendable para productos finales donde se desea proteger la propiedad intelectual.
#fuses noput
#fuses nolvp
//#fuses nomclr     //El fusible NOMCLR desactiva la función de Master Clear (MCLR) del microcontrolador. Cuando NOMCLR está activado, el pin MCLR se convierte en una entrada digital normal en lugar de ser utilizado para reiniciar el microcontrolador1. Esto puede ser útil si necesitas más pines de entrada/salida y no necesitas la función de reinicio externo.
//#fuses novregen   // se utiliza para desactivar la función de Voltage Regulator Enable (VREGEN) en algunos microcontroladores PIC. Esta función controla el regulador de voltaje interno del microcontrolador.. Al usar #FUSES NOVREGEN, se desactiva esta función, lo que puede ser útil en situaciones donde se utiliza una fuente de alimentación externa regulada o cuando se desea reducir el consumo de energía

#use STANDARD_IO( C )
#use STANDARD_IO( B )


/////////////Nombres de pines/////////////////////////////////////
/////////////////////////////////////////////////////////////////
#define modo_manual                          PIN_B2
#define gate_1                               PIN_C0    //SALIDA
#define Led_sobrecorriente                   PIN_C1    //SALIDA
#define Led_modo_stambay                     PIN_C2    //SALIDA
#define Led_modo_metal                       PIN_C3    //SALIDA
#define interrupcion_sobrecorriente          PIN_B0    //ENTRADA
#define feedback_igbt                        PIN_B1    //ENTRADA
////////////////////////////////////////////////////////////////////

#define duty_max 10  // duty cuando detecta metal
#define duty_min 4   // duty para los auto_set
/////////Declaracion de Variables Globales////////////////////////
////////////////////////////////////////////////////////////////////
int16 duty_cycle = 4;
//int1 enganche = 1;
int16 i=0;
int numero_pulsos = 0;
int1 hora_auto_set = 0;
////////////////////////////////////////////////////////////////



///////////////Prototipo de Funciones/////// ////////////////////
////////////////////////////////////////////////////////////////
Void Enganchado(); 
void auto_set();
//////////////////////////////////////////////////////////////

#INT_IOC
void  EXT_isr(void) 
{
     output_bit(gate_1,0);  // ante una subida de corriente, apaga el igbt
     while(1)
     {
         output_bit(led_sobrecorriente,1);  // led alarma
         output_bit(Led_modo_stambay,0);  // led alarma
         output_bit(Led_modo_metal,0);  // led alarma
     }
}

 #INT_TIMER0
void  TIMER0_isr(void)  //entra cada 420ms
{
     //  auto_set();   // la bobina tiene que estar al aire (permeabiidad del vacio)
       hora_auto_set = 1;
       set_timer0(0);

}


  


void main()
{
output_bit(led_sobrecorriente,0);  // led alarma
output_bit(Led_modo_stambay,0);
output_bit(Led_modo_metal,0);

enable_interrupts(INT_IOC_B0_H2L);
enable_interrupts(GLOBAL);
 enable_interrupts(INT_TIMER0);
   enable_interrupts(GLOBAL);
   
   setup_timer_0(T0_INTERNAL | T0_DIV_32 | T0_16_BIT);
   set_timer0(0);
   
   auto_set();   // la bobina tiene que estar al aire (permeabiidad del vacio)


   while(TRUE)
   {
   
         if (hora_auto_set == 1)
         { 
               auto_set();   // la bobina tiene que estar al aire (permeabiidad del vacio)
               hora_auto_set = 0;
               delay_us(300);  
         }
         
         
         if(duty_cycle== duty_max)
         {
  //         enganche = 1;
           Enganchado();
         }
     }
}
         
  


Void Enganchado()       // si se ha enganchado (encontro un level bajo en feedback) entra aqui.
{
      while(duty_cycle == duty_max)  // mantente aqui mientras se este enganchando, cuando deje de engarchar se sale.
      {
      // enganche = 0;
       output_bit(gate_1,1);        // lanzamos pulso
       delay_us(duty_cycle);
       output_bit(gate_1,0);
       delay_us(29);
      
      /*
     //    enganche = 0;                // dejamos apagado variable_enganchar
         output_bit(gate_1,1);        // lanzamos pulso
         delay_us(duty_cycle);
         output_bit(gate_1,0);
         delay_us(9);
         for(i=0 ; i<15 ; i++)    // buscamos el enganche entre (5-20)us
         {
                if(input(feedback_igbt) == 1)
                {
   //                enganche = 1;      // si engancha, encendemos variable_enganchar
                   delay_us(8);//12
                   break;
                }
                delay_us(1);
         }
         */
         if (hora_auto_set == 1)
         { 
                delay_ms(10);
                auto_set();   // la bobina tiene que estar al aire (permeabiidad del vacio)
                hora_auto_set = 0;
         }
      }
}


void auto_set()  // la bobina tiene que estar al aire (permeabiidad del vacio)
{
         numero_pulsos = 0;
         output_bit(gate_1,1);
         delay_us(duty_min);               // se le da un pulso de 4us
         output_bit(gate_1,0);
         delay_us(5);               // suponiendo T=20us, esperamos un cuarto  (5us)
         for(i=0 ; i<10 ; i++)  // a la espera de un pulso alto en feedback, entre 5us-15us, (cuando se pone alto, es que la senal alterna supera la ref continua)
         {
                if(input(feedback_igbt) == 1) 
                {
                     numero_pulsos = 1;
                     break;
                }
                delay_us(1);
         }
         delay_us(10-i);  // se garantiza que se espera hasta transcurridos 15us 
         delay_us(9); // tiempo extra para llegar a 24us
         for(i=0 ; i<8 ; i++)  // a la espera de otro pulso alto en feedback, entre 24us-32us 
         {
                if(input(feedback_igbt) == 1) //si se encuentra, entonces estamos en stambay y no hay metal. 
                {
                     numero_pulsos = 2;
                     break;
                }
                delay_us(1);
         }
         delay_us(8-i);  // se garantiza que se espera hasta transcurridos 32us 
         delay_us(10); // tiempo extra para llegar a 42us
         for(i=0 ; i<10 ; i++)  // a la espera de otro pulso alto en feedback, entre 42us-52us 
         {
                if(input(feedback_igbt) == 1) //si se encuentra, entonces estamos en stambay y no hay metal. 
                {
                     numero_pulsos = 3;
                     break;
                }
                delay_us(1);
         }
        
         if (input(modo_manual) == 1)
         {
               duty_cycle = duty_max;
               output_bit(Led_modo_stambay,0);
               output_bit(Led_modo_metal,1);
         }
         else if(numero_pulsos == 3) // modo stambay (baja potencia)
         {
               duty_cycle = duty_min;
               output_bit(Led_modo_stambay,1);
               output_bit(Led_modo_metal,0);
         }
         else if (numero_pulsos == 2 || numero_pulsos == 1) // hay metal (alta potencia)
         {
               duty_cycle = duty_max;
               output_bit(Led_modo_stambay,0);
               output_bit(Led_modo_metal,1);
         }
         else
      {
             duty_cycle = duty_min;
               output_bit(Led_modo_stambay,1);
               output_bit(Led_modo_metal,0);
      }
    ///  numero_pulsos = 0;
}
/*
//////////////////prueba de frecuencia de resonancia/////////////
void main()
{

output_bit(led_sobrecorriente,0);  // led alarma
  enable_interrupts(INT_IOC_B0_H2L);
    
     enable_interrupts(GLOBAL);
   while(TRUE)
   {
   
     
         for(int16 i=0 ; i<10000 ; i++)
         {
               output_bit(gate_1,1);
               delay_us(5);
               output_bit(gate_1,0);
               delay_us(200);
         }
          for(int16 i=0 ; i<10000 ; i++)
         {
               output_bit(gate_1,1);
               delay_us(7);
               output_bit(gate_1,0);
               delay_us(200);
         }
          for(int16 i=0 ; i<10000 ; i++)
         {
               output_bit(gate_1,1);
               delay_us(7);
               output_bit(gate_1,0);
               delay_us(200);
         }
   }

}


/*
//////////////////prueba para la resistencia/////////////
void main()
{


   while(TRUE)
   {
         output_bit(gate_1,1);
         delay_us(10);
         output_bit(gate_1,0);
         delay_us(20);

   }

}
*/
