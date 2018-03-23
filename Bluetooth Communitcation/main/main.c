#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"
#include "usart1.h"

u8 RxData=0;
u8 counter=0; 
u8 pwm=126;
u8 c=0;
u8 v=0;
u8 flag1=0;
u8 flag2=0;
u8 flag3=0;
u8 tag=0;

void RCC_Configuration(void);	 //时钟初始化，开启外设时钟
void GPIO_Configuration(void);	 //IO口初始化，配置其功能
void tim3(void);				 //定时器tim4初始化配置
void tim4(void);				 //定时器tim4初始化配置
void nvic(void);				 //中断优先级等配置
void exti(void);	

void TIM3_IRQHandler(void)		//	  //TIM3的溢出更新中断响应函数，产生pwm波
{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	//	 //	 清空TIM3溢出中断响应函数标志位

		if(counter==255)            //counter 从0到255累加循环计数，每进一次中断，counter加一
			counter = 0;
		else 
			counter +=1;

		if(counter < pwm){
			GPIO_SetBits(GPIOC, GPIO_Pin_14 | GPIO_Pin_15);
		}
		else{
			GPIO_ResetBits(GPIOC,GPIO_Pin_14|GPIO_Pin_15);
		}
}  
	

void assert_failed(uint8_t* file, uint32_t line)
{
 printf("Wrong parameters value: file %s on line %d\r\n", file, line);
 while(1);
}


 
int main(void)
{	//该程序为串口通信示例程序，单片机在收到串口数据之前，一直发A，收到数据后，发该数据
	
	USART1_Configuration();	//初始化配置STM32串口1通信
	RCC_Configuration();															   		
    GPIO_Configuration();							
	nvic(); 
	tim3();
						
	
	while(1)
	{
	}	
}
void RCC_Configuration(void)				 //使用任何一个外设时，务必开启其相应的时钟
{
  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);	  //使能APB2控制外设的时钟，包括GPIOC, 功能复用时钟AFIO等，
                                                                              //其他包括的外设，详见固件库手册等资料
  
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //使能APB1控制外设的时钟，定时器tim3、4，其他外设详见手册

                
}


void GPIO_Configuration(void)			 //使用某io口输入输出时，请务必对其初始化配置
{
    GPIO_InitTypeDef GPIO_InitStructure;   //定义格式为GPIO_InitTypeDef的结构体的名字为GPIO_InitStructure  
                                       	  //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //配置IO口工作模式为	推挽输出（有较强的输出能力）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //配置IO口最高的输出速率为50M
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;	 //配置被选中的管脚，|表示同时被选中
    GPIO_Init(GPIOC, &GPIO_InitStructure);		  //初始化GPIOC的相应IO口为上述配置，用于led输出
	
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//失能STM32 JTAG烧写功能，只能用SWD模式烧写，解放出PA15和PB中部分IO口
}


void tim3()							  //配置TIM3为基本定时器模式 ，约10us触发一次，触发频率约100kHz
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  //定义格式为TIM_TimeBaseInitTypeDef的结构体的名字为TIM_TimeBaseStructure  

  	TIM_TimeBaseStructure. TIM_Period =9;		  //配置计数阈值为9，超过时，自动清零，并触发中断
	  TIM_TimeBaseStructure.TIM_Prescaler =71;	   //	 时钟预分频值，除以多少
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	 // 时钟分频倍数
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 // 计数方式为向上计数

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		 //	 初始化tim3
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除TIM3溢出中断标志
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //  使能TIM3的溢出更新中断
    TIM_Cmd(TIM3,ENABLE);					  //		   使能TIM3
}


void nvic()									//配置中断优先级
{	 
     NVIC_InitTypeDef NVIC_InitStructure;  //	 //	  命名一优先级变量

 	   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);    //	   将优先级分组方式配置为group1,有2个抢占（打断）优先级，8个响应优先级
     NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; //该中断为TIM4溢出更新中断
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//打断优先级为1，在该组中为较低的，0优先级最高
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // 响应优先级0，打断优先级一样时，0最高
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	 	//	设置使能
     NVIC_Init(&NVIC_InitStructure);					   	//	初始化

	 
 	   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //要用同一个Group
     NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3	溢出更新中断
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//	打断优先级为1，与上一个相同，不希望中断相互打断对方
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 	//	响应优先级1，低于上一个，当两个中断同时来时，上一个先执行
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
 
}
