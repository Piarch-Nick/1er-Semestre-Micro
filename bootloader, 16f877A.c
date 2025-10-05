#include <bootloader, 16f877A.h>
 
#define _bootloader

// NOTE - User must include bootloader.h in application program
#include <bootloader.h>
#include <loader.c>

#define PUSH_BUTTON PIN_A0

#INT_GLOBAL
void isr(void){
	jump_to_isr(LOADER_END+5);
}

#org LOADER_END+1, LOADER_END+2
void application(void) {
  while(TRUE);
}

void main()
{
 
	// Enter Bootloader if Pin A0 is low after a RESET
	if(!input(PIN_A0))
	{
		load_program();
	}
	
	application();

}
