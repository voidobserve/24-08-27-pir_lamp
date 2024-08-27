/******************************************************************************
;  *   	@型号				  : MC30P6280
;  *   	@创建日期             : 2021.08.04
;  *   	@公司/作者			  : SINOMCU-FAE
;  *   	@晟矽微技术支持       : 2048615934
;  *   	@晟矽微官网           : http://www.sinomcu.com/
;  *   	@版权                 : 2021 SINOMCU公司版权所有.
;  *----------------------摘要描述---------------------------------

******************************************************************************/

#include "user.h"

#if USE_MY_DEBUG
// 使用红外协议发送32位数据，低位先行
void send_32bits_data_by_irsir(u32 send_data)
{
	// 先发送格式头 9ms高电平+4.5ms低电平
	// __set_input_pull_up(); // 高电平
	P15D = 1;
	delay_ms(9);
	// __set_output_open_drain(); // 低电平
	P15D = 0;
	delay_100us(45); // 延时4.5ms

	for (u8 i = 0; i < 32; i++)
	{
		if ((send_data >> i) & 0x01)
		{
			// 如果要发送逻辑1
			// __set_input_pull_up();	   // 高电平
			P15D = 1;
			delay_100us(6); // 600us
			// __set_output_open_drain(); // 低电平
			P15D = 0;
			delay_100us(17); // 1700us
		}
		else
		{
			// 如果要发送逻辑0
			// __set_input_pull_up();	   // 高电平
			P15D = 1;
			delay_100us(6); // 600us
			// __set_output_open_drain(); // 低电平
			P15D = 0;
			delay_100us(6); // 600us
		}
	}

	// 最后，设置为低电平
	// __set_output_open_drain(); // 低电平
	P15D = 0;
}
#endif

// 100us级延时，误差5%左右，（用于1.5ms的延时和单线发送数据上）
// 前提条件：FCPU = FHIRC / 1
void delay_100us(u32 xus)
{
	while (xus)
	{
		u8 i = 97;
		while (i)
		{
			Nop();
			i--;
		}
		xus--;
	}
}

// 延时5us，还是会有误差
// 不能传参，传参也会影响延时时间
void delay_5us(void)
{
	u8 xus = 2;
	while (xus--)
	{
		// Nop();
		__asm;
		// nop;
		// nop;
		// nop;
		// nop;
		__endasm;
	}
}

// 毫秒级延时
// 前提条件：FCPU = FHIRC / 1
void delay_ms(u32 xms)
{
	while (xms)
	{
		u16 i = 531;
		while (i--)
		{
			Nop();
		}
		xms--;
	}
}

// 人体检测传感器检测的数据引脚切换为输入模式
void pir_pin_in(void)
{
	// PIR_PIN = 1; // PIR引脚输出高电平
	DDR1 |= 0x01 << 4; // 输入模式
	// PIR_PIN = 1; // PIR引脚输出高电平
	// PUCON |= 0x01 << 4; // 内部上拉无效
	PUCON &= ~(0x01 << 4); // 内部上拉有效
}

// 人体检测传感器检测的数据引脚切换为输出模式
void pir_pin_out(void)
{
	DDR1 &= ~(0x01 << 4);
}

// 拨码开关检测引脚的配置
// 检测到0，开启人体感应模式
// 检测到1，开启常亮模式
void sel_pin_config(void)
{
	DDR1 |= 0x01;	  // 输入模式
	// PUCON &= ~(0x01); // 使能上拉电阻
	PDCON &= ~(0x01); // 使能下拉电阻
}

// 检测光敏的引脚
void light_sensor_pin_config(void)
{
	DDR1 |= 0x01 << 1;
	// PUCON &= ~(0x01 << 1); // 使能上拉电阻
	PDCON &= ~(0x01 << 1); // 使能下拉电阻
}

/************************************************
;  *    @Function Name       : C_RAM
;  *    @Description         : 初始化RAM
;  *    @IN_Parameter      	 :
;  *    @Return parameter    :
;  ***********************************************/
void C_RAM(void)
{
	__asm;
	movai 0x3F;
	movra FSR;
	movai 47;
	movra 0x3F;
	decr FSR;
	clrr INDF;
	djzr 0x3F;
	goto $ - 3;
	clrr 0x3F;
	__endasm;
}

/************************************************
;  *    @Function Name       : IO_Init
;  *    @Description         :
;  *    @IN_Parameter      	 :
;  *    @Return parameter    :
;  ***********************************************/
void IO_Init(void)
{
	P1 = 0x00;	  // 1:input 0:output
	DDR1 = 0x00;  // 1:input 0:output
	PUCON = 0xff; // 0:Effective 1:invalid
	PDCON = 0xff; // 0:Effective 1:invalid
	ODCON = 0x00; // 0:推挽输出  1:开漏输出
}

/************************************************
;  *    @Function Name       : Sys_Init
;  *    @Description         : 系统初始化
;  *    @IN_Parameter      	 :
;  *    @Return parameter    :
;  ***********************************************/
void Sys_Init(void)
{
	GIE = 0;
	C_RAM();
	IO_Init();

	sel_pin_config();		   // 拨码开关检测引脚的配置
	light_sensor_pin_config(); // 检测光敏的引脚的相关配置

	GIE = 1;
}

// 获取人体传感器的数值
u16 get_pirdata(void)
{
	u16 pirData;
	u8 checkCode, i;

	// PIR_PIN_IN();
	// if (PIR_PIN == 1)
	// {
	GIE = 0; // 关闭总中断

	PIR_PIN_OUT(); // 切换输出模式
	PIR_PIN = 1;   // PIR引脚输出高电平
	// DelayUs(120); // 延时120us左右
	delay_100us(1); // 延时100us
					// for (i = 0; i < 10; i++)
					// {
					// 	delay_5us();
					// }
	delay_5us();
	delay_5us();

	pirData = 0;
	checkCode = 0;
	for (i = 0; i < 19; i++) // 读取19位数据
	{
		PIR_PIN_OUT(); // 切换输出模式
		PIR_PIN = 0;   // PIR引脚输出低电平
		// DelayUs(4);	 // 延时4us~8us
		delay_5us();

		PIR_PIN = 1; // PIR引脚输出高电平
		// DelayUs(4);	 // 延时4us~8us
		delay_5us();
		// delay_5us();

		PIR_PIN_IN(); // 切换输入模式（输入上拉）
					  // PIR_PIN = 1;  // PIR引脚输出高电平
					  // delay_5us();
					  // delay_5us();

#if USE_MY_DEBUG
		P15D = ~P15D;
#endif

		// 读取数据
		if (i < 2 || i > 17)
		{
			checkCode <<= 1;
			if (PIR_PIN == 1)
			{
				checkCode |= 0x01;
			}
			else
			{
				PIR_PIN = 0;   // PIR引脚输出低电平
				PIR_PIN_OUT(); // 切换输出模式
			}
		}
		else
		{
			pirData <<= 1;
			if (PIR_PIN == 1)
			{
				pirData |= 0x01;
			}
			else
			{
				PIR_PIN = 0;   // PIR引脚输出低电平
				PIR_PIN_OUT(); // 切换输出模式
			}
		}

		delay_5us();
	}

	PIR_PIN_OUT(); // 切换输出模式
	PIR_PIN = 0;   // PIR引脚输出低电平

	// DelayUs(4); // 延时4us~8us
	delay_5us();
	// PIR_PIN_IN(); // 切换输入模式

	GIE = 1; // 开启总中断

	if (checkCode == 0x04) // 校验头码尾码
	{
		if ((pirData & (u16)0x8000) != 0) // 校验负数
		{
			pirData = (~pirData) + 1; // 转换为正数
		}

		if (pirData < 1500)
		{
			return pirData;
		}
		else
		{
			return 0;
		}
	}
	// } // 检测引脚高电平
	return 0;
}

// P13设置成输入上拉，由内部的上拉电阻将线拉成高电平
void __set_input_pull_up(void)
{
	DDR1 |= 0x01 << 3;	   // P13配置成输入模式
	PUCON &= ~(0x01 << 3); // 使能P13上拉电阻
}

// P13设置成开漏输出，输出低电平
void __set_output_open_drain(void)
{
	DDR1 &= ~(0x01 << 3); // P13配置为输出模式，P13只有开漏输出
	P13D = 0;
}

// 点亮所有LED
void led_on(void)
{
	__set_output_open_drain();
	P12D = 0;
}

// 熄灭所有LED
void led_off(void)
{
	__set_input_pull_up();
	P12D = 1;
}

// 获取光敏器件的状态
// 返回值： 1--没有检测到光，2--检测到光
u8 get_detected_light(void)
{
	if (LIGHT_SENSOR_PIN)
	{
		// 如果没有检测到光
		return 1;
	}
	else
	{
		// 如果检测到光
		return 2;
	}
}

// 人体感应模式
void mode_pir(void)
{
	u16 i = 0; // 循环计数值

	while (1)
	{
		// 退出条件 当开关状态切换时
		if (SEL_PIN)
		{
			for (i = 0; i < 20; i++)
			{
				if (SEL_PIN)
				{
					loop_cnt++;
				}
				delay_ms(1);
			}

			if (loop_cnt >= 16)
			{
				loop_cnt = 0; // 清除计数值
				// 如果开关选择了常亮模式，退出当前的人体感应模式
				return;
			}

			loop_cnt = 0; // 清除计数值
		}

		// if (1 == get_detected_light())
		{
			// 如果光敏器件没有检测到光
			// 开始人体感应

			for (i = 0; i < 3; i++)
			{
				pir_val = get_pirdata();

				if (wake_up_times >= 15)
				{
					if (pir_val >= PIR_THRESHOLD_VAL)
					{
						// 如果检测到的值大于 设定值，说明可能检测到有人
						detect_cnt++;
					}
					else
					{
						detect_cnt = 0;
					}

					if (detect_cnt >= 3)
					{
						IS_DETECT_PERSON_FLAG = 1;
					}
				} // if (wake_up_times >= 15)

				delay_ms(20);
			}
			detect_cnt = 0; // 清空计数值

			if (IS_DETECT_PERSON_FLAG)
			{
				// 如果运行到这里，说明检测到有人
				IS_DETECT_PERSON_FLAG = 0;

				led_on();		// 点亮LED
				delay_ms(5000); // 5s内不做检测

				// 接下来的10s内，如果一直检测不到有人，则熄灭LED
				for (i = 0; i < 500; i++)
				{
					pir_val = get_pirdata();
					if (pir_val >= PIR_THRESHOLD_VAL)
					{
						// 如果检测到的值大于 设定值，说明可能检测到有人
						detect_cnt++;
					}
					else
					{
						detect_cnt = 0;
					}

					if (detect_cnt >= 2)
					{
						detect_cnt = 0; // 清空计数值
						IS_DETECT_PERSON_FLAG = 1;
						break;
					}

					delay_ms(20);
				}
				detect_cnt = 0; // 清空计数值

				if (1 == IS_DETECT_PERSON_FLAG)
				{
					// 如果检测到有人
					// 重置时间
					IS_DETECT_PERSON_FLAG = 0;
					continue;
				}
				else if (0 == IS_DETECT_PERSON_FLAG)
				{
					// 如果这段时间内一直检测不到人
					led_off(); // 熄灭LED
					cur_light_status = 0;
				}
			}

		} // 如果光敏器件没有检测到光

		// 灯熄灭时，进入低功耗模式
		if (0 == cur_light_status)
		{
			// 打开对应的定时器--20ms唤醒一次
			TMRCR = 0x40;			   // 时钟源为内部 32768 Hz 低频
			T0CR = 4;				   // 预分频器分配给T0(而不是WDT)，32分频
			T0CNT = 255 - T0_CNT_TIME; // 递增计数器(20ms产生一次中断)
			T0IE = 1;				   // 打开定时器

			// 进入低功耗模式
			Stop();
			Nop();
			Nop();

			// 从低功耗唤醒后，关闭定时器
			T0IE = 0;

			if (wake_up_times <= 15)
			{
				wake_up_times++;
			}

			for (i = 0; i < 3; i++)
			{
				pir_val = get_pirdata();

				if (wake_up_times >= 15)
				{
					if (pir_val >= PIR_THRESHOLD_VAL)
					{
						// 如果检测到的值大于 设定值，说明可能检测到有人
						detect_cnt++;
					}
					else
					{
						detect_cnt = 0;
					}

					if (detect_cnt >= 3)
					{
						IS_DETECT_PERSON_FLAG = 1;
					}
				} // if (wake_up_times > 15)

				delay_ms(20);
			}

			detect_cnt = 0; // 清空计数值
		} // 灯熄灭时，进入低功耗模式 if (0 == cur_light_status)

	} // while (1)
}

//=================================================================================
void main(void)
{
	Sys_Init();

	P15D = 1;  // 传感器要一直供电
	led_off(); // 熄灭所有LED

#if 1 // while (1)

	while (1)
	{
		u8 i = 0;

#if 0  // 测试能否从传感器中读到数据
	   // pir_data = get_pirdata();
	   // send_32bits_data_by_irsir(pir_data);
	   // delay_ms(20);
#endif // 测试能否从传感器中读到数据

		if (SEL_PIN && 0 == cur_light_status) // 如果灯是灭的，才进行下一步，否则灯会有跳动
		{
			for (i = 0; i < 20; i++)
			{
				if (SEL_PIN)
				{
					loop_cnt++;
				}
				delay_ms(1);
			}

			if (loop_cnt >= 16)
			{
				// 高电平，LED常亮
				led_on();
				cur_light_status = 1; //
			}

			loop_cnt = 0; // 清除计数值
		}
		else if (0 == SEL_PIN)
		{
			for (i = 0; i < 20; i++)
			{
				if (0 == SEL_PIN)
				{
					loop_cnt++;
				}
				delay_ms(1);
			}

			if (loop_cnt >= 16)
			{
				// loop_cnt = 0; // 清除计数值
				// 开关一侧检测到是低电平，开启人体感应模式
				led_on(); // 点亮LED
				cur_light_status = 1;
				delay_ms(3000);
				led_off(); // 熄灭LED
				cur_light_status = 0;

				mode_pir(); // 人体感应模式 (内部有关循环)
			}
			// else
			// {
			// }

			loop_cnt = 0; // 清除计数值
		}

#if 0  // 5us延时测试
		delay_5us();
		P16D = ~P16D;
#endif //  5us延时测试
	}
#endif

#if 0 // 低功耗测试（测试通过）

	TMRCR = 0x40;			   // 时钟源为内部 32768 Hz 低频
	T0CR = 4;				   // 预分频器分配给T0(而不是WDT)，32分频
	T0CNT = 255 - T0_CNT_TIME; // 递增计数器(20ms产生一次中断)
	T0IE = 0;				   // 关闭定时器

	while (1)
	{
		// 开启低功耗唤醒的定时器
		T0CNT = 255 - T0_CNT_TIME;
		T0IE = 1;
		// Nop();
		// Nop();

		Stop(); // 进入低功耗模式
		Nop();
		Nop();

		// 关闭定时器
		T0IE = 255 - T0_CNT_TIME;
		T0IE = 0;

		// 测试周期唤醒的时间是否正确
		P15D = ~P15D;
	}

#endif // 低功耗测试
}

/************************************************
;  *    @Function Name       : Interrupt
;  *    @Description         : The interrupt function
;  *    @IN_Parameter          	 :
;  *    @Return parameter      	:
;  ***********************************************/
void int_isr(void) __interrupt
{
	__asm;
	movra _abuf;
	swapar _STATUS;
	movra _statusbuf;
	__endasm;

	if ((T0IF) && (T0IE))
	{
		T0CNT = 255 - T0_CNT_TIME; // 递增计数器
		T0IF = 0;
	}

	__asm;
	swapar _statusbuf;
	movra _STATUS;
	swapr _abuf;
	swapar _abuf;
	__endasm;
}

/**************************** end of file *********************************************/