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

//// Prototipos de Funciones 
///////////////////////////////////////////
void initial ();
void elegir_estado_actual();    // maquina de mealy que, en funcion del estado actual y de las entradas decide cual es su estado futuro...
void ejecutar_estado_actual();
void mover_patas();
void ready_fpga();
///////////////////////////////////////////

////////////////// matriz[0]    matriz[1]
//int matriz[10] = {0b100111111 , 0b00000110 , 0b01011011 , 0b01001111 , 0b1100110 , 0b01101101 , 0b01111101 , 0b00000111 , 0b01111111 , 0b01101111};
//int matriz[3][4]; // Matriz de 3 filas y 4 columnas de enteros
int estado_actual = 0;
int1 ok = 0;
int1 flanco_subida=1;
int n_bits =0;
int1 error = 1;
int1 bit_envio=0;
int pata=0;
int fila=0;
int columna=0;


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
      else if (estado_actual == 2 && ok == 1)  // Si ha recivido el "ok" corectamente pasa a mandar PATA1
      {                       
         estado_actual = 3;
          ok=0;//limpiamos el flag
      }
     else if (estado_actual == 2 && error == 1)  // si se ha activado error haz lo que hace estado error
      {                       
        estado_actual = 10;  // ESTADO DE ERROR. 
      }
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
       ok=0;//limpiamos el flag
         estado_actual = 3;
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
                           output_bit(tx,bit_envio);
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
                          // if(buffer_rx == 0b00000101)
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
                  
                //MANDAR COMANDO "VELOCIDAD MEDIA"
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
                           bit_envio = comando[9]>>k & 0b00000001 ;   //Enviamos los 8 bits de ready
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
                  
         case 2:
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
