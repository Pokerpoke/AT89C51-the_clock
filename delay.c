#include <reg51.h>

void delay (unsigned int i)
//延时10us
{
	unsigned int a,b;
	for(;i>0;i--)
		for(b=1;b>0;b--)
			for(a=2;a>0;a--);
}
