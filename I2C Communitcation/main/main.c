
#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"
#include "MPU6050.h"
#include "NixieTube.h"
#include "math.h"

u8 counter=0;
int pwm=120;
ACCEL AccelValue;
float angle = 0;
void assert_failed(uint8_t* file, uint32_t line)
{
 printf("Wrong parameters value: file %s on line %d\r\n", file, line);
 while(1);
}

void GPIO_Configuration(void);
void nvic()									
{	 
     NVIC_InitTypeDef NVIC_InitStructure;  
	
		 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
     NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 	
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure); 
}

void tim3()							 
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  
  	TIM_TimeBaseStructure. TIM_Period =9;		 
	  TIM_TimeBaseStructure.TIM_Prescaler =71;	   
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	 
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		 
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); 
    TIM_Cmd(TIM3,ENABLE);					
}

void TIM3_IRQHandler(void)		
{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	

		if(counter==255)           
			counter = 0;

		else 
			counter +=1;

		if(counter < pwm){
			GPIO_SetBits(GPIOA, GPIO_Pin_2);
		}
		else{
			GPIO_ResetBits(GPIOA,GPIO_Pin_2);
		}
}  

void RCC_Configuration(void)				 
{	                                                                             
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
}

int main(void)
{
	int jj;
	RCC_Configuration();
	Nixie_Configuration();
	GPIO_Configuration();
	MPU6050_Init();
	nvic(); 
	tim3();
	
	delay_nms(500);		
	
	while(1)
	{
		AccelValue=MPU6050_GetValue_ACCEL();
		angle = atan((float)AccelValue.x/(float)AccelValue.z);
		if(angle > 0){
			GPIO_SetBits(GPIOA,GPIO_Pin_0);
			GPIO_ResetBits(GPIOA,GPIO_Pin_1);	
		} 
		if(angle < 0){
			GPIO_SetBits(GPIOA,GPIO_Pin_1);
			GPIO_ResetBits(GPIOA,GPIO_Pin_0);	
		} 
		pwm = angle > 0 ? (int)(angle * 255 * 2 /3.1415926 ): -(int)(angle * 255 * 2/3.1415926);
		angle = angle * 180 / 3.1415926535;
		for(jj=0;jj<50;jj++) NumDisplay((int)(angle*10));
  	}	
}	 



void GPIO_Configuration(void)			 //????io????????????????????????????
{
    GPIO_InitTypeDef GPIO_InitStructure;   //???????GPIO_InitTypeDef???????????GPIO_InitStructure  
                                       	  //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //????IO????????	??????????§ß?????????????
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //????IO??????????????50M
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;	 //???????§Ö????|??????????
    GPIO_Init(GPIOA, &GPIO_InitStructure);		  //?????GPIOC?????IO????????????????led??
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//???STM32 JTAG??§Õ??????????SWD????§Õ??????PA15??PB?§Ó???IO??
}



