#include "stdio.h"
#include "stm32f10x.h"

void RCC_Configuration(void);  //ʱ�ӳ�ʼ������������ʱ��
void GPIO_Configuration(void); //IO�ڳ�ʼ���������书��
void delay_nus(u32);           //72Mʱ���£�Լ��ʱus
void delay_nms(u32);           //72Mʱ���£�Լ��ʱms
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
        //GPIO_SetBits(GPIOC, GPIO_Pin_12 | GPIO_Pin_13); //	 ��������GPIOC��BSRR�Ĵ���
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

void delay_nus(u32 n) //72Mʱ���£�Լ��ʱus
{
    u8 i;
    while (n--)
    {
        i = 7;
        while (i--)
            ;
    }
}

void delay_nms(u32 n) //72Mʱ���£�Լ��ʱms
{
    while (n--)
        delay_nus(1000);
}

void RCC_Configuration(void) //ʹ���κ�һ������ʱ����ؿ�������Ӧ��ʱ��
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //ʹ��APB2���������ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

void GPIO_Configuration(void) //ʹ��ĳio���������ʱ������ض����ʼ������
{
    GPIO_InitTypeDef GPIO_InitStructureA; //�����ʽΪGPIO_InitTypeDef�Ľṹ�������ΪGPIO_InitStructure
                                          //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;
    GPIO_InitTypeDef GPIO_InitStructureC;

    GPIO_InitStructureA.GPIO_Mode = GPIO_Mode_Out_PP;  //����IO�ڹ���ģʽΪ	����������н�ǿ�����������
    GPIO_InitStructureA.GPIO_Speed = GPIO_Speed_50MHz; //����IO����ߵ��������Ϊ50M
    GPIO_InitStructureA.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOA, &GPIO_InitStructureA); //��ʼ��GPIOA����ӦIO��Ϊ�������ã�����led���

    GPIO_InitStructureC.GPIO_Mode = GPIO_Mode_IPU;  
    GPIO_InitStructureC.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructureC.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStructureC); //��ʼ��GPIOC����ӦIO��Ϊ�������ã�����led���
}
