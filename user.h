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

#define T0_CNT_TIME  20 // 定时器T0唤醒周期（从低功耗唤醒），单位：ms

#define PIR_PIN P14D // 人体检测传感器检测的数据引脚(3脚)
#define PIR_PIN_IN() pir_pin_in() // 引脚切换成输入上拉模式
#define PIR_PIN_OUT() pir_pin_out() // 引脚切换成输出模式

#define SEL_PIN P10D		  // 拨码开关检测引脚(7脚)
#define LIGHT_SENSOR_PIN P11D // 光敏器件检测引脚(6脚)，0-检测到光，1-未检测到光

// 下面这两个LED控制引脚一同控制LED的亮灭
// #define LED_CTL_PIN_1 P13D // 4脚
// #define LED_CTL_PIN_2 P12D // 5脚

#define PIR_THRESHOLD_VAL 120 // 人体感应阈值（样机的是80）

// 记录当前LED的状态，0--熄灭，1--点亮
// 如果当前LED是点亮的，在人体感应模式下就不检测光敏器件的状态
volatile u8 cur_light_status = 0;
u8 detect_cnt; // 临时存放当前人体传感器的检测到有人的次数
volatile u16 pir_val; // 存放从人体传感器中读取的数据

volatile u8 loop_cnt; // 检测开关状态是否变化时，使用到的计数值，当这个计数值大于一定值时，才认为开关状态变化
volatile u8 wake_up_times; // 记录当前是第几次唤醒（测试发现，前8次左右的唤醒，读取传感器的数据时会有很大的跳动）

//===============IO Define===============

//===============Global Variable===============

//===============Global Function===============
void C_RAM(void);
void IO_Init(void);
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

#define LED_ON_OFF_FLAG flag1.bits.bit0		  // LED亮/灭的标志位，0--熄灭，1--点亮
#define IS_DETECT_PERSON_FLAG flag1.bits.bit2 // 是否感应到有人的标志位，0--未感应到，1--感应到有人


#endif

/**************************** end of file *********************************************/