#include <comunicacion por PWM RECEPTOR.h>

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

int1 flag_flanco = 0;
int1 nueva_recepcion = 0;
float tiempo = 0;
float T_AP = 0;


#INT_EXT
void  EXT_isr(void) 
{
   if(flag_flanco == 0)
      {
         set_timer0(0);  // empieza a contar cada tic de 32us
         ext_int_edge(T0_EXT_H_TO_L);  // flanco de alto a bajo
         flag_flanco = 1;
      }
   else if (flag_flanco == 1)
   {
         tiempo = get_timer0();   // adquiere valor entre 0 y 255
         ext_int_edge(T0_EXT_L_TO_H);  //flanco de bajo a alto
         flag_flanco = 0;
         if (nueva_recepcion == 0)  
         {nueva_recepcion = 1;}  // activa la recepcion para recibir valores
   }
}




#INT_RTCC
void  RTCC_isr(void) 
{

}

void main()
{
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_32|RTCC_8_BIT);      
   set_timer0(6);                  // cada tic de 32us // desborda en 8ms // 

   ext_int_edge(T0_EXT_L_TO_H);
   enable_interrupts(INT_EXT);
   enable_interrupts(GLOBAL);
   flag_flanco=0;    // por default flanco de subida
   
   nueva_recepcion = 0;  // por default no hay recepcion
   

   while(TRUE)
   {
         if (nueva_recepcion == 1)
         {
               T_AP = (tiempo * (8./250.));   ///Ancho de pulso en milisegundos
               
               if((T_AP > 0) && (T_AP < 2.2))   // bit de inicio
               {
                  
               }
         }
         nueva_recepcion = 0;
   }

}
