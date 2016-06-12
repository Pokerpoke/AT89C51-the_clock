#include <reg51.h>

void timer0_init()//定时器初始化
{
	TMOD = 0x01; //选择工作方式1
    TH0 = (65536-50000)/256;	 //设置初始值
    TL0 = (65536-50000)%256; 
    EA = 1;			 //打开总中断
    ET0 = 1;		 //打开定时器0中断
    TR0 = 1;		 //启动定时器0
}

void int0_conf()//外部中断0的中断函数
{
	//int0 configuration
	IT0=1;//跳变沿出发方式（下降沿）
	EX0=1;//打开INT0的中断允许。
	EA=1;//打开总中断	
}
