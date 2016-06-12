#include <reg51.h>
#include "time_struct.h"
#include "delay.h"
#include "dig_display.h"
#include "timer.h"
#include "beep.h"

#define GPIO_KEY P1

enum
{
	PLACE_MINUTE=0,
	PLACE_HOUR=1,
}CURSOR_PLACE=PLACE_MINUTE;

enum
{
	CLOCK=0,
	ALARM=1,
	CLOCK_SET=2,
	ALARM_SET=3,
}CLOCK_MODEL;

enum
{
	ALARM_DISABLE=0,
	ALARM_ENABLE=1,
}ALARM_STATUS;

enum
{
	KEY_VALUE_NULL=0,
	KEY_VALUE_K1=1,
	KEY_VALUE_K2=2,
	KEY_VALUE_K3=3,
	KEY_VALUE_K4=4,
}KEY_VALUE=KEY_VALUE_NULL;

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

void key_down();
void check_clock_mode();
void alarm();

time 	time_clock={0,0,0,DISPLAY_MODEL_CL},
		time_alarm={0,1,0,DISPLAY_MODEL_CL},
		time_display={0,0,DISPLAY_MODEL_CL};//initialize the time of clock and alarm
unsigned int i=0;

int main()
{
	timer0_init();
	int0_conf();
	while (1)
	{
		key_down();
		check_clock_mode();
		alarm();
		dig_display(&time_display);
	}
	return 0;
}

void alarm()
{
	if (time_clock.hour==time_alarm.hour&&time_clock.second==time_alarm.second&&ALARM_STATUS==ALARM_ENABLE&&KEY_VALUE!=KEY_VALUE_NULL)
	{
		beep();
	}
}

void timer0() interrupt 1//time0 interrupt
{ 
	TH0 = (65536-50000)/256;	 //set the initial value
    TL0 = (65536-50000)%256;
    i++;
    if (i==20)
    {
    	i=0;
    	time_clock.second++;
    }
	if(time_clock.second>=60)time_clock.second=0,time_clock.minute++;
	if(time_clock.minute>=60)time_clock.minute=0,time_clock.hour++;
	if(time_clock.hour>=24)time_clock.hour=0;
}

void check_clock_mode()
{
	switch (CLOCK_MODEL)
	{
		case CLOCK:
			time_display=time_clock;
			time_display.display_model=DISPLAY_MODEL_CL;
			ALARM_STATUS=ALARM_ENABLE;
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

void key_down()
{
	int a=0;
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)
  	{
		delay(100);
		if(GPIO_KEY!=0x0f)
		{
			//test row
			GPIO_KEY=0X0F;
			delay(5);
			switch(GPIO_KEY)
			{
				case(0X07):	//key1,set model
					KEY_VALUE=KEY_VALUE_K1;//key to set model
					switch (CLOCK_MODEL)
					{
						case CLOCK:
							//convert to clock_set model
							ET0=0;
							time_clock.second=0;
							CURSOR_PLACE=PLACE_MINUTE;
							CLOCK_MODEL=CLOCK_SET;
							break;
						case ALARM:
							//convert to alarm_set model
							ET0=1;
							CURSOR_PLACE=PLACE_MINUTE;
							CLOCK_MODEL=ALARM_SET;
							break;
						case CLOCK_SET:
							//get back
							ET0=1;
							CLOCK_MODEL=CLOCK;
							break;
						case ALARM_SET:
							//get back
							ET0=1;
							CLOCK_MODEL=ALARM;
							break;
						default:
							break;
					}
					break;
				case(0X0b):	//key2,change the minute and hour
					KEY_VALUE=KEY_VALUE_K2;
					switch(CLOCK_MODEL)
					{
						case CLOCK_SET:
							switch(CURSOR_PLACE)
							{
								case PLACE_MINUTE:
									time_clock.minute++;
									if (time_clock.minute>=60)time_clock.minute=0;
									delay(10);
									break;
								case PLACE_HOUR:
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
				case(0X0d): //key3,set cursor place
					KEY_VALUE=KEY_VALUE_K3;//cursor set
					switch (CURSOR_PLACE)
					{
						case PLACE_MINUTE://convert to hour
							CURSOR_PLACE=PLACE_HOUR;
							break;
						case PLACE_HOUR://convert to NULL,circulation
							CURSOR_PLACE=PLACE_MINUTE;
							break;
						default:
							break;
					}
					break;
				case(0X0e):	//key3,change clock an alarm model
					KEY_VALUE=KEY_VALUE_K4;//clock alarm convert key
					switch (CLOCK_MODEL)
					{
						case CLOCK:
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
			while ((a<50)&&GPIO_KEY!=0xf0)//check if loosen the key
			{
				delay(100);
				a++;
			}
		}
	}
}
