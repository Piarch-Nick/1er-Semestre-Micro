#include <EVALUACION SIGUE-LINEA.h>

//// configuraciones ////
#fuses nowdt    //desactiva perro guardian. es un temporizador que reinicia el microcontrolador si detecta que el programa se ha quedado atascado en un bucle infinito
#fuses hs         // para configurar el pic a frecuencias de clk mas de 8M Hz. 
#fuses noprotect  //permite que el código almacenado en el microcontrolador pueda ser leído por dispositivos externos, lo cual puede ser útil durante el desarrollo y la depuración, pero no es recomendable para productos finales donde se desea proteger la propiedad intelectual.
//#fuses nomclr     //El fusible NOMCLR desactiva la función de Master Clear (MCLR) del microcontrolador. Cuando NOMCLR está activado, el pin MCLR se convierte en una entrada digital normal en lugar de ser utilizado para reiniciar el microcontrolador1. Esto puede ser útil si necesitas más pines de entrada/salida y no necesitas la función de reinicio externo.
//#fuses novregen   // se utiliza para desactivar la función de Voltage Regulator Enable (VREGEN) en algunos microcontroladores PIC. Esta función controla el regulador de voltaje interno del microcontrolador.. Al usar #FUSES NOVREGEN, se desactiva esta función, lo que puede ser útil en situaciones donde se utiliza una fuente de alimentación externa regulada o cuando se desea reducir el consumo de energía
////

// Manejo de Puertos //
#use standard_io(a)
#use standard_io(b)
#use standard_io(c)
#use standard_io(d)
////

//// prototipos de funciones 
////
void rampa_creciente_derecha();
void rampa_decreciente_derecha();

void rampa_creciente_izquierda();
void rampa_decreciente_izquierda();

void verificar_choque();
////

//// inicializacion ////
int count1=0;
int duty_derecha=0;
int duty_izquierda=0;

// banderas creadas para el control del flujo:
short flag_derecha=0;
short flag_izquierda=0;
short choque_rueda_derecha=0;
short choque_rueda_izquierda=0;
short retroceder_derecha = 0;
short retroceder_izquierda = 0;

int count_buzzer = 0;
int f_buzzer = 0;
/////////////

#INT_EXT
void  EXT_isr(void)  // ENTRA CUANDO CHOCA ALGUNO DE LOS 2 SENSORES DE CHOQUE (CHOCA ==> 1)
{
      disable_interrupts(Global);

      if (flag_derecha == 1)
      {
            choque_rueda_derecha = 1;     // si choquo con la rueda derecha encendida se levanta bandera
      }
      else if (flag_izquierda == 1)
      {
            choque_rueda_izquierda = 1;   // si choquo con la rueda izquierda encendida se levanta bandera
      }
      
      enable_interrupts(global);
      enable_interrupts(INT_TIMER0);
      enable_interrupts(INT_EXT);
}


#INT_TIMER0
void  TIMER0_isr(void)   // entra cada 6us
{
      disable_interrupts(Global);
      
      if(count1 == 50)      //perido 300us  // f=3.3khz
      { 
      /////////////////////
               
               if(retroceder_derecha == 1)
                  {
                         output_bit(pin_b2,1);   // RETROCEDE_RUEDA_DERECHA
                         output_bit(pin_b1,0);
                  }
               else
               {
                output_bit(pin_b2,0);
                output_bit(pin_b1,1);  // AVANCE_RUEDA_DERECHA. POR DEFAULT
               }
     ////////////////////////////
     ////////////////////////////
               if(retroceder_izquierda == 1)
                  {
                         output_bit(pin_d7,1);   // RETROCEDE_RUEDA_IZQUIERDA
                         output_bit(pin_d6,0);
                  }
               else
               {
                output_bit(pin_d7,0);
                output_bit(pin_d6,1);  // AVANCE_RUEDA_IZQUIERDA.  POR DEFAULT
               }
     ///////////////////////////
            count1=0;
      
      
      }
      else if (count1 < 50)              // duty==>   (50<-->100%) (25<-->50%) (10<-->50%) 
      {
             if(count1 == duty_derecha)
             {
             ///////////////////
                         output_bit(pin_b2,0);   // RETROCEDE_RUEDA_DERECHA
                         output_bit(pin_b1,0);   // AVANCE_RUEDA_DERECHA
              }
             ////////////////
             
             if(count1 == duty_izquierda)
             {
             /////////////////// 
                         output_bit(pin_d7,0);   // RETROCEDE_RUEDA_IZQUIERDA
                         output_bit(pin_d6,0);   // AVANCE_RUEDA_IZQUIERDA
               }
             ////////////////
             
              count1++;
      }
      ///////////////////////////////////BUZZER/////////////////////////
      /////////////////////////////////////////////////////////////
      if (count_buzzer == f_buzzer)      // 20 (8 khz) < f_buzzer < 500 (300 hz)
      {
            output_bit(pin_d0,1);
            count_buzzer = 0;
      }
      else if (count_buzzer < f_buzzer)
      {
            if(count_buzzer == f_buzzer/2)
            {
                   output_bit(pin_d0,0);
            }
            count_buzzer++;
      }
      
      
      
      enable_interrupts(global);
      enable_interrupts(INT_TIMER0);
      enable_interrupts(INT_EXT);
      set_timer0(250); //ajustar
       
}

void main()
{
   // INICIALIZAR SALIDAS //
output_bit(pin_d2,0);   // ENABLE_RUEDA_IZQUIERDA
output_bit(pin_d6,0);   // AVANCE_RUEDA_IZQUIERDA
output_bit(pin_d7,0);   // RETROCESO_RUEDA_IZQUIERDA

output_bit(pin_d3,0);   // ENABLE_RUEDA_DERECHA
output_bit(pin_b1,0);   // AVANCE_RUEDA_DERECHA
output_bit(pin_b2,0);   // RETROCESO_RUEDA_DERECHA

output_bit(pin_d1,0);   // BUZZER_RUEDA_IZQUIERDA
output_bit(pin_d0,0);   // BUZZER_RUEDA_DERECHA
////

   //// NOMENCLATURA ENTRADAS ////
// PIN_A1    ===>>    ENTRADA SENSOR INFRERROJO
// PIN_B0    ===>>    ENTRADA INTERRUPCION EXTERNA
   //////////////////////////////
   
   
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1|RTCC_8_BIT);
   set_timer0(250);                  // cada tic de 1us // desborda en 6us // 

   enable_interrupts(INT_TIMER0);
   enable_interrupts(GLOBAL);
 //  disable_interrupts(Global); // no queremos el pwm por ahora...
  //aqui comienzan los dos pwm por las dos ruedas simultanemente....
      

   enable_interrupts(INT_EXT);
   enable_interrupts(GLOBAL);


   while(TRUE)
   {                                // LOGICA NORMAL == LINEA_NEGRA SE PONE A 1
     
         ////////////////////////////////GIRO_RUEDA_DERECHA/////////////////////////////
      while(input(pin_a1) == 0)     // mientras el sensor este desactivado, mueve la rueda derecha hasta activar
            {
                  flag_derecha = 1;
                  flag_izquierda =0;
                  verificar_choque();     // verifica a ver si se levanto el bit de choque
                  output_bit(pin_d3,1);   // ENABLE_RUEDA_DERECHA
                  retroceder_derecha = 0;  //SE GARANTIZA QUE LA RUEDA GIRE HACIA ADELANTE
                  f_buzzer = 100; ////// colocamos 1.6 khz mientras gira a la derecha
                  rampa_creciente_derecha();
            }
      
      while(input(pin_a1) == 1)     // mientras el sensor este activado, disminuye la velocidad de rueda derecha
            {
                  verificar_choque();
                  rampa_decreciente_derecha();
            }
      output_bit(pin_d3,0);   // DISABLE_RUEDA_DERECHA 
      delay_ms(100);  // tiempo de inercia para la rueda derecha
      duty_derecha = 0;    // LIMPIAR EL DUTY_DERECHA PARA QUE VUELVA A COMENZAR EN 0;
      
      
         ////////////////////////////////GIRO_RUEDA_IZQUIERDA/////////////////////////////    
      while(input(pin_a1) == 0)     // mientras el sensor este desactivado, mueve la rueda izquierda hasta activar
            {
                  flag_derecha = 0;
                  flag_izquierda =1;           
                  verificar_choque();
                  output_bit(pin_d2,1);      // ENABLE_RUEDA_IZQUIERDA
                  retroceder_izquierda = 0;  //SE GARANTIZA QUE LA RUEDA GIRE HACIA ADELANTE
                  f_buzzer = 20; ////// colocamos 8.3 khz mientras gira a la izquierda
                  rampa_creciente_izquierda();
            }
      
      while(input(pin_a1) == 1)     // mientras el sensor este activado, disminuye la velocidad de rueda izquierda hasta desactivar...
            {
                  verificar_choque();
                  rampa_decreciente_izquierda();
            }
      output_bit(pin_d2,0);   // DISABLE_RUEDA_IZQUIERDA  
      delay_ms(100);  // tiempo de inercia para la rueda izquierda
      duty_izquierda = 0;    // LIMPIAR EL DUTY_IZQUIERDA PARA QUE VUELVA A COMENZAR EN 0;
            
    
   }

}



void rampa_creciente_derecha()
{
   if(duty_derecha == 25)     // SI LLEGA A 50% DEL DC% QUEDESE ALLI.
   {
         ;
   }
   else if (duty_derecha < 25)
   {
        duty_derecha++;
        delay_ms(50);
   }
}


void rampa_decreciente_derecha()
{
   if(duty_derecha == 25 || (duty_derecha > 0 && duty_derecha < 25))
   {
        duty_derecha--;
        delay_ms(1); 
   }
   else if (duty_derecha == 0) // SI LLEGA A 0% DEL DC% QUEDESE ALLI.
   {
        ;                  
   }
}


void rampa_creciente_izquierda()
{
   if(duty_izquierda == 25)
   {
         ;
   }
   else if (duty_izquierda < 25)
   {
        duty_izquierda++;
        delay_ms(50);
   }
}


void rampa_decreciente_izquierda()
{
    if(duty_izquierda == 25 || (duty_derecha > 0 && duty_derecha < 25))
   {
        duty_izquierda--;
        delay_ms(1); 
   }
   else if (duty_izquierda == 0)
   {
        ;
   }
}


void verificar_choque()    // si se levanta alguna bandera de choque, ejecuta giro hacia atras, dependiendo de la bandera.
{
 f_buzzer = 500; ////// colocamos 333.3 khz mientras retrocede
      if(choque_rueda_derecha == 1)
      {
           retroceder_derecha = 1;      //FLAG PARA QUE RETROCEDA LA RUEDA DERECHA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           retroceder_izquierda = 1;     //FLAG PARA QUE RETROCEDA LA RUEDA IZQUIERDA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           output_bit(pin_d3,0);   // DISABLE_RUEDA_DERECHA
           output_bit(pin_d2,0);   // DISABLE_RUEDA_IZQUIERDA
           delay_ms(1000);  // tiempo de estabilizacion
           
           ///////// RUTINA DE DETECTAR EL FINAL ///////////////////////////////
           
           retroceder_derecha = 1;  //FLAG PARA QUE RETROCEDA LA RUEDA DERECHA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           duty_derecha = 25;
           delay_us(300);
           output_bit(pin_d3,1);   // ENABLE_RUEDA_DERECHA
           delay_ms(400);          // RETROSEDE LA RUEDA DERECHA CON DUTY DE 50% POR 0.4s
           output_bit(pin_d3,0);   // DISABLE_RUEDA_DERECHA
            
           retroceder_izquierda = 0;   //FLAG PARA QUE ADELANTE LA RUEDA IZQUIERDA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           duty_izquierda = 25;
           delay_us(300);
           output_bit(pin_d2,1);   // ENABLE_RUEDA_IZQUIERDA
           delay_ms(400);          // ADELANTA LA RUEDA IZQUIERDA CON DUTY DE 50% POR 0.4s
           output_bit(pin_d2,0);   // DISABLE_RUEDA_IZQUIERDA
           
           
           
           retroceder_izquierda = 1;   //FLAG PARA QUE RETROSEDA LA RUEDA IZQUIERDA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           duty_izquierda = 25;
           delay_us(300);
           output_bit(pin_d2,1);   // ENABLE_RUEDA_IZQUIERDA
           delay_ms(500);          // RETROCEDE LA RUEDA IZQUIERDA CON DUTY DE 50% POR 0.5s
           output_bit(pin_d2,0);   // DISABLE_RUEDA_IZQUIERDA
           
           if (choque_rueda_derecha == 1 && choque_rueda_izquierda == 1) // si ha chocado dos veces
           {
               output_bit(pin_d3,0);   // DISABLE_RUEDA_DERECHA
                output_bit(pin_d2,0);   // DISABLE_RUEDA_IZQUIERDA
               while(1)
               {
                      //Buzzer(apagado, musica graciosa);         //   BUCLE INFINITO REPRODUCIENDO MUSICA
                      f_buzzer = 600; ////// colocamos 333.3 khz mientras retrocede
                      while(f_buzzer > 20)
                      {
                           f_buzzer--;
                           delay_ms(10);
                      }
               }
           }
           //////////////////////////////
           
           
           /////////RUTINA PARA MOVER HACIA ATRAS RUEDA DERECHA//////////
           
           retroceder_derecha = 1;      //FLAG PARA QUE RETROCEDA LA RUEDA DERECHA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           retroceder_izquierda = 1;     //FLAG PARA QUE RETROCEDA LA RUEDA IZQUIERDA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           output_bit(pin_d3,0);        // DISABLE_RUEDA_DERECHA
           output_bit(pin_d2,0);        // DISABLE_RUEDA_IZQUIERDA
           delay_ms(1000);              //TIEMPO ESTABILIZACION
           
           duty_derecha = 25;
           duty_izquierda = 25;
           delay_us(300);
           output_bit(pin_d3,1);   // ENABLE_RUEDA_DERECHA
           output_bit(pin_d2,1);   // ENABLE_RUEDA_IZQUIERDA
           delay_ms(1500);          // RETROSEDE LAS DOS RUEDAS CON DUTY DE 50% POR 1.5s
           output_bit(pin_d3,0);   // DISABLE_RUEDA_DERECHA
           output_bit(pin_d2,0);   // DISABLE_RUEDA_IZQUIERDA
           
           delay_us(300);
           output_bit(pin_d3,1);   // ENABLE_RUEDA_DERECHA 
           duty_derecha = 25;
           delay_ms(100);           //GIRA 100ms con el 50& de duty
           output_bit(pin_d3,0);    // DISABLE_RUEDA_DERECHA
           delay_ms(1000);           //TIEMPO ESTABILIZACION
           
           output_bit(pin_d3,0);   // DISABLE_RUEDA_DERECHA
           output_bit(pin_d2,0);   // DISABLE_RUEDA_IZQUIERDA
           //limpiar flags
           retroceder_derecha = 0;      //FLAG PARA QUE AVANCE LA RUEDA DERECHA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           retroceder_izquierda = 0;     //FLAG PARA QUE AVANCE LA RUEDA IZQUIERDA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           choque_rueda_derecha = 0;     //se borra esta bandera de choque para que no entre de nuevo aqui hasta otro choque.
          /////////
          
      }
      
        
      else if(choque_rueda_izquierda == 1)
      {
           retroceder_derecha = 1;      //FLAG PARA QUE RETROCEDA LA RUEDA DERECHA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           retroceder_izquierda = 1;     //FLAG PARA QUE RETROCEDA LA RUEDA IZQUIERDA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           output_bit(pin_d3,0);   // DISABLE_RUEDA_DERECHA
           output_bit(pin_d2,0);   // DISABLE_RUEDA_IZQUIERDA
           delay_ms(1000);  // tiempo de estabilizacion
           
            ///////// RUTINA DE DETECTAR EL FINAL ///////////////////////////////
           
           retroceder_izquierda = 1;  //FLAG PARA QUE RETROCEDA LA RUEDA IZQUIERDA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           duty_izquierda = 25;
           delay_us(300);
           output_bit(pin_d2,1);   // ENABLE_RUEDA_IZQUIERDA
           delay_ms(400);          // RETROSEDE LA RUEDA IZQUIERDA CON DUTY DE 50% POR 0.4s
           output_bit(pin_d2,0);   // DISABLE_RUEDA_IZQUIERDA
            
           retroceder_derecha = 0;   //FLAG PARA QUE ADELANTE LA RUEDA DERECHA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           duty_derecha = 25;
           delay_us(300);
           output_bit(pin_d3,1);   // ENABLE_RUEDA_DERECHA
           delay_ms(400);          // ADELANTA LA RUEDA IZQUIERDA CON DUTY DE 50% POR 0.4s
           output_bit(pin_d3,0);   // DISABLE_RUEDA_DERECHA
          
           
           retroceder_derecha = 1;   //FLAG PARA QUE RETROSEDA LA RUEDA DERECHA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           duty_derecha = 25;
           delay_us(300);
           output_bit(pin_d3,1);   // ENABLE_RUEDA_DERECHA
           delay_ms(500);          // RETROCEDE LA RUEDA DERECHA CON DUTY DE 50% POR 0.5s
           output_bit(pin_d3,0);   // DISABLE_RUEDA_DERECHA
           
            if (choque_rueda_derecha == 1 && choque_rueda_izquierda == 1) // si ha chocado dos veces
           {
               output_bit(pin_d3,0);   // DISABLE_RUEDA_DERECHA
                output_bit(pin_d2,0);   // DISABLE_RUEDA_IZQUIERDA
               while(1)
               {
                      //Buzzer(apagado, musica graciosa);         //   BUCLE INFINITO REPRODUCIENDO MUSICA
                        //Buzzer(apagado, musica graciosa);         //   BUCLE INFINITO REPRODUCIENDO MUSICA
                      f_buzzer = 600; ////// colocamos 333.3 khz mientras retrocede
                      while(f_buzzer > 20)
                      {
                           f_buzzer--;
                           delay_ms(10);
                      }
               }
           }
           //////////////////////////////
           
           
           /////////RUTINA DE GIRAR Y MOVER HACIA ATRAS RUEDA IZQUIERDA//////////
           
           output_bit(pin_d2,1);   // ENABLE_RUEDA_IZQUIERDA 
           duty_derecha = 25;
           delay_ms(400);           //GIRA 400ms con el 50& de duty
           output_bit(pin_d2,0);   // DISABLE_RUEDA_IZQUIERDA 
           delay_ms(1000);           //TIEMPO ESTABILIZACION
           
           duty_derecha = 25;
           duty_izquierda = 25;
           output_bit(pin_d3,1);   // ENABLE_RUEDA_DERECHA
           output_bit(pin_d2,1);   // ENABLE_RUEDA_IZQUIERDA
           delay_ms(1500);          // RETROSEDE LAS DOS RUEDAS CON DUTY DE 50% POR 1.5s
           output_bit(pin_d3,0);   // DISABLE_RUEDA_DERECHA
           output_bit(pin_d2,0);   // DISABLE_RUEDA_IZQUIERDA
           
           output_bit(pin_d2,1);   // ENABLE_RUEDA_IZQUIERDA
           duty_izquierda = 25;
           delay_ms(100);           //GIRA 100ms con el 50& de duty
           output_bit(pin_d2,0);    // DISABLE_RUEDA_IZQUIERDA
           delay_ms(1000);           //TIEMPO ESTABILIZACION
 
           output_bit(pin_d3,0);   // DISABLE_RUEDA_DERECHA
           output_bit(pin_d2,0);   // DISABLE_RUEDA_IZQUIERDA
           //limpiar flags
           retroceder_derecha = 0;      //FLAG PARA QUE AVANCE LA RUEDA DERECHA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           retroceder_izquierda = 0;     //FLAG PARA QUE AVANCE LA RUEDA IZQUIERDA(CAMBIA EL PWM HACIA EL OTRO LADO DEL PUENTE)
           choque_rueda_izquierda = 0;     //se borra esta bandera de choque para que no entre de nuevo aqui hasta otro choque.
          /////////
      }
}
