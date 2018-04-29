#include "stdio.h"
#include "stm32f10x.h"

void RCC_Configuration(void);  //时钟初始化，开启外设时钟
void GPIO_Configuration(void); //IO口初始化，配置其功能
void delay_nus(u32);           //72M时钟下，约延时us
void delay_nms(u32);           //72M时钟下，约延时ms
void LedFloating(int* mode_control, int* velocity_control,int* flag);
void assert_failed(uint8_t *file, uint32_t line)
{
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
    while (1)
        ;
}

int GPIO_Pins[6] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5};

int main(void)
{
    int mode_control = 0, velocity_control = 0, flag = 0;
    RCC_Configuration();
    GPIO_Configuration();

    while (1)
    {
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) && GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13))
				{
					flag = 1;
		        }
        //GPIO_SetBits(GPIOC, GPIO_Pin_12 | GPIO_Pin_13); //	 函数设置GPIOC的BSRR寄存器
        LedFloating(&mode_control, &velocity_control,&flag);
    }
}

void ignite_A(int *velocity_control,int *mode_control,int *flag)
{
    int ii,jj;
    if (!(*velocity_control))
    {
        for (ii = 0; ii < 6; ii++)
        {
            GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
            GPIO_SetBits(GPIOA, GPIO_Pins[ii]);
			for(jj = 0; jj < 20 ; jj++)
			{
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == 0 && *flag)
				{
					*flag = 0;
					*mode_control = (*mode_control + 1) % 2;
					return;
				}
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13) == 0 && *flag)
				{
					*flag = 0;
					*velocity_control = (*velocity_control + 1) % 2;
					return;
				}								
				delay_nms(5);
		    }
        }
    }
    else
    {
        for (ii = 0; ii < 6; ii++)
        {
            GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
            GPIO_SetBits(GPIOA, GPIO_Pins[ii]);
            for(jj = 0; jj < 20 ; jj++){
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == 0 && *flag){
					*flag = 0;
					*mode_control = (*mode_control + 1) % 2;
						return;
				}
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13) == 0 && *flag){
					*flag = 0;
					*velocity_control = (*velocity_control + 1) % 2;
					return;
				}								
				delay_nms(15);
		    }
        }
    }
}

void ignite_B(int *velocity_control,int * mode_control,int* flag)
{
    int ii,jj;
    if (!(*velocity_control))
    {
        for (ii = 0; ii < 6; ii++)
        {
            GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
            GPIO_SetBits(GPIOA, GPIO_Pins[ii]);
            GPIO_SetBits(GPIOA, GPIO_Pins[5 - ii]);
            for(jj = 0; jj < 20 ; jj++){
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == 0 && *flag)
					
                {
					*flag = 0;
					*mode_control = (*mode_control + 1) % 2;
					return;
				}
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13) == 0 && *flag)
				{
					*flag = 0;
					*velocity_control = (*velocity_control + 1) % 2;
					return;
				}								
					delay_nms(5);
			}
        }
    }

    else
    {
        for (ii = 0; ii < 6; ii++)
        {
            GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
            GPIO_SetBits(GPIOA, GPIO_Pins[ii]);
            GPIO_SetBits(GPIOA, GPIO_Pins[5 - ii]);
            for(jj = 0; jj < 20 ; jj++)
			{
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) == 0 && *flag)
				{
					*flag = 0;
					*mode_control = (*mode_control + 1) % 2;
					return;
				}
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13) == 0 && *flag)
				{
					*flag = 0;
				    *velocity_control = (*velocity_control + 1) % 2;
					return;
				}								
				delay_nms(15);
			}
        }
    }
}

void LedFloating(int* mode_control, int* velocity_control,int *flag)
{
    if ((*mode_control) == 0)
        ignite_A(velocity_control,mode_control,flag);
    else
        ignite_B(velocity_control,mode_control,flag);
}

void delay_nus(u32 n) //72M时钟下，约延时us
{
    u8 i;
    while (n--)
    {
        i = 7;
        while (i--)
            ;
    }
}

void delay_nms(u32 n) //72M时钟下，约延时ms
{
    while (n--)
        delay_nus(1000);
}

void RCC_Configuration(void) //使用任何一个外设时，务必开启其相应的时钟
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //使能APB2控制外设的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

void GPIO_Configuration(void) //使用某io口输入输出时，请务必对其初始化配置
{
    GPIO_InitTypeDef GPIO_InitStructureA; //定义格式为GPIO_InitTypeDef的结构体的名字为GPIO_InitStructure
                                          //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;
    GPIO_InitTypeDef GPIO_InitStructureC;

    GPIO_InitStructureA.GPIO_Mode = GPIO_Mode_Out_PP;  //配置IO口工作模式为	推挽输出（有较强的输出能力）
    GPIO_InitStructureA.GPIO_Speed = GPIO_Speed_50MHz; //配置IO口最高的输出速率为50M
    GPIO_InitStructureA.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOA, &GPIO_InitStructureA); //初始化GPIOA的相应IO口为上述配置，用于led输出

    GPIO_InitStructureC.GPIO_Mode = GPIO_Mode_IPU;  
    GPIO_InitStructureC.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructureC.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStructureC); //初始化GPIOC的相应IO口为上述配置，用于led输出
}
