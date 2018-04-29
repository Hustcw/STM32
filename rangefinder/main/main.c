#include "stm32f10x.h"
#include "stdio.h"
#include "NixieTube.h"
#include "delay.h"


int  count,j;


void RCC_Configuration(void);	 
void GPIO_Configuration(void);	 
void tim3(void);
void nvic(void);				 
void exti(void);				
void delay_nus(u32);           
void delay_nms(u32);            

void assert_failed(uint8_t* file, uint32_t line)
{
 printf("Wrong parameters value: file %s on line %d\r\n", file, line);
 while(1);
}

int main(void)
{
	RCC_Configuration();															   		
  	GPIO_Configuration();	
	tim3();
  	exti();	
	nvic();
	Nixie_Configuration();
	while(1){ 
		for(j=0;j<200;j++){
			NumDisplay(0.17*count);	
		}
		GPIO_SetBits(GPIOA,GPIO_Pin_2);
  		delay_nus(45);
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	}
}	


void EXTI1_IRQHandler(void)							  
{	   
	EXTI_ClearITPendingBit(EXTI_Line1);
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)){	
		TIM3->CNT=0;  
		TIM_Cmd(TIM3,ENABLE);
	}
	else{
		TIM_Cmd(TIM3, DISABLE);
		count=TIM3->CNT;
	}
}

void RCC_Configuration(void)				 
{
  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);	                                    
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
}


void tim3()							 
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	   

  	TIM_TimeBaseStructure. TIM_Period =60000;		 
	  TIM_TimeBaseStructure.TIM_Prescaler =71;	   
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		 
}

void nvic()								  
{	 
     NVIC_InitTypeDef NVIC_InitStructure;		
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 	 
     NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;  
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
     NVIC_Init(&NVIC_InitStructure);							
}


void GPIO_Configuration(void)			 
{
    GPIO_InitTypeDef GPIO_InitStructure;                                         	  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);			        
    	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);		
	
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);  

}

void exti()							 
{
  	EXTI_InitTypeDef EXTI_InitStructure;				  
 	EXTI_InitStructure.EXTI_Line = EXTI_Line1;		
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;       
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;     
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;                 
  	EXTI_Init(&EXTI_InitStructure);                            
}
