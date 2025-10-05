#include <prueba1.h>

//// configuraciones ////
#fuses nowdt    //desactiva perro guardian. es un temporizador que reinicia el microcontrolador si detecta que el programa se ha quedado atascado en un bucle infinito
#fuses xt         // para configurar el pic a frecuencias de clk mas de 8M Hz. 
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

////

//// inicializacion ////



///////////

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
   
   

   while(TRUE)
   {                                // LOGICA NORMAL == LINEA_NEGRA SE PONE A 1
     
         ////////////////////////////////GIRO_RUEDA_DERECHA/////////////////////////////
      while(input(pin_a1) == 0)     // mientras el sensor este desactivado, mueve la rueda derecha hasta activar
            {
                  output_bit(pin_d2,1);   // ENABLE_RUEDA_IZQUIERDA
                  output_bit(pin_d6,1);   // AVANCE_RUEDA_IZQUIERDA
                  output_bit(pin_d7,0);   // RETROCESO_RUEDA_IZQUIERDA

                  output_bit(pin_d3,0);   // ENABLE_RUEDA_DERECHA
                  output_bit(pin_b1,0);   // AVANCE_RUEDA_DERECHA
                  output_bit(pin_b2,0);   // RETROCESO_RUEDA_DERECHA
            }
      
      while(input(pin_a1) == 1)     // mientras el sensor este activado, disminuye la velocidad de rueda derecha
            {
                 output_bit(pin_d2,0);   // ENABLE_RUEDA_IZQUIERDA
                  output_bit(pin_d6,0);   // AVANCE_RUEDA_IZQUIERDA
                  output_bit(pin_d7,0);   // RETROCESO_RUEDA_IZQUIERDA

                  output_bit(pin_d3,1);   // ENABLE_RUEDA_DERECHA
                  output_bit(pin_b1,1);   // AVANCE_RUEDA_DERECHA
                  output_bit(pin_b2,0);   // RETROCESO_RUEDA_DERECHA
            }
     
}
}
