#include <reg51.h>
#include "time_struct.h"
#include "delay.h"
#include "dig_display.h"
#include "timer.h"

#define GPIO_KEY P1
sbit bee=P1^5;

enum
{
	PLACE_MINUTE=0,
	PLACE_HOUR=1,
}CURSOR_PLACE=PLACE_MINUTE;
//光标显示位置，初始为分

enum
{
	CLOCK=0,
	ALARM=1,
	CLOCK_SET=2,
	ALARM_SET=3,
}CLOCK_MODEL=CLOCK;
//时钟模式，初始为时钟模式

enum
{
	ALARM_DISABLE=0,
	ALARM_ENABLE=1,
}ALARM_STATUS=ALARM_ENABLE;
//闹钟状态，初始为允许闹钟

enum
{
	KEY_VALUE_NULL=0,
	KEY_VALUE_K1=1,
	KEY_VALUE_K2=2,
	KEY_VALUE_K3=3,
	KEY_VALUE_K4=4,
}KEY_VALUE=KEY_VALUE_NULL;
//获取按键

enum
{
	DISPLAY_MODEL_CL=0,
	DISPLAY_MODEL_CLS=1,
	DISPLAY_MODEL_CLSM=2,
	DISPLAY_MODEL_CLSH=3,
	DISPLAY_MODEL_AL=4,
	DISPLAY_MODEL_ALS=5,
	DISPLAY_MODEL_ALSM=6,
	DISPLAY_MODEL_ALSH=7,
}DIG_DISPLAY_MODEL;
//数码管显示模式，由time.dispaly_model提供

void key_down();//按键检测
void check_clock_mode();//判定时钟模式
void alarm();//闹钟响起

time 	time_clock={0,0,0,DISPLAY_MODEL_CL},
		time_alarm={0,1,0,DISPLAY_MODEL_CL},
		time_display={0,0,DISPLAY_MODEL_CL};//初始化时钟，闹钟与时间显示，由time.h提供time结构体
unsigned int i=0;//临时变量

int main()
{
	timer0_init();//初始化计时器0
	int0_conf();//中断0配置
	while (1)
	{
		key_down();//按键检测
		check_clock_mode();//判定时钟模式
		alarm();//闹钟
		dig_display(&time_display);//数码管显示
	}
	return 0;
}

void alarm()
{
	if (time_clock.hour==time_alarm.hour&&time_clock.minute==time_alarm.minute)
	{
		if(ALARM_STATUS==ALARM_DISABLE)return;//如有按键则停止闹钟
		ALARM_STATUS=ALARM_ENABLE;//否则启用闹钟
		bee=1;
	}
}

void timer0() interrupt 1//计时器0中断1
{ 
	TH0 = (65536-50000)/256;//设置初值
    TL0 = (65536-50000)%256;
    i++;
    if (i==20)
    {
    	i=0;
    	time_clock.second++;//每秒钟计数
    }
	if(time_clock.second>=60)time_clock.second=0,time_clock.minute++;//进位
	if(time_clock.minute>=60)time_clock.minute=0,time_clock.hour++;//进位
	if(time_clock.hour>=24)time_clock.hour=0;//清零
}

void check_clock_mode()
{
	switch (CLOCK_MODEL)//判定闹钟模式,改变time_display.display_model，即显示模式
	{
		case CLOCK:
			time_display=time_clock;
			time_display.display_model=DISPLAY_MODEL_CL;
			break;
		case ALARM:
			time_display=time_alarm;
			time_display.display_model=DISPLAY_MODEL_AL;
			ALARM_STATUS=ALARM_DISABLE;
			break;
		case CLOCK_SET:
			time_display=time_clock;
			time_display.display_model=DISPLAY_MODEL_CLS;
			ALARM_STATUS=ALARM_DISABLE;
			switch (CURSOR_PLACE)
			{
				case PLACE_MINUTE:
					time_display=time_clock;
					time_display.display_model=DISPLAY_MODEL_CLSM;
					break;
				case PLACE_HOUR:
					time_display=time_clock;
					time_display.display_model=DISPLAY_MODEL_CLSH;
					break;
				default:
					break;
			}
			break;
		case ALARM_SET:
			time_display=time_alarm;
			time_display.display_model=DISPLAY_MODEL_ALS;
			ALARM_STATUS=ALARM_DISABLE;
			switch (CURSOR_PLACE)
			{
				case PLACE_MINUTE:
					time_display=time_alarm;
					time_display.display_model=DISPLAY_MODEL_ALSM;
					break;
				case PLACE_HOUR:
					time_display=time_alarm;
					time_display.display_model=DISPLAY_MODEL_ALSH;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

//检测按键
void key_down()
{
	int a=0;
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)
  	{
		delay(100);
		if(GPIO_KEY!=0x0f)
		{
			//测试列
			GPIO_KEY=0X0F;
			delay(5);
			switch(GPIO_KEY)
			{
				case(0X07):	
				//K1,由时钟/闹钟模式转变为时钟设定/闹钟设定模式
					KEY_VALUE=KEY_VALUE_K1;//获取键值
					if(ALARM_STATUS==ALARM_ENABLE)ALARM_STATUS=ALARM_DISABLE;
					//闹钟响时按键停止闹钟
					switch (CLOCK_MODEL)
					{
						case CLOCK:
							//转变为时钟设定模式，关定时器中断，清零秒位，设置光标位至分位
							ET0=0;
							time_clock.second=0;
							CURSOR_PLACE=PLACE_MINUTE;
							CLOCK_MODEL=CLOCK_SET;
							break;
						case ALARM:
							//转变为闹钟设定模式，开定时器中断，设置光标位至分位
							ET0=1;
							CURSOR_PLACE=PLACE_MINUTE;
							CLOCK_MODEL=ALARM_SET;
							break;
						case CLOCK_SET:
							//返回至时钟模式
							ET0=1;
							CLOCK_MODEL=CLOCK;
							break;
						case ALARM_SET:
							//返回至闹钟模式
							ET0=1;
							CLOCK_MODEL=ALARM;
							break;
						default:
							break;
					}
					break;
				case(0X0b):	
				//K2,调整分，时，仅可加
					KEY_VALUE=KEY_VALUE_K2;
					if(ALARM_STATUS==ALARM_ENABLE)ALARM_STATUS=ALARM_DISABLE;
					switch(CLOCK_MODEL)
					//判断时钟模式，确定设定时钟还是设定闹钟
					{
						case CLOCK_SET:
							switch(CURSOR_PLACE)
							//判断光标位置
							{
								case PLACE_MINUTE:
								//分++
									time_clock.minute++;
									if (time_clock.minute>=60)time_clock.minute=0;
									delay(10);
									break;
								case PLACE_HOUR:
								//时++
									time_clock.hour++;
									if (time_clock.hour>=24)time_clock.hour=0;
									delay(10);
									break;
								default:
									break;	
							}
							break;
						case ALARM_SET:
							switch(CURSOR_PLACE)
							{
								case PLACE_MINUTE:
									time_alarm.minute++;
									if (time_alarm.minute>=60)time_alarm.minute=0;
									delay(10);
									break;
								case PLACE_HOUR:
									time_alarm.hour++;
									if (time_alarm.hour>=24)time_alarm.hour=0;
									delay(10);
									break;
								default:
									break;	
							}
							break;
						default:
							break;
					}
					delay(5);
					break;
				case(0X0d): 
				//K3,设置光标位置
					KEY_VALUE=KEY_VALUE_K3;
					//获取键值
					if(ALARM_STATUS==ALARM_ENABLE)ALARM_STATUS=ALARM_DISABLE;
					switch (CURSOR_PLACE)
					{
						case PLACE_MINUTE:
						//改变光标到时位
							CURSOR_PLACE=PLACE_HOUR;
							break;
						case PLACE_HOUR:
						//改变光标到分位
							CURSOR_PLACE=PLACE_MINUTE;
							break;
						default:
							break;
					}
					break;
				case(0X0e):	
				//K4，在时钟与闹钟模式间转换
					KEY_VALUE=KEY_VALUE_K4;
					//获取键值
					if(ALARM_STATUS==ALARM_DISABLE)ALARM_STATUS=ALARM_ENABLE;
					switch (CLOCK_MODEL)
					{
						case CLOCK:
						//闹钟转换为时钟
							CLOCK_MODEL=ALARM;
							break;
						case CLOCK_SET:
							CLOCK_MODEL=ALARM;
							break;
						case ALARM:
							CLOCK_MODEL=CLOCK;
							break;
						case ALARM_SET:
							CLOCK_MODEL=CLOCK;
							break;
						default:
							break;
					}
					break;
				default:	
					KEY_VALUE=KEY_VALUE_NULL;
					break;	
			}
			while ((a<50)&&GPIO_KEY!=0xf0)
			//检测是否松开
			{
				delay(100);
				a++;
			}
		}
	}
}
