#include <HEXOPODO.h>
#include <math.h>
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


#define clk   PIN_A0
#define tx    PIN_A1
#define rx    PIN_A2
#define led   PIN_A3
// Potenciometro C0;
#define boton_alante   PIN_C1
#define boton_atras   PIN_C2
//// Prototipos de Funciones 
///////////////////////////////////////////
void initial ();
void elegir_estado_actual();    // maquina de mealy que, en funcion del estado actual y de las entradas decide cual es su estado futuro...
void ejecutar_estado_actual();
void mover_patas();
void ready_fpga();
void Velocidad();
void botones();
///////////////////////////////////////////

////////////////// matriz[0]    matriz[1]
//int matriz[10] = {0b100111111 , 0b00000110 , 0b01011011 , 0b01001111 , 0b1100110 , 0b01101101 , 0b01111101 , 0b00000111 , 0b01111111 , 0b01101111};
//int matriz[3][4]; // Matriz de 3 filas y 4 columnas de enteros
int estado_actual = 0;
int1 ok = 0;
int1 flanco_subida=1;
int n_bits =0;
int1 error = 0;
int1 bit_envio=0;
int pata=0;
int fila=0;
int columna=0;
int16 adc= 0;
int v=2;
int z=10;
int1 boton_adelante=0;
int1 boton_atrass=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////COMANDOS PARA CAMINAR///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

                           ///////////MOVIMIENTO 1///////////////////
int matrix[36][3] = {{0b01000110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA1  70grados,110grados,85grados.
                    {0b01000110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA2  70grados,110grados,85grados.
                    {0b01101110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA3  110grados,110grados,85grados.
                    {0b01000110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA4  70grados,110grados,85grados.
                    {0b01000110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA5  70grados,110grados,85grados.
                    {0b01101110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA6  110grados,110grados,85grados.
                   
                           ///////////MOVIMIENTO 2///////////////////
                    {0b01011010, 0b10010110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA1  90grados,150grados,110grados.
                    {0b01010000, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA2  80grados,110grados,85grados.
                    {0b01011010, 0b10010110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA3  90grados,150grados,110grados.
                    {0b01010000, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA4  80grados,110grados,85grados.
                    {0b01011010, 0b10010110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA5  90grados,150grados,85grados.
                    {0b01100100, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA6  100grados,110grados,85grados.
                    
                          ///////////MOVIMIENTO 3///////////////////
                    {0b01101110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA1  110grados,110grados,85grados.
                    {0b01011010, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA2  90grados,110grados,85grados.
                    {0b01000110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA3  70grados,110grados,85grados.
                    {0b01011010, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA4  90grados,110grados,85grados.
                    {0b01101110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA5  110grados,110grados,85grados.
                    {0b01011010, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA6  90grados,110grados,85grados.
                    
                            ///////////MOVIMIENTO 4///////////////////
                    {0b01100100, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA1  100grados,110grados,85grados.
                    {0b01100100, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA2  100grados,110grados,85grados.
                    {0b01010000, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA3  80grados,110grados,85grados.
                    {0b01100100, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA4  100grados,110grados,85grados.
                    {0b01100100, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA5  100grados,110grados,85grados.
                    {0b01010000, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA6  80grados,110grados,85grados.
                    
                             ///////////MOVIMIENTO 5///////////////////
                    {0b01011010, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA1  90grados,110grados,85grados.
                    {0b01101110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA2  110grados,110grados,85grados.
                    {0b01011010, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA3  90grados,110grados,85grados.
                    {0b01101110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA4  110grados,110grados,85grados.
                    {0b01011010, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA5  90grados,110grados,85grados.
                    {0b01000110, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA6  70grados,110grados,85grados.
                    
                           ///////////MOVIMIENTO 6///////////////////
                    {0b01010000, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA1  80grados,110grados,85grados.
                    {0b01011010, 0b10010110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA2  90grados,150grados,110grados.
                    {0b01100100, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA3  100grados,110grados,85grados.
                    {0b01011010, 0b10010110, 0b01101110},  /// 3 BYTES DE GRADOS PARA PATA4  90grados,150grados,110grados.
                    {0b01010000, 0b01101110, 0b01010101},  /// 3 BYTES DE GRADOS PARA PATA5  80grados,110grados,85grados.
                    {0b01011010, 0b10010110, 0b01101110}};  /// 3 BYTES DE GRADOS PARA PATA6  90grados,150grados,110grados.
                    
                    
                    
                    
                    //pata1     //pata2       //pata3     //pata4     //pata5     //pata6   //all servo  //all patas   //vel seguridad  //vel lenta  //vel media    //vel rapida  //READY        //PE
int comando[14] = {0b00000011 , 0b00000101, 0b00000111, 0b00001001, 0b00001011, 0b00001101, 0b00001111, 0b00010001,    0b00010011 ,   0b00010101,     0b00010111,    0b00011001,  0b00011011,  0b00011111};
                  //   0            1           2            3            4           5         6           7             8              9                10              11           12            13    
                  
                  
int buffer_rx = 0;
int1 bit_rx = 0;



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
                                   
   disable_interrupts(GLOBAL);
   disable_interrupts(INT_TIMER0);
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
}
  



void elegir_estado_actual()
{
   if (estado_actual == 0 && ok == 0)    ///////  MANDANDO READY
         {
            estado_actual = estado_actual;
         }
   else if (estado_actual == 0 && ok == 1)  // Si ha recivido el "ok" corectamente pasa a mandar velocidad
      {                       
         estado_actual = 1;  
         ok=0;//limpiamos el flag
       
      }
      else if (estado_actual == 0 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
         estado_actual = 10;  // ESTADO DE ERROR.  
       
      }
   else if (estado_actual == 1 && ok == 1)  // Si ha recivido el "ok" corectamente pasa a mandar ALL PATAS
      {                       
         estado_actual = 2;
          ok=0;//limpiamos el flag
      }
     else if (estado_actual == 1 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
        estado_actual = 10;  // ESTADO DE ERROR. 
      }
      
      
  ////////////////////////////////////////////////////////////////////////////////////////    
  ///////////////////////////////PREGUNTA CON LOS PULSADORES////////////////////////boton_alante   PIN_C1
//#define boton_atras
  ////////////////////////////////////////////////////////////////////////////////////////
      else if (estado_actual == 2 && ok == 1 && input(boton_alante)==1)  // Si ha recivido el "ok" corectamente pasa a mandar PATA1
      {                       
         estado_actual = 3; //// mueve para delante
          ok=0;//limpiamos el flag
      }
      else if (estado_actual == 2 && ok == 1 && input(boton_atras)==1)  // Si ha recivido el "ok" corectamente pasa a mandar PATA1
      {                       
         estado_actual = 50;   ///mueve para atras
          ok=0;//limpiamos el flag
      }
     else if (estado_actual == 2 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
        estado_actual = 10;  // ESTADO DE ERROR. 
      }
      
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
      else if (estado_actual == 3 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a mandar PATA2
      {                       
         estado_actual = 4;
          ok=0;//limpiamos el flag
      }
     else if (estado_actual == 3 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
        estado_actual = 10;  // ESTADO DE ERROR. 
      }
      else if (estado_actual == 4 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a mandar PATA3
      {                       
         estado_actual = 5;
          ok=0;//limpiamos el flag
      }
     else if (estado_actual == 4 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
        estado_actual = 10;  // ESTADO DE ERROR. 
      }
      else if (estado_actual == 5 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a mandar PATA4
      {                       
         estado_actual = 6;
          ok=0;//limpiamos el flag
      }
     else if (estado_actual == 5 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
        estado_actual = 10;  // ESTADO DE ERROR. 
      }
      else if (estado_actual == 6 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a mandar PATA5
      {                       
         estado_actual = 7;
          ok=0;//limpiamos el flag
      }
     else if (estado_actual == 6 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
        estado_actual = 10;  // ESTADO DE ERROR. 
      }
      else if (estado_actual == 7 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a mandar PATA6
      {                       
         estado_actual = 8;
          ok=0;//limpiamos el flag
      }
     else if (estado_actual == 7 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
        estado_actual = 10;  // ESTADO DE ERROR. 
      }
      else if (estado_actual == 8 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
         estado_actual = 9;
          ok=0;//limpiamos el flag
      }
     else if (estado_actual == 8 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
        estado_actual = 10;  // ESTADO DE ERROR. 
      }
        else if (estado_actual == 9 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
         estado_actual = 11;
          ok=0;//limpiamos el flag
      }
       else if (estado_actual == 11 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
         estado_actual = 12;
          ok=0;//limpiamos el flag
      }
       else if (estado_actual == 12 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
         estado_actual = 13;
          ok=0;//limpiamos el flag
      }
       else if (estado_actual == 13 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
         estado_actual = 14;
          ok=0;//limpiamos el flag
      }
       else if (estado_actual == 14 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
         estado_actual = 15;
          ok=0;//limpiamos el flag
      }
       else if (estado_actual == 15 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
         estado_actual = 16;
          ok=0;//limpiamos el flag
      }
      else if (estado_actual == 16 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
         estado_actual = 17;
          ok=0;//limpiamos el flag
      }
      else if (estado_actual == 17 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 18;
      }
      else if (estado_actual == 18 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 19;
      }
      else if (estado_actual == 19 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 20;
      }
      else if (estado_actual == 20 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 21;
      }
      else if (estado_actual == 21 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 22;
      }
      else if (estado_actual == 22 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 23;
      }
      else if (estado_actual == 23 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 24;
      }
      else if (estado_actual == 24 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 25;
      }
      else if (estado_actual == 25 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 26;
      }
      else if (estado_actual == 26 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 27;
      }
      else if (estado_actual == 27 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 28;
      }
       else if (estado_actual == 28 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 29;
      }
       else if (estado_actual == 29 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 30;
      }
       else if (estado_actual == 30 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 31;
      }
       else if (estado_actual == 31 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 32;
      }
       else if (estado_actual == 32 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 33;
      }
       else if (estado_actual == 33 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 34;
      }
       else if (estado_actual == 34 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 35;
      }
       else if (estado_actual == 35 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 36;
      }
       else if (estado_actual == 36 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 37;
      }
       else if (estado_actual == 37 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 38;
      }
       else if (estado_actual == 38 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 39;
      }
       else if (estado_actual == 39 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 40;
      }
       else if (estado_actual == 40 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 41;
      }
       else if (estado_actual == 41 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 42;
      }
       else if (estado_actual == 42 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 43;
      }
       else if (estado_actual == 43 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 44;
      }
       else if (estado_actual == 44 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 45;
      }
       else if (estado_actual == 45 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {               
         ok=0;   //limpiamos el flag
         estado_actual=2; 
      }
      
      
      
      
      
      
      
      else if (estado_actual == 50 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 51;
      }
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
         estado_actual = 56;
      }
      else if (estado_actual == 56 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 57;
      }
      else if (estado_actual == 57 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 58;
      }
      else if (estado_actual == 58 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 59;
      }
      else if (estado_actual == 59 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 60;
      }
      else if (estado_actual == 60 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 61;
      }
      else if (estado_actual == 61 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 62;
      }
      else if (estado_actual == 62 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 63;
      }
      else if (estado_actual == 63 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 64;
      }
      else if (estado_actual == 64 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 65;
      }
      else if (estado_actual == 65 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 66;
      }
      else if (estado_actual == 66 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 67;
      }
      else if (estado_actual == 67 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 68;
      }
      else if (estado_actual == 68 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 69;
      }
      else if (estado_actual == 69 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 70;
      }
      else if (estado_actual == 70 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 71;
      }
      else if (estado_actual == 71 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 72;
      }
      else if (estado_actual == 72 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 73;
      }
      else if (estado_actual == 73 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 74;
      }
      else if (estado_actual == 74 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 75;
      }
      else if (estado_actual == 75 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 76;
      }
      else if (estado_actual == 76 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 77;
      }
      else if (estado_actual == 77 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 78;
      }
      else if (estado_actual == 78 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 79;
      }
      else if (estado_actual == 79 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 80;
      }
      else if (estado_actual == 80 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 81;
      }
      else if (estado_actual == 81 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 82;
      }
      else if (estado_actual == 82 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 83;
      }
      else if (estado_actual == 83 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 84;
      }
      else if (estado_actual == 84 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 85;
      }
      else if (estado_actual == 85 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 86;
      }
      else if (estado_actual == 86 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 87;
      }
      else if (estado_actual == 87 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 88;
      }
      else if (estado_actual == 88 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 89;
      }
      else if (estado_actual == 89 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 90;
      }
      else if (estado_actual == 90 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 91;
      }
      else if (estado_actual == 91 && ok == 1)  // Si ha recivido el "ok" correctamente pasa a esperar el ready de la FPGA
      {                       
       ok=0;//limpiamos el flag
         estado_actual = 2;
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
                             if(buffer_rx == 0b11111111)
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
       
         case 1:  
                  
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
                           bit_envio = 0b01011010>>k & 0b00000001 ;   //Enviamos los 90grados en binario MOTOR 3
                           output_bit(tx,bit_envio);
                           n_bits = k+1;
                           delay_us(600);
                        }
                        else 
                        {
                          ;
                        }  
                      }
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
                           if(buffer_rx == 0b11111111)
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
           
                  
         case 2:
        
                      
                 if (v==0) {z=8;}
                 else if (v==1) {z=9;}
                 else if (v==2) {z=10;}
                 else if (v==3) {z=11;}
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
                           bit_envio = comando[z]>>k & 0b00000001 ;   //Enviamos los 8 bits de ready
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
                           if(buffer_rx == 0b11111111)
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
                        
                
           case 3:
           ///////////PATA 1////////////////////
           /////////////////////////////////////
                      pata=0;
                      fila=0;
                      columna=0;
                      mover_patas();
                       
                  break;       
                                    
      case 4:
      ///////////PATA 2////////////////////
           /////////////////////////////////////
                      pata=1;
                      fila=1;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
        case 5:
      ///////////PATA 3////////////////////
           /////////////////////////////////////
                      pata=2;
                      fila=2;
                      columna=0;
                      mover_patas();
   
                  break;    
       case 6:
      ///////////PATA 4////////////////////
           /////////////////////////////////////
                      pata=3;
                      fila=3;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
     case 7:
      ///////////PATA 5////////////////////
           /////////////////////////////////////
                      pata=4;
                      fila=4;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
    case 8:
      ///////////PATA 6////////////////////
           /////////////////////////////////////
                      pata=5;
                      fila=5;
                      columna=0;
                      mover_patas();
                      break;
     case 9:
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"//////////////////////////
       ///////////////////////////////////////////////////////////////
                  ready_fpga();      
                  
                  break;      
      case 10:
      ///////////ERROR/////////////////////
           /////////////////////////////////////
                     while(1)
                     {
                        output_toggle(led);
                        delay_ms(300);
                     }
                      break;
                      
       case 11:
           ///////////PATA 1////////////////////
           /////////////////////////////////////
                      pata=0;
                      fila=6;
                      columna=0;
                      mover_patas();
   
                  break;       
                                    
      case 12:
      ///////////PATA 2////////////////////
           /////////////////////////////////////
                      pata=1;
                      fila=7;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
        case 13:
      ///////////PATA 3////////////////////
           /////////////////////////////////////
                      pata=2;
                      fila=8;
                      columna=0;
                      mover_patas();
   
                  break;    
       case 14:
      ///////////PATA 4////////////////////
           /////////////////////////////////////
                      pata=3;
                      fila=9;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
     case 15:
      ///////////PATA 5////////////////////
           /////////////////////////////////////
                      pata=4;
                      fila=10;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
    case 16:
      ///////////PATA 6////////////////////
           /////////////////////////////////////
                      pata=5;
                      fila=11;
                      columna=0;
                      mover_patas();
                      break;
     case 17:
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"///////////
                           //////////////////////////////////////////////
                  ready_fpga();                      
                  break;      
    
    case 18:
           ///////////PATA 1////////////////////
           /////////////////////////////////////
                      pata=0;
                      fila=12;
                      columna=0;
                      mover_patas();
   
                  break;       
                                    
      case 19:
      ///////////PATA 2////////////////////
           /////////////////////////////////////
                      pata=1;
                      fila=13;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
        case 20:
      ///////////PATA 3////////////////////
           /////////////////////////////////////
                      pata=2;
                      fila=14;
                      columna=0;
                      mover_patas();
   
                  break;    
       case 21:
      ///////////PATA 4////////////////////
           /////////////////////////////////////
                      pata=3;
                      fila=15;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
     case 22:
      ///////////PATA 5////////////////////
           /////////////////////////////////////
                      pata=4;
                      fila=16;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
    case 23:
      ///////////PATA 6////////////////////
           /////////////////////////////////////
                      pata=5;
                      fila=17;
                      columna=0;
                      mover_patas();
                      break;
     case 24:
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"///////////
                           //////////////////////////////////////////////
                  ready_fpga();      
                  break;   
                  
       case 25:
           ///////////PATA 1////////////////////
           /////////////////////////////////////
                      pata=0;
                      fila=18;
                      columna=0;
                      mover_patas();
   
                  break;       
                                    
      case 26:
      ///////////PATA 2////////////////////
           /////////////////////////////////////
                      pata=1;
                      fila=19;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
        case 27:
      ///////////PATA 3////////////////////
           /////////////////////////////////////
                      pata=2;
                      fila=20;
                      columna=0;
                      mover_patas();
   
                  break;    
       case 28:
      ///////////PATA 4////////////////////
           /////////////////////////////////////
                      pata=3;
                      fila=21;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
     case 29:
      ///////////PATA 5////////////////////
           /////////////////////////////////////
                      pata=4;
                      fila=22;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
    case 30:
      ///////////PATA 6////////////////////
           /////////////////////////////////////
                      pata=5;
                      fila=23;
                      columna=0;
                      mover_patas();
                      break;
     case 31:
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"///////////
                           //////////////////////////////////////////////
                  ready_fpga();      
                  break;    
 
  case 32:
           ///////////PATA 1////////////////////
           /////////////////////////////////////
                      pata=0;
                      fila=24;
                      columna=0;
                      mover_patas();
   
                  break;       
                                    
      case 33:
      ///////////PATA 2////////////////////
           /////////////////////////////////////
                      pata=1;
                      fila=25;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
        case 34:
      ///////////PATA 3////////////////////
           /////////////////////////////////////
                      pata=2;
                      fila=26;
                      columna=0;
                      mover_patas();
   
                  break;    
       case 35:
      ///////////PATA 4////////////////////
           /////////////////////////////////////
                      pata=3;
                      fila=27;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
     case 36:
      ///////////PATA 5////////////////////
           /////////////////////////////////////
                      pata=4;
                      fila=28;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
    case 37:
      ///////////PATA 6////////////////////
           /////////////////////////////////////
                      pata=5;
                      fila=29;
                      columna=0;
                      mover_patas();
                      break;
     case 38:
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"///////////
                           //////////////////////////////////////////////
                  ready_fpga();                      
                  break;    
                  
     case 39:
           ///////////PATA 1////////////////////
           /////////////////////////////////////
                      pata=0;
                      fila=30;
                      columna=0;
                      mover_patas();
   
                  break;       
                                    
      case 40:
      ///////////PATA 2////////////////////
           /////////////////////////////////////
                      pata=1;
                      fila=31;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
        case 41:
      ///////////PATA 3////////////////////
           /////////////////////////////////////
                      pata=2;
                      fila=32;
                      columna=0;
                      mover_patas();
   
                  break;    
       case 42:
      ///////////PATA 4////////////////////
           /////////////////////////////////////
                      pata=3;
                      fila=33;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
     case 43:
      ///////////PATA 5////////////////////
           /////////////////////////////////////
                      pata=4;
                      fila=34;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
    case 44:
      ///////////PATA 6////////////////////
           /////////////////////////////////////
                      pata=5;
                      fila=35;
                      columna=0;
                      mover_patas();
                      break;
     case 45:
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"///////////
                           //////////////////////////////////////////////
                           
                  ready_fpga(); 
                  Velocidad();
                  botones();
                  
                  break; 
      
      
      
      
      
      
      ////////////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////MOVER PARA ATRAS////////////////////////////////////////////////////////////
      //////////////////////////////////////////////////////////////////////////////////////////////////
      
       case 50:
           ///////////PATA 1////////////////////
           /////////////////////////////////////
                      pata=0;
                      fila=0;
                      columna=0;
                      mover_patas();
   
                  break;       
                                    
      case 51:
      ///////////PATA 2////////////////////
           /////////////////////////////////////
                      pata=1;
                      fila=1;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
        case 52:
      ///////////PATA 3////////////////////
           /////////////////////////////////////
                      pata=2;
                      fila=2;
                      columna=0;
                      mover_patas();
   
                  break;    
       case 53:
      ///////////PATA 4////////////////////
           /////////////////////////////////////
                      pata=3;
                      fila=3;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
     case 54:
      ///////////PATA 5////////////////////
           /////////////////////////////////////
                      pata=4;
                      fila=4;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
    case 55:
      ///////////PATA 6////////////////////
           /////////////////////////////////////
                      pata=5;
                      fila=5;
                      columna=0;
                      mover_patas();
                      break;
     case 56:
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"///////////
                           //////////////////////////////////////////////
                           
                  ready_fpga();                  
                  break;   
      
   
 
  case 57:
           ///////////PATA 1////////////////////
           /////////////////////////////////////
                      pata=0;
                      fila=30;
                      columna=0;
                      mover_patas();
   
                  break;       
                                    
      case 58:
      ///////////PATA 2////////////////////
           /////////////////////////////////////
                      pata=1;
                      fila=31;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
        case 59:
      ///////////PATA 3////////////////////
           /////////////////////////////////////
                      pata=2;
                      fila=32;
                      columna=0;
                      mover_patas();
   
                  break;    
       case 60:
      ///////////PATA 4////////////////////
           /////////////////////////////////////
                      pata=3;
                      fila=33;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
     case 61:
      ///////////PATA 5////////////////////
           /////////////////////////////////////
                      pata=4;
                      fila=34;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
    case 62:
      ///////////PATA 6////////////////////
           /////////////////////////////////////
                      pata=5;
                      fila=35;
                      columna=0;
                      mover_patas();
                      break;
     case 63:
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"///////////
                           //////////////////////////////////////////////
                  ready_fpga();                      
                  break;   
                  
    case 64:
           ///////////PATA 1////////////////////
           /////////////////////////////////////
                      pata=0;
                      fila=24;
                      columna=0;
                      mover_patas();
   
                  break;       
                                    
      case 65:
      ///////////PATA 2////////////////////
           /////////////////////////////////////
                      pata=1;
                      fila=25;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
        case 66:
      ///////////PATA 3////////////////////
           /////////////////////////////////////
                      pata=2;
                      fila=26;
                      columna=0;
                      mover_patas();
   
                  break;    
       case 67:
      ///////////PATA 4////////////////////
           /////////////////////////////////////
                      pata=3;
                      fila=27;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
     case 68:
      ///////////PATA 5////////////////////
           /////////////////////////////////////
                      pata=4;
                      fila=28;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
    case 69:
      ///////////PATA 6////////////////////
           /////////////////////////////////////
                      pata=5;
                      fila=29;
                      columna=0;
                      mover_patas();
                      break;
     case 70:
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"///////////
                           //////////////////////////////////////////////
                  ready_fpga();                      
                  break;   
                  
    case 71:
           ///////////PATA 1////////////////////
           /////////////////////////////////////
                      pata=0;
                      fila=18;
                      columna=0;
                      mover_patas();
   
                  break;       
                                    
      case 72:
      ///////////PATA 2////////////////////
           /////////////////////////////////////
                      pata=1;
                      fila=19;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
        case 73:
      ///////////PATA 3////////////////////
           /////////////////////////////////////
                      pata=2;
                      fila=20;
                      columna=0;
                      mover_patas();
   
                  break;    
       case 74:
      ///////////PATA 4////////////////////
           /////////////////////////////////////
                      pata=3;
                      fila=21;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
     case 75:
      ///////////PATA 5////////////////////
           /////////////////////////////////////
                      pata=4;
                      fila=22;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
    case 76:
      ///////////PATA 6////////////////////
           /////////////////////////////////////
                      pata=5;
                      fila=23;
                      columna=0;
                      mover_patas();
                      break;
     case 77:
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"///////////
                           //////////////////////////////////////////////
                  ready_fpga();                      
                  break;  
                  
      case 78:
           ///////////PATA 1////////////////////
           /////////////////////////////////////
                      pata=0;
                      fila=12;
                      columna=0;
                      mover_patas();
   
                  break;       
                                    
      case 79:
      ///////////PATA 2////////////////////
           /////////////////////////////////////
                      pata=1;
                      fila=13;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
        case 80:
      ///////////PATA 3////////////////////
           /////////////////////////////////////
                      pata=2;
                      fila=14;
                      columna=0;
                      mover_patas();
   
                  break;    
       case 81:
      ///////////PATA 4////////////////////
           /////////////////////////////////////
                      pata=3;
                      fila=15;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
     case 82:
      ///////////PATA 5////////////////////
           /////////////////////////////////////
                      pata=4;
                      fila=16;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
    case 83:
      ///////////PATA 6////////////////////
           /////////////////////////////////////
                      pata=5;
                      fila=17;
                      columna=0;
                      mover_patas();
                      break;
     case 84:
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"///////////
                           //////////////////////////////////////////////
                  ready_fpga();                      
                  break;  
                  
    case 85:
           ///////////PATA 1////////////////////
           /////////////////////////////////////
                      pata=0;
                      fila=6;
                      columna=0;
                      mover_patas();
   
                  break;       
                                    
      case 86:
      ///////////PATA 2////////////////////
           /////////////////////////////////////
                      pata=1;
                      fila=7;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
        case 87:
      ///////////PATA 3////////////////////
           /////////////////////////////////////
                      pata=2;
                      fila=8;
                      columna=0;
                      mover_patas();
   
                  break;    
       case 88:
      ///////////PATA 4////////////////////
           /////////////////////////////////////
                      pata=3;
                      fila=9;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
     case 89:
      ///////////PATA 5////////////////////
           /////////////////////////////////////
                      pata=4;
                      fila=10;
                      columna=0;
                      mover_patas();
   
                  break;    
                  
    case 90:
      ///////////PATA 6////////////////////
           /////////////////////////////////////
                      pata=5;
                      fila=11;
                      columna=0;
                      mover_patas();
                      break;
     case 91:
      ///////////ESPERANDO EL READY DE LA FPGA////////////////////
           /////////////////////////////////////
                     
       /////////////////////LEER EL "READY"///////////
                           //////////////////////////////////////////////
                  ready_fpga();  
                  Velocidad();
                  botones();
                  break; 
     
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
                           if(buffer_rx == 0b11111111)
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
                           //  if(buffer_rx == 0b00000101)
                           if(buffer_rx == 0b11111111)
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




void botones()
{
     if(input(boton_alante)==1 && input(boton_atras)==1)
     {
            boton_adelante = 1;
            boton_atrass=0;
     }
     else if(input(boton_alante)==1)
     {
             boton_adelante = 1;
              boton_atrass=0;
     }
     else if (input(boton_atras==1))
     {
             boton_atrass = 1;
              boton_adelante=0;
     }
     else{;}
  
}
