#include <Teclado Matricial.h>


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

#define  fila1        pin_a0
#define  fila2        pin_a1
#define  fila3        pin_a2
#define  fila4        pin_a3
#define  columna1     pin_a4
#define  columna2     pin_a5
#define  columna3     pin_e0

#define  led0         pin_b0
#define  led1         pin_b1
#define  led2         pin_b2
#define  led3         pin_b3

////

///////////prototipos de funciones///////////////
int leer_entradas();
void prenderdelamatriz();
///////////datos///////////////////////////  
int dato;
////////////////////////////////////////////////////



int matriz [4][3] = {{0b00000001,0b00000010,0b00000011},
                     {0b00000100,0b00000101,0b00000110},
                     {0b00000111,0b00001000,0b00001001},
                     {0b00001010,0b00001011,0b00001100}};

void main()
{
output_low(fila1);
output_low(fila2);
output_low(fila3);

   while(TRUE)
   {    
      dato = leer_entradas();
   //   prenderdelamatriz();                    
      output_b(dato);
   }
}



int leer_entradas()
{
 output_high(fila1);         
                if(input(columna1)==1)
                {
                    return(1);
                }
           else if(input(columna2)==1)
                {
                    return(2);
                }
           else if(input(columna3)==1)
                {
                    return(3);
                }
      output_low(fila1);
      delay_ms(1);
      output_high(fila2);         
                if(input(columna1)==1)
                {
                    return(4);
                }
           else if(input(columna2)==1)
                {
                    return(5);
                }
           else if(input(columna3)==1)
                {
                    return(6);
                }
      output_low(fila2);
      delay_ms(1);          
      output_high(fila3);         
                if(input(columna1)==1)
                {
                    return(7);
                }
           else if(input(columna2)==1)
                {
                    return(8);
                }
           else if(input(columna3)==1)
                {
                    return(9);
                }
           else return(0);
      output_low(fila3);
      delay_ms(1);          
      output_high(fila4);
                if(input(columna1)==1)
                {
                    return(10);
                }
           else if(input(columna2)==1)
                {
                    return(11);
                }
           else if(input(columna3)==1)
                {
                    return(12);
                }
      output_low(fila4);
      delay_ms(1);
}


void prenderdelamatriz()
{
 if(dato == 1)
               {
                   output_b(matriz[1][1]);
               }
         else if(dato == 2)
               {
                   output_b(matriz[1][2]);
               }
         else if(dato == 3)
               {
                   output_b(matriz[1][3]);
               }
         else if(dato == 4)
               {
                   output_b(matriz[2][1]);
               }
         else if(dato == 5)
               {
                   output_b(matriz[2][2]);
               }
         else if(dato == 6)
               {
                   output_b(matriz[2][3]);
               }
         else if(dato == 7)
               {
                   output_b(matriz[3][1]);
               }
         else if(dato == 8)
               {
                   output_b(matriz[3][2]);
               }
         else if(dato == 9)
               {
                   output_b(matriz[3][3]);
               }
         else if(dato == 10)
               {
                   output_b(matriz[4][1]);
               }
         else if(dato == 11)
               {
                   output_b(matriz[4][2]);
               }
         else if(dato == 12)
               {
                   output_b(matriz[4][3]);
               }
         else output_b(0);
   
}
