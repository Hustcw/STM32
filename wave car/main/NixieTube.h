#include "stm32f10x.h"
#include "math.h"
#include "delay.h"

void Nixie_Configuration(void);	 //对数码管使用的IO口等进行初始化
void NumDisplay(uint32_t  i);//在4位数码管上显示最高4位的整数数字
