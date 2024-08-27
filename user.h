#ifndef USER
#define USER

#include "mc30-common.h"
#include "mc30P6280.h"

#define u8 unsigned char
#define u16 unsigned int
#define u32 unsigned long int
#define uint8_t  unsigned char
#define uint16_t unsigned int
#define uint32_t unsigned long int

#define DEF_SET_BIT0           	   	0x01
#define DEF_SET_BIT1           	   	0x02
#define DEF_SET_BIT2           	   	0x04
#define DEF_SET_BIT3           	   	0x08
#define DEF_SET_BIT4           	   	0x10
#define DEF_SET_BIT5           	   	0x20
#define DEF_SET_BIT6           	   	0x40
#define DEF_SET_BIT7           	   	0x80

#define DEF_CLR_BIT0           	   	0xFE
#define DEF_CLR_BIT1           	   	0xFD
#define DEF_CLR_BIT2           	   	0xFB
#define DEF_CLR_BIT3           	   	0xF7
#define DEF_CLR_BIT4           	   	0xEF
#define DEF_CLR_BIT5           	   	0xDF
#define DEF_CLR_BIT6           	   	0xBF
#define DEF_CLR_BIT7           	   	0x7F

#define ENABLE 	           	   	1
#define DISABLE            	   	0

#define FAIL   	           	   	1
#define PASS               	   	0

//===============Field Protection Variables===============
u8 abuf;
u8 statusbuf;

#define USE_MY_DEBUG 0 // 是否打开调试功能

#if USE_MY_DEBUG
#define SIG_FEEDBACK_PIN P16D // 检测信号反馈的引脚（测试用）
#else
#define SIG_FEEDBACK_PIN P10D // 检测信号反馈的引脚
#endif

u8 abuf;
u8 statusbuf;

volatile u16 cnt_100us;
volatile u32 distance; // 存放距离的变量，单位：mm

//===============IO Define===============

//===============Global Variable===============

//===============Global Function===============
void CLR_RAM(void);
void IO_Init(void);
void TIMER0_INT_Init(void);
void TIMER1_INT_Init(void);
void Sys_Init(void);

//===============Define  Flag===============
typedef union {
   	unsigned char byte;
   	struct
   	{
   	   	u8 bit0 : 1;
   	   	u8 bit1 : 1;
   	   	u8 bit2 : 1;
   	   	u8 bit3 : 1;
   	   	u8 bit4 : 1;
   	   	u8 bit5 : 1;
   	   	u8 bit6 : 1;
   	   	u8 bit7 : 1;
   	}bits;
}bit_flag;
volatile bit_flag flag1;


#define TIMER_0_IT_FLAG flag1.bits.bit0		 // 定时器0是否触发中断的标志位
#define LOW_LEVEL_100us_FLAG flag1.bits.bit1 // 信号反馈引脚是否有100us低电平的标志位


#endif

/**************************** end of file *********************************************/