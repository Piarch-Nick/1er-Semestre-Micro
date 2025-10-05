#include <hexapodo_optimizado.h>
//// configuraciones ////
#fuses nowdt    //desactiva perro guardian. es un temporizador que reinicia el microcontrolador si detecta que el programa se ha quedado atascado en un bucle infinito
#fuses hs         // para configurar el pic a frecuencias de clk mas de 8M Hz. 
#fuses noprotect  //permite que el código almacenado en el microcontrolador pueda ser leído por dispositivos externos, lo cual puede ser útil durante el desarrollo y la depuración, pero no es recomendable para productos finales donde se desea proteger la propiedad intelectual.
//#fuses nomclr     //El fusible NOMCLR desactiva la función de Master Clear (MCLR) del microcontrolador. Cuando NOMCLR está activado, el pin MCLR se convierte en una entrada digital normal en lugar de ser utilizado para reiniciar el microcontrolador1. Esto puede ser útil si necesitas más pines de entrada/salida y no necesitas la función de reinicio externo.
//#fuses novregen   // se utiliza para desactivar la función de Voltage Regulator Enable (VREGEN) en algunos microcontroladores PIC. Esta función controla el regulador de voltaje interno del microcontrolador.. Al usar #FUSES NOVREGEN, se desactiva esta función, lo que puede ser útil en situaciones donde se utiliza una fuente de alimentación externa regulada o cuando se desea reducir el consumo de energía
//////////////////////////

// Manejo de Puertos //
#use standard_io(a)
#use standard_io(b)
#use standard_io(c)
//#use standard_io(d)
/////////////////////////


#define clk     PIN_A0
#define tx      PIN_A1
#define rx      PIN_A2
#define led     PIN_A3
#define led_0   PIN_A4
#define led_1   PIN_A5
// Potenciometro C0;
#define boton_pararse            PIN_C1
#define boton_esconderse         PIN_C2
#define boton_caminar_adelante   PIN_C3
#define boton_caminar_atras      PIN_C4
#define boton_saludo             PIN_C5
//// Prototipos de Funciones 
///////////////////////////////////////////
void initial ();
void elegir_estado_actual();    // maquina de mealy que, en funcion del estado actual y de las entradas decide cual es su estado futuro...
void ejecutar_estado_actual();
void mover_patas();
void ready_fpga();
void Velocidad();
void Errorr();
void saludar();
void velocidad_total();
///////////////////////////////////////////

////////////////// matriz[0]    matriz[1]
//int matriz[10] = {0b100111111 , 0b00000110 , 0b01011011 , 0b01001111 , 0b1100110 , 0b01101101 , 0b01111101 , 0b00000111 , 0b01111111 , 0b01101111};
//int matriz[3][4]; // Matriz de 3 filas y 4 columnas de enteros
int estado_actual = 0;
int1 ok = 0;
int1 flanco_subida=1;
int n_bits =0;
int v=3;
int z=11;
int1 bit_envio=0;
int pata=0;
int fila=0;   
int columna=0;
int16 adc= 0;
int1 aviso=0;
                    
                    //pata1     //pata2       //pata3     //pata4     //pata5     //pata6   //all servo  //all patas   //vel seguridad  //vel lenta  //vel media    //vel rapida  //READY        //PE
int comando[14] = {0b00000011 , 0b00000101, 0b00000111, 0b00001001, 0b00001011, 0b00001101, 0b00001111, 0b00010001,    0b00010011 ,   0b00010101,     0b00010111,    0b00011001,  0b00011011,  0b00011111};
                  //   0            1           2            3            4           5         6           7             8              9                10              11           12            13    
/*                  
int matrix[36][3] ={{0b01000110, 0b01101110, 0b01011111},  /// 3 BYTES DE GRADOS PARA PATA1  70grados,110grados,95grados.
                    {0b01000110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA2  70grados,110grados,85grados.
                    {0b01101110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA3  110grados,110grados,85grados.
                    {0b01000110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA4  70grados,110grados,85grados.
                    {0b01000110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA5  70grados,110grados,85grados.
                    {0b01101110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA6  110grados,110grados,85grados.
                   
                           ///////////MOVIMIENTO 2///////////////////
                    {0b01011010, 0b10010110, 0b10000010},  /// 3 BYTES DE GRADOS PARA PATA1  90grados,150grados,130grados.
                    {0b01010000, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA2  80grados,110grados,85grados.
                    {0b01011010, 0b10010110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA3  90grados,150grados,110grados.
                    {0b01010000, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA4  80grados,110grados,85grados.
                    {0b01011010, 0b10010110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA5  90grados,150grados,85grados.
                    {0b01100100, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA6  100grados,110grados,85grados.
                    
                          ///////////MOVIMIENTO 3///////////////////
                    {0b01101110, 0b01101110, 0b01011111},  /// 3 BYTES DE GRADOS PARA PATA1  110grados,110grados,95grados.
                    {0b01011010, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA2  90grados,110grados,85grados.
                    {0b01000110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA3  70grados,110grados,85grados.
                    {0b01011010, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA4  90grados,110grados,85grados.
                    {0b01101110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA5  110grados,110grados,85grados.
                    {0b01011010, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA6  90grados,110grados,85grados.
                    
                            ///////////MOVIMIENTO 4///////////////////
                    {0b01100100, 0b01101110, 0b01011111},  /// 3 BYTES DE GRADOS PARA PATA1  100grados,110grados,95grados.
                    {0b01100100, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA2  100grados,110grados,85grados.
                    {0b01010000, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA3  80grados,110grados,85grados.
                    {0b01100100, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA4  100grados,110grados,85grados.
                    {0b01100100, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA5  100grados,110grados,85grados.
                    {0b01010000, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA6  80grados,110grados,85grados.
                    
                             ///////////MOVIMIENTO 5///////////////////
                    {0b01011010, 0b01101110, 0b01011111},  /// 3 BYTES DE GRADOS PARA PATA1  90grados,110grados,95grados.
                    {0b01101110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA2  110grados,110grados,85grados.
                    {0b01011010, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA3  90grados,110grados,85grados.
                    {0b01101110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA4  110grados,110grados,85grados.
                    {0b01011010, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA5  90grados,110grados,85grados.
                    {0b01000110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA6  70grados,110grados,85grados.
                    
                           ///////////MOVIMIENTO 6///////////////////
                    {0b01010000, 0b01101110, 0b01011111},  /// 3 BYTES DE GRADOS PARA PATA1  80grados,110grados,95grados.
                    {0b01011010, 0b10010110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA2  90grados,150grados,110grados.
                    {0b01100100, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA3  100grados,110grados,85grados.
                    {0b01011010, 0b10010110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA4  90grados,150grados,110grados.
                    {0b01010000, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA5  80grados,110grados,85grados.
                    {0b01011010, 0b10010110, 0b01101110}};  /// 3 BYTES DE GRADOS PARA PATA6  90grados,150grados,110grados.
*/                    

                   /////////////////////////MOVIMIENTO 1////////////////////////////////////////////
int matrix[24][3] ={{0b01011111, 0b01000110, 0b01000110},  /// 3 BYTES DE GRADOS PARA PATA1  95grados,70grados,70grados.
                    {0b01111101, 0b01101110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA2  125grados,110grados,110grados.
                    {0b01000110, 0b01101110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA3  70grados,110grados,110grados.
                    {0b01011111, 0b01101110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA4  95grados,110grados,110grados.
                    {0b01111101, 0b01000110, 0b01000110},  /// 3 BYTES DE GRADOS PARA PATA5  125grados,70grados,70grados.
                    {0b01000110, 0b01000110, 0b01000110},  /// 3 BYTES DE GRADOS PARA PATA6  70grados,70grados,70grados.
                   
                           ///////////MOVIMIENTO 2///////////////////
                    {0b01001011, 0b00011110, 0b00110010},  /// 3 BYTES DE GRADOS PARA PATA1  75grados,30grados,50grados.
                    {0b01101001, 0b01101110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA2  105grados,110grados,110grados.
                    {0b01011010, 0b10010110, 0b10000010},  /// 3 BYTES DE GRADOS PARA PATA3  90grados,150grados,130grados.
                    {0b01001011, 0b01101110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA4  75grados,110grados,110grados.
                    {0b01101001, 0b00011110, 0b00110010},  /// 3 BYTES DE GRADOS PARA PATA5  105grados,30grados,50grados.
                    {0b01011010, 0b01000110, 0b01000110},  /// 3 BYTES DE GRADOS PARA PATA6  90grados,70grados,70grados.
                    
                          ///////////MOVIMIENTO 3///////////////////
                    {0b00110010, 0b01000110, 0b01000110},  /// 3 BYTES DE GRADOS PARA PATA1  50grados,70grados,70grados.
                    {0b01010101, 0b01101110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA2  85grados,110grados,110grados.
                    {0b01101110, 0b01101110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA3  110grados,110grados,110grados.
                    {0b00110010, 0b01101110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA4  50grados,110grados,110grados.
                    {0b01010101, 0b01000110, 0b01000110},  /// 3 BYTES DE GRADOS PARA PATA5  85grados,70grados,70grados.
                    {0b01101110, 0b01000110, 0b01000110},  /// 3 BYTES DE GRADOS PARA PATA6  110grados,70grados,70grados.
                    
                            ///////////MOVIMIENTO 4///////////////////
                    {0b01001011, 0b01000110, 0b01000110},  /// 3 BYTES DE GRADOS PARA PATA1  75grados,70grados,70grados.
                    {0b01101001, 0b10010110, 0b10000010},  /// 3 BYTES DE GRADOS PARA PATA2  105grados,150grados,130grados.
                    {0b01011010, 0b01101110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA3  90grados,110grados,110grados.
                    {0b01001011, 0b10010110, 0b10000010},  /// 3 BYTES DE GRADOS PARA PATA4  75grados,150grados,130grados.
                    {0b01101001, 0b01000110, 0b01000110},  /// 3 BYTES DE GRADOS PARA PATA5  105grados,70grados,70grados.
                    {0b01011010, 0b00011110, 0b00110010}};  /// 3 BYTES DE GRADOS PARA PATA6  90grados,30grados,50grados.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
                             
                   /////////////////////////MOVIMIENTO 1////////////////////////////////////////////
int saludo[12][3] ={{0b01011010, 0b01011010, 0b01011010},  /// 3 BYTES DE GRADOS PARA PATA1  90grados,90grados,90grados.
                    {0b00110010, 0b10011011, 0b00000000},  /// 3 BYTES DE GRADOS PARA PATA2  50grados,155grados,0grados.
                    {0b01011010, 0b01011010, 0b01011010},  /// 3 BYTES DE GRADOS PARA PATA3  90grados,90grados,90grados.
                    {0b01011010, 0b01011010, 0b01011010},  /// 3 BYTES DE GRADOS PARA PATA4  90grados,90grados,90grados.
                    {0b01011010, 0b01011010, 0b01011010},  /// 3 BYTES DE GRADOS PARA PATA5  90grados,90grados,90grados.
                    {0b01011010, 0b01011010, 0b01011010},  /// 3 BYTES DE GRADOS PARA PATA6  90grados,90grados,90grados.
                    
                    
                          ///////////MOVIMIENTO 2///////////////////
                    {0b01011010, 0b01011010, 0b01011010},  /// 3 BYTES DE GRADOS PARA PATA1  90grados,90grados,90grados.
                    {0b01100100, 0b10011011, 0b00000000},  /// 3 BYTES DE GRADOS PARA PATA2  100grados,155grados,0grados.
                    {0b01011010, 0b01011010, 0b01011010},  /// 3 BYTES DE GRADOS PARA PATA3  90grados,90grados,90grados.
                    {0b01011010, 0b01011010, 0b01011010},  /// 3 BYTES DE GRADOS PARA PATA4  90grados,90grados,90grados.
                    {0b01011010, 0b01011010, 0b01011010},  /// 3 BYTES DE GRADOS PARA PATA5  90grados,90grados,90grados.
                    {0b01011010, 0b01011010, 0b01011010}};  /// 3 BYTES DE GRADOS PARA PATA6  90grados,90grados,90grados.
                    
                    
                     
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                              
                             
                             
                             
                             
                             
                             
                             
                             
                             
                             
                              
int buffer_rx = 0;
int1 bit_rx = 0;
int1 error = 0;


#INT_TIMER0
void interrupt()   // cada 1ms entra
{
      if(flanco_subida == 0)
      {
            output_bit(clk,1);
            flanco_subida=1;
      }
      else
      {
            output_bit(clk,0);
            flanco_subida=0;
      }
    set_timer0(100);
}






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main()
{              
   setup_timer_0(RTCC_INTERNAL | RTCC_8_BIT |RTCC_DIV_16);
   set_timer0(100);                  // desborda en 500us        // cada tic es de 3.2us
                                   
 
   initial();      // inicializar variables
 
   while(true)
   {
      elegir_estado_actual();    // maquina de mealy que, en funcion del estado actual y de las entradas decide cual es su estado futuro...
      ejecutar_estado_actual();  // ejecuta el estado correspodiente..
    
   }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






void initial ()
{
       output_bit(clk,1);
       output_bit(tx,0);
       output_low(led);
       output_low(led_0);
       output_low(led_1);
}
  



void elegir_estado_actual()
{
  
            /// si estado_actual = 0 no hace nada....... puro ready y ok.
  
   if (estado_actual == 0  && ok==1 && input(boton_esconderse)==1)  // Si ha recivido el "ok" corectamente pasa all patas
      {                       
         estado_actual = 1;  
         ok=0;//limpiamos el flag
      }
      
  else if (estado_actual == 0 && ok==1 && input(boton_pararse) == 1)  // Si ha recivido el "ok" corectamente pasa all patas
      {                       
         estado_actual = 46;  
         ok=0;//limpiamos el flag
      }
      
  else if (estado_actual == 0  && ok==1 && input(boton_caminar_adelante) == 1)  // Si ha recivido el "ok" corectamente pasa all patas
      {                       
         estado_actual = 2;  // Si ha recivido el "ok" corectamente pasa a mandar velocidad
         ok=0;//limpiamos el flag
      }
  else if (estado_actual == 0  && ok==1 && input(boton_saludo) == 1)  // Si ha recivido el "ok" corectamente pasa all patas
      {                       
         estado_actual = 7;  // Si ha recivido el "ok" corectamente pasa a mandar velocidad
         ok=0;//limpiamos el flag
      }
 else if (estado_actual == 0  && ok==1 && input(boton_caminar_atras) == 1)  // Si ha recivido el "ok" corectamente pasa all patas
      {                       
         estado_actual = 47;  // Si ha recivido el "ok" corectamente pasa a mandar velocidad
         ok=0;//limpiamos el flag
      }
      
   else if (estado_actual == 0 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
         estado_actual = 10;  // ESTADO DE ERROR.  
      }
  
  
  
   else if (estado_actual == 1 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
         estado_actual = 10;  // ESTADO DE ERROR.  
      }
   else if (estado_actual == 1 && ok == 1)  // si se ha activado error haz lo que hace estado error
      {                       
         estado_actual = 0;  // ESTADO 0 
           ok=0;//limpiamos el flag
      }
      
    else if (estado_actual == 2 && ok == 1 && input(boton_esconderse) == 1)  // Si ha recivido el "ok" corectamente pasa a mandar pata1
      {                       
        estado_actual = 1;  
         ok=0;//limpiamos el flag
      }  
    
    else if (estado_actual == 2 && ok == 1 && input(boton_pararse) == 1)  // Si ha recivido el "ok" corectamente pasa a mandar pata1
      {                       
        estado_actual = 46;  
         ok=0;//limpiamos el flag
      }    
  else if (estado_actual == 2 && ok == 1 && input(boton_caminar_atras) == 1)  // Si ha recivido el "ok" corectamente pasa a mandar pata1
      {                       
        estado_actual = 47;  
         ok=0;//limpiamos el flag
      }   
   else if (estado_actual == 2  && ok==1 && input(boton_saludo) == 1)  // Si ha recivido el "ok" corectamente pasa all patas
      {                       
         estado_actual = 7;  // Si ha recivido el "ok" corectamente pasa a mandar velocidad
         ok=0;//limpiamos el flag
      }
      
   else if (estado_actual == 2 && ok == 1 && input(boton_esconderse) == 0 && input(boton_pararse) == 0 && input(boton_caminar_atras) == 0 && input(boton_saludo) == 0)  // Si ha recivido el "ok" corectamente pasa a mandar pata1
      {                       
        estado_actual = 3;  //caminar adelante
         ok=0;//limpiamos el flag
      }  
   else if (estado_actual == 2 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
         estado_actual = 10;  // ESTADO DE ERROR.  
      }
  
   else if (estado_actual == 3 && ok == 1)  // termino movimiento 1 de las 6 patas
      {                       
        estado_actual = 4;  
         ok=0;//limpiamos el flag
      }  
      
   else if (estado_actual == 4 && ok == 1) // termino movimiento 2 de las 6 patas
      {                       
        estado_actual = 5;  
         ok=0;//limpiamos el flag
      }  
      
   else if (estado_actual == 5 && ok == 1)  // termino movimiento 3 de las 6 patas
      {                       
        estado_actual = 6;  
         ok=0;//limpiamos el flag
      }  
     
   else if (estado_actual == 6 && ok == 1)  // termino movimiento 4 de las 6 patas
      {                       
        estado_actual = 2;    //velocidad bucle... pregunta botones.  
         ok=0;//limpiamos el flag
      }  
   else if (estado_actual == 6 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
         estado_actual = 10;  // ESTADO DE ERROR.  
      }



  else if (estado_actual == 7 && ok == 1 && input(boton_esconderse) == 1)  // Si ha recivido el "ok" corectamente pasa a mandar pata1
      {                       
        estado_actual = 1;   // esconderse
         ok=0;//limpiamos el flag
      }  
    
    else if (estado_actual == 7 && ok == 1 && input(boton_pararse) == 1)  // Si ha recivido el "ok" corectamente pasa a mandar pata1
      {                       
        estado_actual = 46;  // pararse
         ok=0;//limpiamos el flag
      } 
      
    else if (estado_actual == 7 && ok == 1 && input(boton_caminar_adelante) == 1)  // Si ha recivido el "ok" corectamente pasa a mandar pata1
      {                       
        estado_actual = 2;  // camina adelante
         ok=0;//limpiamos el flag
      } 
    
    else if (estado_actual == 7 && ok == 1 && input(boton_caminar_atras) == 1)  // Si ha recivido el "ok" corectamente pasa a mandar pata1
      {                       
        estado_actual = 47;  // camina adelante
         ok=0;//limpiamos el flag
      } 
      
   else if (estado_actual == 7 && ok == 1 && input(boton_esconderse) == 0 && input(boton_pararse) == 0 && input(boton_caminar_adelante) == 0 && input(boton_caminar_atras) == 0)  // Si ha recivido el "ok" corectamente pasa a mandar pata1
      {                       
        estado_actual = 8;  
         ok=0;//limpiamos el flag
      }  
   else if (estado_actual == 7 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
         estado_actual = 10;  // ESTADO DE ERROR.
}

else if (estado_actual == 8 && ok == 1) // termino movimiento 2 de las 6 patas
      {                       
        estado_actual = 9;  
         ok=0;//limpiamos el flag
      }  

else if (estado_actual == 9 && ok == 1) // termino movimiento 2 de las 6 patas
      {                       
        estado_actual = 7;  
         ok=0;//limpiamos el flag
      }  

/*
   else if (estado_actual == 7 && ok == 1)  // termino movimiento 5 de las 6 patas
      {                       
        estado_actual = 8;  
         ok=0;//limpiamos el flag
      }  

   else if (estado_actual == 8 && ok == 1) // termino movimiento 6 de las 6 patas
      {                       
        estado_actual = 2;    //velocidad bucle... pregunta botones.  
         ok=0;//limpiamos el flag
      }  
   else if (estado_actual == 8 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
         estado_actual = 10;  // ESTADO DE ERROR.  
      }
  */    
     
     
    
     
     
     
      
   else if(estado_actual == 46 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a ir al estado 0
      {               
         ok=0;   //limpiamos el flag
         estado_actual=0;
      }
   else if(estado_actual == 46 && error == 1)  // Si ha recivido el "ok" correctamente pasa a ir al estado 0
      {               
         estado_actual=10;
      }
     
     
     
     
     
     
     
     
     
     
     
      else if (estado_actual == 47 && ok == 1 && input(boton_esconderse) == 1)  // Si ha recivido el "ok" corectamente pasa a mandar pata1
      {                       
        estado_actual = 1;   // esconderse
         ok=0;//limpiamos el flag
      }  
    
    else if (estado_actual == 47 && ok == 1 && input(boton_pararse) == 1)  // Si ha recivido el "ok" corectamente pasa a mandar pata1
      {                       
        estado_actual = 46;  // pararse
         ok=0;//limpiamos el flag
      } 
      
    else if (estado_actual == 47 && ok == 1 && input(boton_caminar_adelante) == 1)  // Si ha recivido el "ok" corectamente pasa a mandar pata1
      {                       
        estado_actual = 2;  // camina adelante
         ok=0;//limpiamos el flag
      } 
    
    else if (estado_actual == 47 && ok == 1 && input(boton_saludo) == 1)  // Si ha recivido el "ok" corectamente pasa a mandar pata1
      {                       
        estado_actual = 7;  // camina adelante
         ok=0;//limpiamos el flag
      } 
      
   else if (estado_actual == 47 && ok == 1 && input(boton_esconderse) == 0 && input(boton_pararse) == 0 && input(boton_caminar_adelante) == 0 && input(boton_saludo) == 0)  // Si ha recivido el "ok" corectamente pasa a mandar pata1
      {                       
        estado_actual = 50;  
         ok=0;//limpiamos el flag
      }  
   else if (estado_actual == 47 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
         estado_actual = 10;  // ESTADO DE ERROR.
      }
     
     
       else if (estado_actual == 50 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 53;
      }
      /*
      else if (estado_actual == 51 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 52;
      }
      else if (estado_actual == 52 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 53;
      }
      */
      else if (estado_actual == 53 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 54;
      }
      else if (estado_actual == 54 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 55;
      }
      else if (estado_actual == 55 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 47;
      }
   
     
     
 else     
   {
   estado_actual = estado_actual;
   }
}



void ejecutar_estado_actual()
{
   switch (estado_actual)
      {
         case 0:   
                           
              //MANDAR COMANDO "READY"
              /////////////////////////////////////////////////////////////////////
                      enable_interrupts(GLOBAL);
                      enable_interrupts(INT_TIMER0);
                      delay_ms(5);
                         n_bits = 0;  //limpia el conteo
                         
                      for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = comando[12]>>k & 0b00000001 ;   //Enviamos los 8 bits de ready
                           output_bit(tx,bit_envio);                   //0b00011011
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
                      output_bit(tx,0);
                      n_bits = 0;  //limpia el conteo
                  
                 //////////ESPERAR 9 FLANCOS DE BAJADA DE CLK//////////////////////////////////////////////
                 ////////////////////////////////////////////////////////////////////////////////////
                       for (int k=0 ; k<9 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           n_bits = k+1;
                           delay_us(600);
                        }
                      }
                 ////////////////////////////////////////////////////////////////////////////////////////////
                 ////////////////////////////////////////////////////////////////////////////
             //   output_bit(tx,1);
                      n_bits = 0;  //limpia el conteo
                      
                      
                        while(flanco_subida == 1) // estabilizar
                           {;}
                           
                        while(flanco_subida == 0) // no leer flanco de bajada
                           {;}
      
               //            output_bit(tx,0);  
                           /////////////////////LEER EL "OK"///////////
                           //////////////////////////////////////////////
                        for (int k=0 ; k<8 ; k++)       /// llega justo al 10mo flanco, luego de 9 flancos de subida
                      {
                      
                        while(flanco_subida == 0)
                           {;}
                        if(flanco_subida == 1 && n_bits == k)
                        {
                           bit_rx = input(rx);
                              if(bit_rx == 1)
                              {
                                     buffer_rx = buffer_rx >> 1;           // desplazamos el byte
                                     buffer_rx = (buffer_rx & 0b01111111); // limpiar el bit MSB
                                     buffer_rx = (buffer_rx | 0b10000000); // colocar "1" en el bit MSB
                                     n_bits = k+1;
                                     delay_us(550);
                              }
                              else
                              {
                                     buffer_rx = buffer_rx >> 1;           // desplazamos el byte
                                     buffer_rx = (buffer_rx & 0b01111111); // limpiar el bit MSB
                                     n_bits = k+1;
                                     delay_us(550);
                              }                       
                        }
                      }
                      
                         n_bits = 0;  //limpia el conteo
                          // if(buffer_rx == 0b00000101)// recivo el ok
                        
                             if(buffer_rx == 0b00000101)
                             {
                                 ok=1;
                                error=0;
                             }
                           else 
                             {
                               error=1; 
                                ok=0;
                             }
           output_low(led_0);
                      ////////////////////////////////////////////////////////////////////////////////////
                      ////////////////////////////////////////////////////////////////////////////////
                  break;
       
       
       
         
                  
         case 1:        //ALL PATAS PARA ESCONDERSE
       //  output_high(led_0);
       
                                //MANDAR COMANDO "ALL PATAS"
              /////////////////////////////////////////////////////////////////////
               n_bits = 0;  //limpia el conteo
               while(flanco_subida == 0) // no leer flanco de bajada
                           {;}
                      for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = comando[7]>>k & 0b00000001 ;  
                           output_bit(tx,bit_envio);                   //0b00010001,
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
                      
           /////////////////////////////////////////////////////////////////////////////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////////////////////
           ////////////////ENVIAR GRADO DEL MOTOR 1////////////////////////////////////////////////
           /////////////////////////////////////////////////////////////////////////////////////////
                      n_bits = 0;  //limpia el conteo
                   for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = 0b01011010>>k & 0b00000001 ;   //Enviamos los 90grados en binario MOTOR 1
                           output_bit(tx,bit_envio);
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
                  /////////////////////////////////////////////////////////////////////////////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////////////////////
           ////////////////ENVIAR GRADO DEL MOTOR 2////////////////////////////////////////////////
           /////////////////////////////////////////////////////////////////////////////////////////     
                         n_bits = 0;  //limpia el conteo
                   for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = 0b10101010>>k & 0b00000001 ;   //Enviamos los 170grados en binario MOTOR 2
                           output_bit(tx,bit_envio);
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
               /////////////////////////////////////////////////////////////////////////////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////////////////////
           ////////////////ENVIAR GRADO DEL MOTOR 3////////////////////////////////////////////////
           /////////////////////////////////////////////////////////////////////////////////////////        
                             n_bits = 0;  //limpia el conteo
                   for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = 0b10100101>>k & 0b00000001 ;   //Enviamos 170grados en binario MOTOR 3
                           output_bit(tx,bit_envio);
                           n_bits = k+1;
                           delay_us(500);
                        }
                        else 
                        {
                          ;
                        }  
                      }
                      
                          output_bit(tx,0);
                      n_bits = 0;  //limpia el conteo
                      
           ///////////////////////////////////////////////////////////////////////////////
           //////////////////ESPERAR HATA QUE FPGA RESPONDA///////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////
           
            aviso=0;
                     while(aviso == 0)///////////bucle hasta que detecte un alto en rx.
                     {
                      if (flanco_subida == 1)
                      {
                           if (input(rx) == 1)
                           {aviso = 1;}
                           else{;}
                      }
                      else if (flanco_subida ==0)
                      {;}
                     }
                      
                        /////////////////////LEER EL "OK"///////////
                           //////////////////////////////////////////////
                            n_bits = 0;  //limpia el conteo
                        for (int k=0 ; k<8 ; k++)
                      {
                      
                        while(flanco_subida == 0)
                           {;}
                        if(flanco_subida == 1 && n_bits == k)
                        {
                           bit_rx = input(rx);
                              if(bit_rx == 1)
                              {
                                     buffer_rx = buffer_rx >> 1;           // desplazamos el byte
                                     buffer_rx = (buffer_rx & 0b01111111); // limpiar el bit MSB
                                     buffer_rx = (buffer_rx | 0b10000000); // colocar "1" en el bit MSB
                                     n_bits = k+1;
                                     delay_us(550);
                              }
                              else
                              {
                                     buffer_rx = buffer_rx >> 1;           // desplazamos el byte
                                     buffer_rx = (buffer_rx & 0b01111111); // limpiar el bit MSB
                                     n_bits = k+1;
                                     delay_us(550);
                              }                       
                        }
                      }
                      
                         n_bits = 0;  //limpia el conteo
                           //  if(buffer_rx == 0b00000101)
                           if(buffer_rx == 0b00000101)
                             {
                                 ok=1;
                             }
                             else
                             {
                                 error = 1; 
                             }
                         
                      ////////////////////////////////////////////////////////////////////////////////////
                      ////////////////////////////////////////////////////////////////////////////////
           break;   
           
        
        case 2:  ///VELOCIDAD
          velocidad_total();
                      
                  break;
        
                 case 3:      //MOVIMIENTO 1
              /////////////////////////////////////////////////////
              ////////////BUCLE PARA CAMINAR///////////////////
              ////////////////////////////////////////////////////
                 for(int p=0 ; p<6 ; p++)
     {
           ///////////PATA 1,2,3,4,5,6////////////////////
           /////////////////////////////////////
                     
                      delay_ms(5);
                      pata=p;
                      fila=p;
                      columna=0;
                      mover_patas();
                       if(error == 1)
                       {Errorr();}
     }
     
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////''OK''
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                   ok=0;//limpiamos el flag
                  ready_fpga();
                  
                  break;            
                        
           /////////////////////////////////////////////////////////////////
           /////////////////ERROR///////////////////////////////////////////////
           //////////////////////////////////////////////////////////////////////
        case 10:  
                        output_high(pin_a3);
                        break;                   
    //////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
           
           
                      
       case 4:      //MOVIMIENTO 2
              /////////////////////////////////////////////////////
              ////////////BUCLE PARA CAMINAR///////////////////
              ////////////////////////////////////////////////////
                 for(int p=6 ; p<12 ; p++)
     {
           ///////////PATA 1,2,3,4,5,6////////////////////
           /////////////////////////////////////
                     
                      delay_ms(5);
                      pata=p-6;
                      fila=p;
                      columna=0;
                      mover_patas();
                       if(error == 1)
                       {Errorr();}
     }
     
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////''OK''
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                     ok=0;//limpiamos el flag
                  ready_fpga();
                  
                  break;                   
       
     
       case 5:      //MOVIMIENTO 3
              /////////////////////////////////////////////////////
              ////////////BUCLE PARA CAMINAR///////////////////
              ////////////////////////////////////////////////////
                 for(int p=12 ; p<18 ; p++)
     {
           ///////////PATA 1,2,3,4,5,6////////////////////
           /////////////////////////////////////
                     
                      delay_ms(5);
                      pata=p-12;
                      fila=p;
                      columna=0;
                      mover_patas();
                       if(error == 1)
                       {Errorr();}
     }
     
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////''OK''
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                     ok=0;//limpiamos el flag
                  ready_fpga();
                  
                  break;                      
      
                 
                 
      case 6:      //MOVIMIENTO 4
              /////////////////////////////////////////////////////
              ////////////BUCLE PARA CAMINAR///////////////////
              ////////////////////////////////////////////////////
                 for(int p=18 ; p<24 ; p++)
     {
           ///////////PATA 1,2,3,4,5,6////////////////////
           /////////////////////////////////////
                     
                      delay_ms(5);
                      pata=p-18;
                      fila=p;
                      columna=0;
                      mover_patas();
                       if(error == 1)
                       {Errorr();}
     }
     
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////''OK''
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                    ok=0;//limpiamos el flag
                  ready_fpga();
                  
                  break;                   
     
     
     
     
     
     
     
     
     
         case 7:  ///VELOCIDAD
           velocidad_total();
                      
                  break;
        
                 case 8:      //MOVIMIENTO 1
              /////////////////////////////////////////////////////
              ////////////BUCLE PARA SALUDAR///////////////////
              ////////////////////////////////////////////////////
                 for(int p=0 ; p<6 ; p++)
     {
           ///////////PATA 1,2,3,4,5,6////////////////////
           /////////////////////////////////////
                     
                      delay_ms(5);
                      pata=p;
                      fila=p;
                      columna=0;
                     saludar();
                       if(error == 1)
                       {Errorr();}
     }
     
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////''OK''
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                   ok=0;//limpiamos el flag
                  ready_fpga();
                  
                  break;            
                        
     
           
           
                      
       case 9:      //MOVIMIENTO 2
              /////////////////////////////////////////////////////
              ////////////BUCLE PARA SALUDAR///////////////////
              ////////////////////////////////////////////////////
                 for(int p=6 ; p<12 ; p++)
     {
           ///////////PATA 1,2,3,4,5,6////////////////////
           /////////////////////////////////////
                     
                      delay_ms(5);
                      pata=p-6;
                      fila=p;
                      columna=0;
                      saludar();
                       if(error == 1)
                       {Errorr();}
     }
     
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////''OK''
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                     ok=0;//limpiamos el flag
                  ready_fpga();
                  
                  break;                   
        
     
     
     
     
     
     
     
      /*
     case 7:      //MOVIMIENTO 5
              /////////////////////////////////////////////////////
              ////////////BUCLE PARA CAMINAR///////////////////
              ////////////////////////////////////////////////////
                 for(int p=24 ; p<30 ; p++)
     {
           ///////////PATA 1,2,3,4,5,6////////////////////
           /////////////////////////////////////
                     
                      delay_ms(5);
                      pata=p-24;
                      fila=p;
                      columna=0;
                      mover_patas();
                       if(error == 1)
                       {Errorr();}
     }
     
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////''OK''
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                     ok=0;//limpiamos el flag
                  ready_fpga();
                  
                  break;      
                  
    case 8:      //MOVIMIENTO 6
              /////////////////////////////////////////////////////
              ////////////BUCLE PARA CAMINAR///////////////////
              ////////////////////////////////////////////////////
                 for(int p=30 ; p<36 ; p++)
     {
           ///////////PATA 1,2,3,4,5,6////////////////////
           /////////////////////////////////////
                     
                      delay_ms(5);
                      pata=p-30;
                      fila=p;
                      columna=0;
                      mover_patas();
                       if(error == 1)
                       {Errorr();}
     }
     
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////''OK''
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                    ok=0;//limpiamos el flag
                  ready_fpga();
                  
                  break;                   
    
       */
                        
          case 46:         //ALL PATAS PARA PARARSE
         
       
                                //MANDAR COMANDO "ALL PATAS"
              /////////////////////////////////////////////////////////////////////
               n_bits = 0;  //limpia el conteo
               while(flanco_subida == 0) // no leer flanco de bajada
                           {;}
                      for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = comando[7]>>k & 0b00000001 ;  
                           output_bit(tx,bit_envio);                   //0b00010001,
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
                      
           /////////////////////////////////////////////////////////////////////////////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////////////////////
           ////////////////ENVIAR GRADO DEL MOTOR 1////////////////////////////////////////////////
           /////////////////////////////////////////////////////////////////////////////////////////
                      n_bits = 0;  //limpia el conteo
                   for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = 0b01011010>>k & 0b00000001 ;   //Enviamos los 90grados en binario MOTOR 1
                           output_bit(tx,bit_envio);
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
                  /////////////////////////////////////////////////////////////////////////////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////////////////////
           ////////////////ENVIAR GRADO DEL MOTOR 2////////////////////////////////////////////////
           /////////////////////////////////////////////////////////////////////////////////////////     
                         n_bits = 0;  //limpia el conteo
                   for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = 0b01011010>>k & 0b00000001 ;   //Enviamos los 90grados en binario MOTOR 2
                           output_bit(tx,bit_envio);
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
               /////////////////////////////////////////////////////////////////////////////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////////////////////
           ////////////////ENVIAR GRADO DEL MOTOR 3////////////////////////////////////////////////
           /////////////////////////////////////////////////////////////////////////////////////////        
                             n_bits = 0;  //limpia el conteo
                   for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = 0b01011010>>k & 0b00000001 ;   //Enviamos 90grados en binario MOTOR 3
                           output_bit(tx,bit_envio);
                           n_bits = k+1;
                           delay_us(500);
                        }
                        else 
                        {
                          ;
                        }  
                      }
                      
                      
                      
           ///////////////////////////////////////////////////////////////////////////////
           //////////////////ESPERAR HATA QUE FPGA RESPONDA///////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////
           
            int1 aviso=0;
                     while(aviso == 0)///////////bucle hasta que detecte un alto en rx.
                     {
                      if (flanco_subida == 1)
                      {
                           if (input(rx) == 1)
                           {aviso = 1;}
                           else{;}
                      }
                      else if (flanco_subida ==0)
                      {;}
                     }
                      
                        /////////////////////LEER EL "OK"///////////
                           //////////////////////////////////////////////
                            n_bits = 0;  //limpia el conteo
                        for (int k=0 ; k<8 ; k++)
                      {
                      
                        while(flanco_subida == 0)
                           {;}
                        if(flanco_subida == 1 && n_bits == k)
                        {
                           bit_rx = input(rx);
                              if(bit_rx == 1)
                              {
                                     buffer_rx = buffer_rx >> 1;           // desplazamos el byte
                                     buffer_rx = (buffer_rx & 0b01111111); // limpiar el bit MSB
                                     buffer_rx = (buffer_rx | 0b10000000); // colocar "1" en el bit MSB
                                     n_bits = k+1;
                                     delay_us(550);
                              }
                              else
                              {
                                     buffer_rx = buffer_rx >> 1;           // desplazamos el byte
                                     buffer_rx = (buffer_rx & 0b01111111); // limpiar el bit MSB
                                     n_bits = k+1;
                                     delay_us(550);
                              }                       
                        }
                      }
                      
                         n_bits = 0;  //limpia el conteo
                           //  if(buffer_rx == 0b00000101)
                           if(buffer_rx == 0b00000101)
                             {
                                 ok=1;
                             }
                             else
                             {
                                 error = 1; 
                             }
                         
                      ////////////////////////////////////////////////////////////////////////////////////
                      ////////////////////////////////////////////////////////////////////////////////
           break;   
                         
                        
       
                
     case 47:  ///VELOCIDAD
           
           velocidad_total();
                      
                  break;
                
      
        ////////////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////MOVER PARA ATRAS////////////////////////////////////////////////////////////
      //////////////////////////////////////////////////////////////////////////////////////////////////
         
     case 50:      //MOVIMIENTO 1
              /////////////////////////////////////////////////////
              ////////////BUCLE PARA CAMINAR///////////////////
              ////////////////////////////////////////////////////
                 for(int p=0 ; p<6 ; p++)
     {
           ///////////PATA 1,2,3,4,5,6////////////////////
           /////////////////////////////////////
                     
                      delay_ms(5);
                      pata=p;
                      fila=p;
                      columna=0;
                      mover_patas();
                       if(error == 1)
                       {Errorr();}
     }
     
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////''OK''
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                   ok=0;//limpiamos el flag
                  ready_fpga();
                  
                  break;            
          
      
   /*
      case 51:      //MOVIMIENTO 2
              /////////////////////////////////////////////////////
              ////////////BUCLE PARA CAMINAR///////////////////
              ////////////////////////////////////////////////////
                 for(int p=30 ; p<36 ; p++)
     {
           ///////////PATA 1,2,3,4,5,6////////////////////
           /////////////////////////////////////
                     
                      delay_ms(5);
                      pata=p-30;
                      fila=p;
                      columna=0;
                      mover_patas();
                       if(error == 1)
                       {Errorr();}
     }
     
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////''OK''
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                   ok=0;//limpiamos el flag
                  ready_fpga();
                  
                  break;            
      
    
      case 52:      //MOVIMIENTO 3
              /////////////////////////////////////////////////////
              ////////////BUCLE PARA CAMINAR///////////////////
              ////////////////////////////////////////////////////
                 for(int p=24 ; p<30 ; p++)
     {
           ///////////PATA 1,2,3,4,5,6////////////////////
           /////////////////////////////////////
                     
                      delay_ms(5);
                      pata=p-24;
                      fila=p;
                      columna=0;
                      mover_patas();
                       if(error == 1)
                       {Errorr();}
     }
     
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////''OK''
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                   ok=0;//limpiamos el flag
                  ready_fpga();
                  
                  break;            
 */    
                    
       case 53:      //MOVIMIENTO 4
              /////////////////////////////////////////////////////
              ////////////BUCLE PARA CAMINAR///////////////////
              ////////////////////////////////////////////////////
                 for(int p=18 ; p<24 ; p++)
     {
           ///////////PATA 1,2,3,4,5,6////////////////////
           /////////////////////////////////////
                     
                      delay_ms(5);
                      pata=p-18;
                      fila=p;
                      columna=0;
                      mover_patas();
                       if(error == 1)
                       {Errorr();}
     }
     
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////''OK''
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                   ok=0;//limpiamos el flag
                  ready_fpga();
                  
                  break;            
 
   
   case 54:      //MOVIMIENTO 5
              /////////////////////////////////////////////////////
              ////////////BUCLE PARA CAMINAR///////////////////
              ////////////////////////////////////////////////////
                 for(int p=12 ; p<18 ; p++)
     {
           ///////////PATA 1,2,3,4,5,6////////////////////
           /////////////////////////////////////
                     
                      delay_ms(5);
                      pata=p-12;
                      fila=p;
                      columna=0;
                      mover_patas();
                       if(error == 1)
                       {Errorr();}
     }
     
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////''OK''
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                   ok=0;//limpiamos el flag
                  ready_fpga();
                  
                  break;            
     
  case 55:      //MOVIMIENTO 6
              /////////////////////////////////////////////////////
              ////////////BUCLE PARA CAMINAR///////////////////
              ////////////////////////////////////////////////////
                 for(int p=6 ; p<12 ; p++)
     {
           ///////////PATA 1,2,3,4,5,6////////////////////
           /////////////////////////////////////
                     
                      delay_ms(5);
                      pata=p-6;
                      fila=p;
                      columna=0;
                      mover_patas();
                       if(error == 1)
                       {Errorr();}
     }
     
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////''OK''
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                   ok=0;//limpiamos el flag
                  ready_fpga();
                  
                  break;            
     
                       
                        
         default:
             {output_high(pin_a3);}
      }
}





    
void mover_patas()
{
          
              /////////////////////////////////////////////////////////////////////
               n_bits = 0;  //limpia el conteo
               while(flanco_subida == 0) // no leer flanco de bajada
                           {;}
                      for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = comando[pata]>>k & 0b00000001 ;  
                           output_bit(tx,bit_envio);                  // 0b00000101
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
                      
           /////////////////////////////////////////////////////////////////////////////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////////////////////
           ////////////////ENVIAR GRADO DEL MOTOR 1////////////////////////////////////////////////
           /////////////////////////////////////////////////////////////////////////////////////////
                      n_bits = 0;  //limpia el conteo
                     columna = 0;
                      
                   for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = matrix[fila][columna]>>k & 0b00000001 ;   //Enviamos grados en binario MOTOR 1
                           output_bit(tx,bit_envio);
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
                  /////////////////////////////////////////////////////////////////////////////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////////////////////
           ////////////////ENVIAR GRADO DEL MOTOR 2////////////////////////////////////////////////
           /////////////////////////////////////////////////////////////////////////////////////////     
                         n_bits = 0;  //limpia el conteo
                         columna = 1;
                   for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = matrix[fila][columna]>>k & 0b00000001 ;   //Enviamos grados en binario MOTOR 2
                           output_bit(tx,bit_envio);
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
               /////////////////////////////////////////////////////////////////////////////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////////////////////
           ////////////////ENVIAR GRADO DEL MOTOR 3////////////////////////////////////////////////
           /////////////////////////////////////////////////////////////////////////////////////////        
                             n_bits = 0;  //limpia el conteo
                             columna = 2;
                   for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                         bit_envio = matrix[fila][columna]>>k & 0b00000001 ;   //Enviamos grados en binario MOTOR 1
                           output_bit(tx,bit_envio);
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
                      n_bits = 0;  //limpia el conteo
                       output_bit(tx,0);
                  
                 //////////ESPERAR 9 FLANCOS DE BAJADA DE CLK//////////////////////////////////////////////
                 ////////////////////////////////////////////////////////////////////////////////////
                       for (int k=0 ; k<9 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           n_bits = k+1;
                           delay_us(550);
                        }
                      }
                 ////////////////////////////////////////////////////////////////////////////////////////////
                 ////////////////////////////////////////////////////////////////////////////
                
                      n_bits = 0;  //limpia el conteo
                      
                      
                        while(flanco_subida == 1) // estabilizar
                           {;}
                           
                        while(flanco_subida == 0) // no leer flanco de bajada
                           {;}
      
                           
                           /////////////////////LEER EL "OK"///////////
                           //////////////////////////////////////////////
                        for (int k=0 ; k<8 ; k++)
                      {
                      
                        while(flanco_subida == 0)
                           {;}
                       //    output_bit(tx,1);
                        if(flanco_subida == 1 && n_bits == k)
                        {
                        
                           bit_rx = input(rx);
                              if(bit_rx == 1)
                              {
                                     buffer_rx = buffer_rx >> 1;           // desplazamos el byte
                                     buffer_rx = (buffer_rx & 0b01111111); // limpiar el bit MSB
                                     buffer_rx = (buffer_rx | 0b10000000); // colocar "1" en el bit MSB
                                     n_bits = k+1;
                                     delay_us(550);
                              }
                              else
                              {
                                     buffer_rx = buffer_rx >> 1;           // desplazamos el byte
                                     buffer_rx = (buffer_rx & 0b01111111); // limpiar el bit MSB
                                     n_bits = k+1;
                                     delay_us(550);
                              }         
                      //        output_bit(tx,0);
                        }
                      }
                      
                         n_bits = 0;  //limpia el conteo
                           //  if(buffer_rx == 0b00000101)
                           if(buffer_rx == 0b00000101)
                             {
                                 ok=1;
                             }
                             else
                             {
                                 error = 1; 
                             }
                      ////////////////////////////////////////////////////////////////////////////////////
                      ////////////////////////////////////////////////////////////////////////////////
           
}


void ready_fpga()
{
                     int1 aviso=0;
                     while(aviso == 0)
                     {
                      if (flanco_subida == 1)
                      {
                           if (input(rx) == 1)
                           {aviso = 1;}
                           else{;}
                      }
                      else if (flanco_subida ==0)
                      {;}
                     }
                      
                      
                        for (int k=0 ; k<8 ; k++)
                      {
                      
                        while(flanco_subida == 0)
                           {;}
                        if(flanco_subida == 1 && n_bits == k)
                        {
                           bit_rx = input(rx);
                              if(bit_rx == 1)
                              {
                                     buffer_rx = buffer_rx >> 1;           // desplazamos el byte
                                     buffer_rx = (buffer_rx & 0b01111111); // limpiar el bit MSB
                                     buffer_rx = (buffer_rx | 0b10000000); // colocar "1" en el bit MSB
                                     n_bits = k+1;
                                     delay_us(550);
                              }
                              else
                              {
                                     buffer_rx = buffer_rx >> 1;           // desplazamos el byte
                                     buffer_rx = (buffer_rx & 0b01111111); // limpiar el bit MSB
                                     n_bits = k+1;
                                     delay_us(550);
                              }                       
                        }
                      }
                      
                         n_bits = 0;  //limpia el conteo
                           //  if(buffer_rx == 0b00000101) //ready
                           if(buffer_rx == 0b00001101)
                             {
                                 ok=1;
                             }
                             else
                             {
                                 error = 1; 
                             }
}


void Velocidad()
{
         setup_adc(adc_clock_internal);
         setup_adc_ports(sAN16);   //C0
         set_adc_channel(16);
         delay_us(100);
         adc = read_adc();
         if (adc <= 256)
         {
             v = 0;                  /// VELOCIDAD SEGURA
         }
         else if (adc > 256 && adc <= 512)
         {
              v = 1;                //// VELOCIDAD LENTA
         }
         else if (adc > 512 && adc <= 768)
         {
              v = 2;                //// VELOCIDAD MEDIA
         }
         else if (adc <= 1024)
         {
              v = 3;               //// VELOCIDAD ALTA
         }

}

void Errorr()
{
   while(1)
   {
      output_high(led);
   }
}


void saludar()
{
          
              /////////////////////////////////////////////////////////////////////
               n_bits = 0;  //limpia el conteo
               while(flanco_subida == 0) // no leer flanco de bajada
                           {;}
                      for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = comando[pata]>>k & 0b00000001 ;  
                           output_bit(tx,bit_envio);                  // 0b00000101
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
                      
           /////////////////////////////////////////////////////////////////////////////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////////////////////
           ////////////////ENVIAR GRADO DEL MOTOR 1////////////////////////////////////////////////
           /////////////////////////////////////////////////////////////////////////////////////////
                      n_bits = 0;  //limpia el conteo
                     columna = 0;
                      
                   for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = saludo[fila][columna]>>k & 0b00000001 ;   //Enviamos grados en binario MOTOR 1
                           output_bit(tx,bit_envio);
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
                  /////////////////////////////////////////////////////////////////////////////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////////////////////
           ////////////////ENVIAR GRADO DEL MOTOR 2////////////////////////////////////////////////
           /////////////////////////////////////////////////////////////////////////////////////////     
                         n_bits = 0;  //limpia el conteo
                         columna = 1;
                   for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = saludo[fila][columna]>>k & 0b00000001 ;   //Enviamos grados en binario MOTOR 2
                           output_bit(tx,bit_envio);
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
               /////////////////////////////////////////////////////////////////////////////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////////////////////
           ////////////////ENVIAR GRADO DEL MOTOR 3////////////////////////////////////////////////
           /////////////////////////////////////////////////////////////////////////////////////////        
                             n_bits = 0;  //limpia el conteo
                             columna = 2;
                   for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                         bit_envio = saludo[fila][columna]>>k & 0b00000001 ;   //Enviamos grados en binario MOTOR 1
                           output_bit(tx,bit_envio);
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
                      n_bits = 0;  //limpia el conteo
                       output_bit(tx,0);
                  
                 //////////ESPERAR 9 FLANCOS DE BAJADA DE CLK//////////////////////////////////////////////
                 ////////////////////////////////////////////////////////////////////////////////////
                       for (int k=0 ; k<9 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           n_bits = k+1;
                           delay_us(550);
                        }
                      }
                 ////////////////////////////////////////////////////////////////////////////////////////////
                 ////////////////////////////////////////////////////////////////////////////
                
                      n_bits = 0;  //limpia el conteo
                      
                      
                        while(flanco_subida == 1) // estabilizar
                           {;}
                           
                        while(flanco_subida == 0) // no leer flanco de bajada
                           {;}
      
                           
                           /////////////////////LEER EL "OK"///////////
                           //////////////////////////////////////////////
                        for (int k=0 ; k<8 ; k++)
                      {
                      
                        while(flanco_subida == 0)
                           {;}
                       //    output_bit(tx,1);
                        if(flanco_subida == 1 && n_bits == k)
                        {
                        
                           bit_rx = input(rx);
                              if(bit_rx == 1)
                              {
                                     buffer_rx = buffer_rx >> 1;           // desplazamos el byte
                                     buffer_rx = (buffer_rx & 0b01111111); // limpiar el bit MSB
                                     buffer_rx = (buffer_rx | 0b10000000); // colocar "1" en el bit MSB
                                     n_bits = k+1;
                                     delay_us(550);
                              }
                              else
                              {
                                     buffer_rx = buffer_rx >> 1;           // desplazamos el byte
                                     buffer_rx = (buffer_rx & 0b01111111); // limpiar el bit MSB
                                     n_bits = k+1;
                                     delay_us(550);
                              }         
                      //        output_bit(tx,0);
                        }
                      }
                      
                         n_bits = 0;  //limpia el conteo
                           //  if(buffer_rx == 0b00000101)
                           if(buffer_rx == 0b00000101)
                             {
                                 ok=1;
                             }
                             else
                             {
                                 error = 1; 
                             }
                      ////////////////////////////////////////////////////////////////////////////////////
                      ////////////////////////////////////////////////////////////////////////////////
           
}

void velocidad_total()
{
  Velocidad();
          
               
                   if (v==0) 
                   {z=8;
                     output_low(led);
                     output_low(led_0);
                     output_low(led_1);
                   
                   }
                 else if (v==1) 
                 {z=9;
                 output_high(led_0);
                 output_low(led_1);
                     }
                 else if (v==2) 
                 {
                 z=10;
                 output_low(led_0);
                 output_high(led_1);
                 }
                 else if (v==3) 
                 {
                 z=11;
                 output_high(led_0);
                 output_high(led_1);
                 }
                 n_bits = 0;  //limpia el conteo
                 
                //MANDAR COMANDO "VELOCIDAD "
              /////////////////////////////////////////////////////////////////////
              
               while(flanco_subida == 0) // no leer flanco de bajada
                           {;}
                      for (int k=0 ; k<8 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           bit_envio = comando[z]>>k & 0b00000001 ;   //Enviamos los 8 bits de velocidad elegida
                           output_bit(tx,bit_envio);
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
                      
           /////////////////////////////////////////////////////////////////////////////////////////////////////
           ///////////////////////////////////////////////////////////////////////////////////////////////
                      output_bit(tx,0);
                      n_bits = 0;  //limpia el conteo
                  
                 //////////ESPERAR 9 FLANCOS DE BAJADA DE CLK//////////////////////////////////////////////
                 ////////////////////////////////////////////////////////////////////////////////////
                       for (int k=0 ; k<9 ; k++)
                      {
                         while(flanco_subida == 1)
                           {;}
                        if(flanco_subida == 0 && n_bits == k)
                        {
                           n_bits = k+1;
                           delay_us(600);
                        }
                      }
                 ////////////////////////////////////////////////////////////////////////////////////////////
                 ////////////////////////////////////////////////////////////////////////////
                
                      n_bits = 0;  //limpia el conteo
                      
                      
                        while(flanco_subida == 1) // estabilizar
                           {;}
                           
                        while(flanco_subida == 0) // no leer flanco de bajada
                           {;}
      
                           
                           /////////////////////LEER EL "OK"///////////
                           //////////////////////////////////////////////
                        for (int k=0 ; k<8 ; k++)
                      {
                      
                        while(flanco_subida == 0)
                           {;}
                        if(flanco_subida == 1 && n_bits == k)
                        {
                           bit_rx = input(rx);
                              if(bit_rx == 1)
                              {
                                     buffer_rx = buffer_rx >> 1;           // desplazamos el byte
                                     buffer_rx = (buffer_rx & 0b01111111); // limpiar el bit MSB
                                     buffer_rx = (buffer_rx | 0b10000000); // colocar "1" en el bit MSB
                                     n_bits = k+1;
                                     delay_us(550);
                              }
                              else
                              {
                                     buffer_rx = buffer_rx >> 1;           // desplazamos el byte
                                     buffer_rx = (buffer_rx & 0b01111111); // limpiar el bit MSB
                                     n_bits = k+1;
                                     delay_us(550);
                              }                       
                        }
                      }
                      
                         n_bits = 0;  //limpia el conteo
                           //  if(buffer_rx == 0b00000101)
                           if(buffer_rx == 0b00000101)
                             {
                                 ok=1;
                             }
                             else
                             {
                                 error = 1; 
                             }
                      ////////////////////////////////////////////////////////////////////////////////////
                      ////////////////////////////////////////////////////////////////////////////////
           
}
