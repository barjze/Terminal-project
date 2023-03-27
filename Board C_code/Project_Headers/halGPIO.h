#ifndef _halGPIO_H_
#define _halGPIO_H_

#include "TFC.h"   		// private library - APP layer


#define ACK		0x06

typedef struct {
  char *array;
  size_t used;
  size_t size;
} Array;

typedef struct {
  Array name;
  int size;
  uint32_t *address;
//  int LRU;
} file_type;

typedef struct {
  file_type *array;
  int how_many_files;
  int free_space;
} Array_files;


extern enum FSMstate state;   // global variable
extern enum SYSmode lpm_mode; // global variable


extern Array massageA;
extern Array_files files_in_memory;

extern int flag_bottom_push_twice;
extern int bottomstate;
extern int bouttom_press;
extern int bouttom_last_press;
extern int k;
extern int start_type_keyboard;
extern int screen_file_show;
extern char *storge;
extern short ACK_flag;

extern void print_file_inline(file_type *a);
extern void initArray(Array *a, size_t initialSize);
extern void insertArray(Array *a, char element);
extern void freeArray(Array *a);
extern void init_filetype(file_type *a, Array name, int size, uint32_t adress);
extern void initArray_files(Array_files *a);
extern void insertArray_files(Array_files *a, file_type element);





extern void sysConfig(void);
extern void enterLPM(unsigned char LPM_level);

extern void enterLPM(unsigned char LPM_level);
extern void setPIT1_timer(int a);
extern void setTPM2_C0V(int a);
extern void delay(unsigned int t);
extern void setPIT1_timer(int a);
extern void disable_Key_Board_Intrapt();
extern void enable_Key_Board_Intrapt();
extern void disable_push_buttom_Intrapt();
extern void enable_push_buttom_Intrapt();

extern int keyboard();


extern void PORTD_IRQHandler(void);
extern void PORTA_IRQHandler(void);
extern void PIT_IRQHandler();
extern void FTM0_IRQHandler();
extern void FTM2_IRQHandler();
extern void ADC0_IRQHandler();
extern void DMA0_IRQHandler();

#endif







