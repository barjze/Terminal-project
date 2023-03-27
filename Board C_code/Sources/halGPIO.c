#include "TFC.h"


void print_file_inline(file_type *a);
void initArray(Array *a, size_t initialSize);
void insertArray(Array *a, char element);
void freeArray(Array *a);
void init_filetype(file_type *a, Array name, int size, uint32_t adress);
void initArray_files(Array_files *a);
void insertArray_files(Array_files *a, file_type element);

void sysConfig(void);
void enterLPM(unsigned char LPM_level);

void enterLPM(unsigned char LPM_level);
void setPIT1_timer(int a);
void setTPM2_C0V(int a);
void delay(unsigned int t);
void setPIT1_timer(int a);
void disable_Key_Board_Intrapt();
void enable_Key_Board_Intrapt();
int keyboard();


void PORTD_IRQHandler(void);
void PORTA_IRQHandler(void);
void PIT_IRQHandler();
void FTM0_IRQHandler();
void FTM2_IRQHandler();
void ADC0_IRQHandler();
void UART0_IRQHandler();
void DMA0_IRQHandler();



void initArray_files(Array_files *a) {
  a->array = malloc(1 * sizeof(file_type));
  a->how_many_files = 0;
  a->free_space = 8192;
}

void insertArray_files(Array_files *a, file_type element) {
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size 
  a->how_many_files += 1;
  a->array = realloc(a->array, a->how_many_files * sizeof(file_type));
  a->array[a->how_many_files - 1] = element;
  a->free_space -= element.size; 
  screen_file_show = files_in_memory.how_many_files - 1;
}



void init_filetype(file_type *a, Array name, int size, uint32_t adress) {
  initArray(&a->name, 1);
  int i;
  for(i=0; i<name.used; i++){
	  insertArray(&a->name, name.array[i]);
  }
  //a->name = malloc(1 * sizeof(name));
  a->size = size;
  a->address = adress;
  //a->LRU = 0;
}


void Array_files_delete_first_element(Array_files *a) {
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size 
	int i;
	freeArray_for_file_type(&a->array[0].name);
	a->how_many_files -= 1;
	a->free_space += a->array[0].size;
	for(i = 0; i < a->how_many_files; i++){
		a->array[i] = a->array[i + 1];
	}
	a->array = realloc(a->array, a->how_many_files * sizeof(file_type));
	//a->array = realloc(a->array, a->how_many_files * sizeof(file_type));
}

void initArray(Array *a, size_t initialSize) {
  a->array = malloc(initialSize * sizeof(char));
  a->used = 0;
  a->size = initialSize;
}

void insertArray(Array *a, char element) {
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size 
  if (a->used == a->size) {
    a->size += 2;   //<<<----------------------------------------------------------------- was here a->size *= 2
    a->array = realloc(a->array, a->size * sizeof(char));
  }
  a->array[a->used++] = element;
}

void freeArray(Array *a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
  initArray(a, 1);
}

void freeArray_for_file_type(Array *a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}


void print_file_inline(file_type *a){
	int i = 0;
	char buffer[10];
	for(i = 0; i < a->name.used; i++){
		lcd_data(a->name.array[i]);
	}

	sprintf(buffer, " %dB", a->size);
	lcd_puts(buffer);
}

//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
		InitGPIO();
		lcd_init();
		//InitADCs();
		//InitDAC();
		InitPIT();
		ClockSetupTPM();
		//InitTPM(2);
		dma_init(0);
		dma_init(1);
		InitUARTs();
		GIE();
		initArray(&massageA, 1);
		initArray_files(&files_in_memory);
		disable_Key_Board_Intrapt();
		state = state0;
}


void enterLPM(unsigned char LPM_level){
	if (LPM_level == stop_mode) 
		stop();    /* Enter Low Power Mode 0 */
    else if(LPM_level == wait_mode) 
    	wait();      /* Enter Low Power Mode 1 */
}

void enable_Key_Board_Intrapt(){
	clear_key_board_flag;
	start_type_keyboard = 0;
	enable_irq(INT_PORTA-16);           // Enable PORTA Interrupts 
}

void disable_Key_Board_Intrapt(){
	disable_irq(INT_PORTA-16);           // Enable PORTA Interrupts 
}


void enable_push_buttom_Intrapt(){
	PBsArrIntPendClear(PB0_LOC);
	PBsArrIntPendClear(PB1_LOC);
	PBsArrIntPendClear(PB2_LOC);
	PBsArrIntPendClear(PB3_LOC);
	enable_irq(INT_PORTD-16);           // Enable PORTA Interrupts 
}

void disable_push_buttom_Intrapt(){
	disable_irq(INT_PORTD-16);           // Enable PORTA Interrupts 
}


void setPIT1_timer(int a){
	disable_pit(); //disable PIT0 and its interrupt
	PIT_LDVAL1 = a; // setup timer a for a counting period
	PIT_TCTRL1 = PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK; //enable PIT0 and its interrupt
	
}

void setTPM2_C0V(int a){
	TPM2_C0V  = a; // setup timer a for a counting period
}

void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;
	
	for(i=t; i>0; i--);
}

void TPM0_off(){
    TPM0_C3SC = off_set_zero;
    TPM0_SC = off_set_zero;
}

void TPM2_off(){
    TPM2_C0SC = off_set_zero;
    TPM2_SC = off_set_zero;
}

void disable_pit(){
	disable_pit_module; // disable PIT0 and its interrupt
}

void start_TPM0(){
	  TPM0_SC |= TOF_TOIE_CMOD01_PS4;  //Start the TPM0 counter
	  TPM0_C3SC = Cflag_IE_inputcupture; //Start channel 3 as inputcupture interrupt enable
}

void start_TPM2(){
    TPM2_SC = CMOD01_DMA; 
    TPM2_C0SC = Cflag_PWM_cflag;
}

int keyboard(){
	int buttom = -1;
	keyboard_out_(row1);
	delay(1);
	if((keyboard_out_check_column_(column1)) == 0){
		buttom = 1;
		flag_bottom_push_twice += 1;
	}
	else if((keyboard_out_check_column_(column2))== 0){
		buttom = 2;
		flag_bottom_push_twice += 1;
	}
	else if((keyboard_out_check_column_(column3))== 0){
		buttom = 3;
		flag_bottom_push_twice += 1;
	}
	else if((keyboard_out_check_column_(column4))== 0){
		buttom = 10;
		flag_bottom_push_twice += 1;
	}
	
	keyboard_out_(row2);
	delay(1);
	if((keyboard_out_check_column_(column1)) == 0){
		buttom = 4;
		flag_bottom_push_twice += 1;
	}
	else if((keyboard_out_check_column_(column2)) == 0){
		buttom = 5;
		flag_bottom_push_twice += 1;
	}
	else if((keyboard_out_check_column_(column3)) == 0){
		buttom = 6;
		flag_bottom_push_twice += 1;
	}
	else if((keyboard_out_check_column_(column4)) == 0){
		buttom = 11;
		flag_bottom_push_twice += 1;
	}
	
	keyboard_out_(row3);
	delay(1);
	if((keyboard_out_check_column_(column1)) == 0){
		buttom = 7;
		flag_bottom_push_twice += 1;
	}
	else if((keyboard_out_check_column_(column2)) == 0){
		buttom = 8;
		flag_bottom_push_twice += 1;
	}
	else if((keyboard_out_check_column_(column3)) == 0){
		buttom = 9;
		flag_bottom_push_twice += 1;
	}
	else if((keyboard_out_check_column_(column4)) == 0){
		buttom = 12;
		flag_bottom_push_twice += 1;
	}
	
	keyboard_out_(row4);
	delay(1);
	if((keyboard_out_check_column_(column1)) == 0){
		buttom = 15;
		flag_bottom_push_twice += 1;
	}
	else if((keyboard_out_check_column_(column2)) == 0){
		buttom = 0;
		flag_bottom_push_twice += 1;
	}
	else if((keyboard_out_check_column_(column3)) == 0){
		buttom = 14;
		flag_bottom_push_twice += 1;
	}
	else if((keyboard_out_check_column_(column4)) == 0){
		buttom = 13;
		flag_bottom_push_twice += 1;
		
	}
	keyboard_out_off;
	if (state == state1){
		setPIT1_timer(0x2DC6C00); // set pit timer to inturrupt every 2000ms
	}
	if(buttom == -1){
		lcd_begin_second_line;
		lcd_puts("Error bottm");
		state = state0;
	}
	else{
		return buttom;
	}
}

char keyboard_decode(int button,int how_much_press ){
	char real_press;
	if(button == 0){
		if(how_much_press % 2 == 0) real_press = 'T';
		if(how_much_press % 2 == 1) real_press = '0';
	}
	else if(button == 1){
		if(how_much_press % 2 == 0) real_press = 'G';
		if(how_much_press % 2 == 1) real_press = '1';
	}
	else if(button == 2){
		if(how_much_press % 2 == 0) real_press = 'H';
		if(how_much_press % 2 == 1) real_press = '2';
	}
	else if(button == 3){
		if(how_much_press % 2 == 0) real_press = 'I';
		if(how_much_press % 2 == 1) real_press = '3';
	}
	else if(button == 4){
		if(how_much_press % 2 == 0) real_press = 'K';
		if(how_much_press % 2 == 1) real_press = '4';
	}
	else if(button == 5){
		if(how_much_press % 2 == 0) real_press = 'L';
		if(how_much_press % 2 == 1) real_press = '5';
	}
	else if(button == 6){
		if(how_much_press % 2 == 0) real_press = 'M';
		if(how_much_press % 2 == 1) real_press = '6';
	}
	else if(button == 7){
		if(how_much_press % 2 == 0) real_press = 'O';
		if(how_much_press % 2 == 1) real_press = '7';
	}
	else if(button == 8){
		if(how_much_press % 2 == 0) real_press = 'P';
		if(how_much_press % 2 == 1) real_press = '8';
	}
	else if(button == 9){
		if(how_much_press % 2 == 0) real_press = 'Q';
		if(how_much_press % 2 == 1) real_press = '9';
	}
	else if(button == 10){
		if(how_much_press % 2 == 0) real_press = 'J';
		if(how_much_press % 2 == 1) real_press = 'C';
	}
	else if(button == 11){
		if(how_much_press % 2 == 0) real_press = 'N';
		if(how_much_press % 2 == 1) real_press = 'D';
	}
	else if(button == 12){
		if(how_much_press % 2 == 0) real_press = 'R';
		if(how_much_press % 2 == 1) real_press = 'E';
	}
	else if(button == 13){
		if(how_much_press % 3 == 0) real_press = 'Z';
		if(how_much_press % 3 == 1) real_press = 'F';
		if(how_much_press % 3 == 2) real_press = 'Y';
	}
	else if(button == 14){
		if(how_much_press % 3 == 0) real_press = 'X';
		if(how_much_press % 3 == 1) real_press = 'B';
		if(how_much_press % 3 == 2) real_press = 'W';
	}
	else if(button == 15){
		if(how_much_press % 2 == 0) real_press = 'S';
		if(how_much_press % 2 == 1) real_press = 'A';
	}
	return real_press;
}

void char_on_screen_for_massge(int buttom){
	char a;
	a = keyboard_decode(buttom, flag_bottom_push_twice);
	if(flag_bottom_push_twice <= 1){
		lcd_data(a);
	}
	else{
		lcd_cursor_left();
		lcd_data(a);
	}
}
	
//-----------------------------------------------------------------
// PORTD = Interrupt Service Routine
//-----------------------------------------------------------------
void PORTD_IRQHandler(void){
   
	delay(debounceVal);
	if(PBsArrIntPend & PB0_LOC){
		 if( PIT_CVAL1 != PIT_LDVAL1){
			 PIT_IRQHandler();
		 }
		 PBsArrIntPendClear(PB0_LOC);
		 insertArray(&massageA, '\n');
		 LCD_clear_move_to_first_line();
		 //disable_Key_Board_Intrapt();		 
		 UART0_C2 |= UART_C2_TIE_MASK;                        // Enable Transmit interrupt
	}
    else if (PBsArrIntPend & PB1_LOC){
    	  screen_file_show--;
    	  if(screen_file_show < 0)
    		  screen_file_show = files_in_memory.how_many_files - 1;
    	  state = state2;
		  PBsArrIntPendClear(PB1_LOC);
    }
    else if (PBsArrIntPend & PB2_LOC){
		 state = state3;
		 PBsArrIntPendClear(PB2_LOC);
    }
    else if (PBsArrIntPend & PB3_LOC){	 
    		state = state4;
    		PBsArrIntPendClear(PB3_LOC);
    }
}

//-----------------------------------------------------------------
//  PORTA - ISR
//-----------------------------------------------------------------
void PORTA_IRQHandler(void){
	delay(debounceVal);
	if(state == state1){
		if(start_type_keyboard == 0){
			  LCD_clear_move_to_first_line();
	    	  start_type_keyboard += 1;
	    	  clear_key_board_flag;
	    	  return;
		}
		start_type_keyboard += 1;
		bouttom_last_press = bouttom_press;
		bouttom_press = keyboard();
		if (bouttom_last_press != bouttom_press){
			bottomstate = 0;
		}
		else{
			bottomstate = 1;
		}
		switch(bottomstate){
				case 0:
					if(bouttom_last_press == -1){
						break;
					}
					else{
						insertArray(&massageA, keyboard_decode(bouttom_last_press, flag_bottom_push_twice - 1));
						flag_bottom_push_twice = 1;
					}
					break;
				case 1:
					break;
				}
		char_on_screen_for_massge(bouttom_press);


		
		
		//setPIT1_timer(0x2DC6C00); // set pit timer to inturrupt every 2000ms
	}
	else if(state == state2 || state == state0){
		int keypress;
		keypress = keyboard();
		freeArray(&massageA);
		int i = 0;
		for(i = 0; i < files_in_memory.array[files_in_memory.how_many_files - keypress].name.used; i++){
			insertArray(&massageA, files_in_memory.array[files_in_memory.how_many_files - keypress].name.array[i]);
		}
		insertArray(&massageA, 2);
		i = 0;
		DMA_DSR_BCR1 = DMA_DSR_BCR_BCR(files_in_memory.array[files_in_memory.how_many_files - keypress].size);
		DMA_SAR1 = (uint32_t)files_in_memory.array[files_in_memory.how_many_files - keypress].address;
//		char* ptr = (char*)files_in_memory.array[files_in_memory.how_many_files - keypress].address;
//		int elment;
//		while(ptr[i] != 3){
//			elment = ptr[i];
//			insertArray(&massageA, elment);
//			i++;
//		}
//		insertArray(&massageA, 3);
		
		flag_bottom_push_twice = 0;
		UART0_C2 |= UART_C2_TIE_MASK;                        // Enable Transmit interrupt
	}
	
	clear_key_board_flag;

}

//-----------------------------------------------------------------
//  PIT - ISR
//-----------------------------------------------------------------
void PIT_IRQHandler(void){
	disable_pit();
	insertArray(&massageA, keyboard_decode(bouttom_press, flag_bottom_push_twice));
	flag_bottom_push_twice = 0;
	bottomstate = 0;
	bouttom_press = -1;
	bouttom_last_press = -1;
	clear_pit_flag;
}



//-----------------------------------------------------------------
//  UART0 - ISR
//-----------------------------------------------------------------
void UART0_IRQHandler(){
	uint8_t Temp;
		  if( UART0_S1 & UART_S1_RDRF_MASK ){ // RX buffer is full and ready for reading
		  		Temp = UART0_D;
		  		  
		  		insertArray(&massageA, Temp);

		  	      if (massageA.array[massageA.used - 1] == '\n'){
		  	    	  if(massageA.array[0] == '0'){
		  	    		freeArray(&massageA);
		  	    	  }
		  	    	  else if(massageA.array[0] == '1'){ // mod1 massge recive
						  LCD_clear_move_to_first_line();
						  int i = 1;
						  for(i = 1; i < massageA.used - 1; i++){ 
							  lcd_data(massageA.array[i]);
						  }
						  freeArray(&massageA);
						  enable_Key_Board_Intrapt();
		  	    	  }
		  	    	  else if(massageA.array[0] == '2'){ // mod2 - file resive 
		  	    		int i = 1;
		  	    		Array filename_temp;
		  	    		initArray(&filename_temp ,1);
		  	    		while(massageA.array[i] != 2){
		  	    			insertArray(&filename_temp,massageA.array[i]);
		  	    			i++;
		  	    		}			
		  	    		i++;
		  	    		int j = 0;
		  	    		int file_size = 0;
		  	    		while(massageA.array[i] != '\n'){
		  	    			file_size += ((massageA.array[i] - 48) * (pow(10,j)));
		  	    			i++;
		  	    			j++;
		  	    		}
		  	    		///<<<-----------------------------------------------------------------------------------------------
		  	    		///                       place for memory manegment --> FIFO
		  	    		while(files_in_memory.free_space < file_size){
		  	    			Array_files_delete_first_element(&files_in_memory);
		  	    		}
		  	    		
		  	    		///<<<------------------------------------------------------------------------------------------------
		  	    		file_type file;
		  	    		init_filetype(&file ,filename_temp, file_size, DMA_DAR0);
		  	    		insertArray_files(&files_in_memory, file);
		  	    		freeArray(&massageA);
		  	    		freeArray(&filename_temp);
		  	    		DMA_DSR_BCR0 = DMA_DSR_BCR_BCR(file_size);
		  	    		insertArray(&massageA, 0x03);
		  	    		insertArray(&massageA, ACK);
		  	    		UART0_C2 |= UART_C2_TIE_MASK;                      // Enable Transmit interrupt
		  	    	  }
		  	    	  else if(massageA.array[0] == '3'){
		  	    		  switch(massageA.array[1]){
		  	    		case '1':
		  	    			Uart0_Br_Sbr(CORE_CLOCK/2/1000, SDA_SERIAL_BAUD1);
		  	    			break;
		  	    		case '2':
		  	    			Uart0_Br_Sbr(CORE_CLOCK/2/1000, SDA_SERIAL_BAUD2);
		  	    			break;
		  	    		case '3':
		  	    			Uart0_Br_Sbr(CORE_CLOCK/2/1000, SDA_SERIAL_BAUD3);
		  	    			break;
		  	    		case '4':
		  	    			Uart0_Br_Sbr(CORE_CLOCK/2/1000, SDA_SERIAL_BAUD4);
		  	    			break;
		  	    		  }
		  	    		freeArray(&massageA);
		  	    		
		  	    	  }
		  	    	  else if(massageA.array[0] == '4'){
		  	    		switch(massageA.array[1]){
		  	    		case '1':
		  	    			state = state1;
		  	    			LCD_clear_move_to_first_line();
		  	    			break;
		  	    		case '2':
		  	    			state = state2;
		  	    			break;
		  	    		case '3':
		  	    			state = state3;
		  	    			break;
		  	    		case '4':
		  	    			ACK_flag = 1;
		  	    			break;
		  	    		case '5':
		  	    			ACK_flag = 0;
		  	    			break;
		  	    		}
		  	    		freeArray(&massageA);
		  	    	  }
		  	    	  else if(massageA.array[0] == '5'){
		  	    		  	freeArray(&massageA);
			  			    UART0_C2 &= ~UART_C2_TIE_MASK;                      // Disable Transmit interrupt
			  			    UART0_C2 &= ~UART_C2_RIE_MASK;                      // Disable Recive interrupt
			  			    state = state0;
			  			    DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(2); // Enable DMA to transfer automaticly from UART0 to RAM
		  	    	  }
		  	    	  else if(massageA.array[0] == '6'){
		  	    		  if(state == state2 || state == state0){
		  	    			  if(massageA.array[1] == '1'){
				  				UART0_C2 &= ~UART_C2_TIE_MASK;                      // Disable Transmit interrupt
				  				UART0_C2 &= ~UART_C2_RIE_MASK;                      // Disable Recive interrupt
				  				state = state0;
				  				DMAMUX0_CHCFG1 |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(3); // Enable DMA to transfer automaticly from UART0 to RAM
				  				//DMA_DCR1 |= 0x00010000;
		  	    			  }
		  	    			  else if(massageA.array[1] == '2' && ACK_flag == 1){
		  	    				LCD_clear_move_to_first_line();
		  	    				lcd_puts("ACK receive");
		  	    				state = state0;
		  	    			  }
		  	    			freeArray(&massageA);
		  	    		  }
		  	    		//LCD_clear_move_to_first_line();
		  	    		//lcd_puts("ACK recive");
		  	    	  }
		  	     }
		  	 }
		  	 else if((UART0_S1 & UART_S1_TDRE_MASK) && (UART0_C2 & UART_C2_TIE_MASK)){ // TX buffer is empty and ready for sending  
				 Temp = massageA.array[k];
				 UART0_D = Temp;
				 if(state == state1){
					 if (massageA.array[k] == '\n'){                  // TX over?
						  k = 0;
						  UART0_C2 &= ~UART_C2_TIE_MASK;                      // Disable Transmit interrupt
						  UART0_C2 |= UART_C2_RIE_MASK;                      // Enable recive interrupt
						  freeArray(&massageA);
						  return;
					 }
				 }
				 else if(state == state2 || state == state0){
					if (massageA.array[k] == 2){                  // TX over?
						k = 0;
		  				UART0_C2 &= ~UART_C2_TIE_MASK;                      // Disable Transmit interrupt
		  				UART0_C2 |= UART_C2_RIE_MASK;                      // Enable recive interrupt
						freeArray(&massageA);
						return;
//		  				UART0_C2 &= ~UART_C2_TIE_MASK;                      // Disable Transmit interrupt
//		  				UART0_C2 &= ~UART_C2_RIE_MASK;                      // Disable Recive interrupt
//		  				DMAMUX0_CHCFG1 |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(3); // Enable DMA to transfer automaticly from UART0 to RAM
//		  				DMA_DCR1 |= 0x00010000;
					}
					else if(massageA.array[k] == 6){
						k = 0;
						freeArray(&massageA);
						UART0_C2 &= ~UART_C2_TIE_MASK;                      // Disable Transmit interrupt
						UART0_C2 |= UART_C2_RIE_MASK;                       // Enable Recive interrupt
						return;
					}
//					else if (massageA.array[k] == 3){
//						k = 0;
//						freeArray(&massageA);
//						UART0_C2 &= ~UART_C2_TIE_MASK;                      // Disable Transmit interrupt
//						UART0_C2 |= UART_C2_RIE_MASK;                       // Enable Recive interrupt
//					}
				 }
				 k++;
				 
			}
}


//-----------------------------------------------------------------
//  DMA0 - ISR
//-----------------------------------------------------------------
void DMA0_IRQHandler(){
	DMAMUX0_CHCFG0 = 0x00; // disble DMA to transfer from UART0
	clear_DMA_BCR0_flag;
	insertArray(&massageA, 0x03);
	insertArray(&massageA, ACK);
	UART0_C2 |= UART_C2_TIE_MASK;                      // Enable Transmit interrupt
	state = state2;

	
	//UART0_D = 0x6;       // reload to buffer of UART ACK
	//DMA_DSR_BCR0 = 0x1;
	//DMA_DCR0 |= DMA_DCR_START_MASK;
}

void DMA1_IRQHandler(){
	DMAMUX0_CHCFG1 = 0x00; // disble DMA to transfer from UART0
	UART0_C2 |= UART_C2_RIE_MASK;                      // Enable receive interrupt
	clear_DMA_BCR1_flag;
	state = state2;
}
