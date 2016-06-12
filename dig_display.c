#include <reg51.h>
#include "delay.h"
#include "time_struct.h"

#define GPIO_DIG P0

static enum
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
//数码管显示模式标志位

static unsigned int temp_clock,temp_alarm;

sbit LSA=P2^2;
sbit LSB=P2^3;
sbit LSC=P2^4;
//引脚定义

DIG_CODE[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x40,
				0x04,0x00,0x39,0x77,0x38};
				//0-9,-,_,null,c,a,l
				//    10 11 12 131415
//显示字母表

void dig_display(time *time_input)
{
	unsigned int i,j;
	for (i=0;i<8;i++)
	{
		GPIO_DIG=0x00;//消影
		switch(i)//位选
		{
			case(0)://秒低位
				LSA=0;LSB=0;LSC=0; 
				if (time_input->display_model!=DISPLAY_MODEL_CL)
				{
					GPIO_DIG=DIG_CODE[15];
					break;
				}
				GPIO_DIG=DIG_CODE[time_input->second%10];
				break;
			case(1)://秒高位
				LSA=1;LSB=0;LSC=0;
				switch (time_input->display_model)
				{
					case DISPLAY_MODEL_CL:
						GPIO_DIG=DIG_CODE[time_input->second/10];
						break;
					case DISPLAY_MODEL_CLS:
						GPIO_DIG=DIG_CODE[13];
						break;
					case DISPLAY_MODEL_CLSH:
						GPIO_DIG=DIG_CODE[13];
						break;
					case DISPLAY_MODEL_CLSM:
						GPIO_DIG=DIG_CODE[13];
						break;
					case DISPLAY_MODEL_AL:
						GPIO_DIG=DIG_CODE[14];
						break;
					case DISPLAY_MODEL_ALS:
						GPIO_DIG=DIG_CODE[14];
						break;
					case DISPLAY_MODEL_ALSH:
						GPIO_DIG=DIG_CODE[14];
						break;
					case DISPLAY_MODEL_ALSM:
						GPIO_DIG=DIG_CODE[14];
						break;
					default:
						break;
				}
				break;
			case(2)://分隔符
				LSA=0;LSB=1;LSC=0; 
				GPIO_DIG=DIG_CODE[10];
				break;
			case(3)://分低位
				LSA=1;LSB=1;LSC=0;
				switch (time_input->display_model)
				{
					case DISPLAY_MODEL_CL:
						GPIO_DIG=DIG_CODE[time_input->minute%10];
						break;
					case DISPLAY_MODEL_CLS:
						GPIO_DIG=DIG_CODE[time_input->minute%10];
						break;
					case DISPLAY_MODEL_CLSH:
						GPIO_DIG=DIG_CODE[time_input->minute%10];
						break;
					case DISPLAY_MODEL_CLSM:
						if (temp_alarm>50)
						{
							GPIO_DIG=DIG_CODE[time_input->minute%10];
							temp_alarm++;
							if (temp_alarm>=100)temp_alarm=0;
							break;	
						}
						else
						{
							GPIO_DIG=DIG_CODE[12];
							temp_alarm++;
							break;
						}
					case DISPLAY_MODEL_AL:
						GPIO_DIG=DIG_CODE[time_input->minute%10];
						break;
					case DISPLAY_MODEL_ALS:
						GPIO_DIG=DIG_CODE[time_input->minute%10];
						break;
					case DISPLAY_MODEL_ALSH:
						GPIO_DIG=DIG_CODE[time_input->minute%10];
						break;
					case DISPLAY_MODEL_ALSM:
						if (temp_clock>50)
						{
							GPIO_DIG=DIG_CODE[time_input->minute%10];
							temp_clock++;
							if (temp_clock>=100)temp_clock=0;
							break;	
						}
						else
						{
							GPIO_DIG=DIG_CODE[12];
							temp_clock++;
							break;
						}
					default:
						break;
				}
				break;
			case(4)://分高位
				LSA=0;LSB=0;LSC=1; 
				GPIO_DIG=DIG_CODE[time_input->minute/10];
				break;
			case(5)://分隔符
				LSA=1;LSB=0;LSC=1; 
				GPIO_DIG=DIG_CODE[10];
				break;
			case(6)://时低位
				LSA=0;LSB=1;LSC=1;
				switch (time_input->display_model)
				{
					case DISPLAY_MODEL_CL:
						GPIO_DIG=DIG_CODE[time_input->hour%10];
						break;
					case DISPLAY_MODEL_CLS:
						GPIO_DIG=DIG_CODE[time_input->hour%10];
						break;
					case DISPLAY_MODEL_CLSH:
						if (temp_clock>50)
						{
							GPIO_DIG=DIG_CODE[time_input->hour%10];
							temp_clock++;
							if (temp_clock>=100)temp_clock=0;
							break;	
						}
						else
						{
							GPIO_DIG=DIG_CODE[12];
							temp_clock++;
							break;
						}
						break;
					case DISPLAY_MODEL_CLSM:
						GPIO_DIG=DIG_CODE[time_input->hour%10];
						break;
					case DISPLAY_MODEL_AL:
						GPIO_DIG=DIG_CODE[time_input->hour%10];
						break;
					case DISPLAY_MODEL_ALSH:
						if (temp_alarm>50)
						{
							GPIO_DIG=DIG_CODE[time_input->hour%10];
							temp_alarm++;
							if (temp_alarm>=100)temp_alarm=0;
							break;	
						}
						else
						{
							GPIO_DIG=DIG_CODE[12];
							temp_alarm++;
							break;
						}
						break;
					case DISPLAY_MODEL_ALSM:
						GPIO_DIG=DIG_CODE[time_input->hour%10];
						break;
					default:
						break;
				} 
				break;
			case(7)://时高位
				LSA=1;LSB=1;LSC=1; 
				GPIO_DIG=DIG_CODE[time_input->hour/10];
				break;	
			default:
				break;
		}
		if(i>7) i=0;
		j=10;
		while(j--) delay(1);
		GPIO_DIG=0x00;//消影
	}	
}
