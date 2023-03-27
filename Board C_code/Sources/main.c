/*
` * main implementation: use this 'C' sample to create your own application
 *
 */


//#include "derivative.h" /* include peripheral declarations */
#include "TFC.h"
#include  "..\Project_Headers\app.h"    		// private library - APP layer
#define MUDULO_REGISTER  0x2EE0

enum FSMstate state;
enum SYSmode lpm_mode;

char *storge = 0x20000000;

Array massageA;
Array_files files_in_memory;

int flag_bottom_push_twice = 0;
int bottomstate = 0;
int bouttom_press = -1;
int bouttom_last_press = -1;
int k = 0;
int start_type_keyboard = 0;
int screen_file_show;
short ACK_flag = 0;




int main(void){
	char buffer[2];
	sysConfig();
	//state = state1;
	//UART0_C2 &= ~UART_C2_TIE_MASK;                      // Disable Transmit interrupt
	//UART0_C2 &= ~UART_C2_RIE_MASK;                      // Disable Transmit interrupt

	while(1){
		switch(state){
		  case state0:
			 enterLPM(wait_mode);
			break;
			 
		  case state1:
			  enable_push_buttom_Intrapt();
			break;
			 
		  case state2:  
			  if(files_in_memory.how_many_files != 0){
				  enable_Key_Board_Intrapt();
				  enable_push_buttom_Intrapt();
				  LCD_clear_move_to_first_line();
				  if(screen_file_show >= 0){
					  sprintf(buffer, "%d)", files_in_memory.how_many_files - screen_file_show);
					  lcd_puts(buffer);
					  print_file_inline(&files_in_memory.array[screen_file_show]);
				  }
				  lcd_begin_second_line;
				  if(screen_file_show > 0){
					  sprintf(buffer, "%d)", files_in_memory.how_many_files - screen_file_show + 1);
					  lcd_puts(buffer);
				  	  print_file_inline(&files_in_memory.array[screen_file_show - 1]);
				  }
			  }
			  else{
				  LCD_clear_move_to_first_line();
				  lcd_puts("No File in Mem");
			  }
			  state = state0;
			  break;
			
		  case state3:
			  disable_Key_Board_Intrapt();
			  disable_push_buttom_Intrapt();
			  LCD_clear_move_to_first_line();
			  UART0_C2 |= UART_C2_RIE_MASK;
			  break;
			  
		  case state4:
			  break;
		   case state5:
			   return 0;
			   break;   
		}
	 }
}








