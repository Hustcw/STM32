
#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"

#define ADC1_DR_Address    ((uint32_t)0x4001244C)

u16 AD1_Value;
int flag=0;
int key=1;

void RCC_Configuration(void);	 //时钟初始化，开启外设时钟
void GPIO_Configuration1(void);	 //IO口初始化，配置其功能
void GPIO_Configuration2(void);
void tim3(void);				 //定时器tim4初始化配置
void nvic(void);				 //中断优先级等配置
void ADC(void);

void assert_failed(uint8_t* file, uint32_t line)
{
 printf("Wrong parameters value: file %s on line %d\r\n", file, line);
 while(1);
}


 


void TIM3_IRQHandler(void)		//	  //TIM3的溢出更新中断响应函数
{
	    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	//	 //	 清空TIM3溢出中断响应函数标志位
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}			


int main(void)
{
  RCC_Configuration();															   		
  GPIO_Configuration1();	
	GPIO_Configuration2();	
	ADC();
	nvic(); 
	tim3();
	delay_nms(500); 
	key=1;
	while(1){
		key= GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1);
		AD1_Value=ADC_GetConversionValue(ADC1);
	  if(key==1){
			if(AD1_Value > 2100){
						flag=1;
			}
		}
		if(flag){
        GPIO_SetBits(GPIOA,GPIO_Pin_2);
        delay_nms(3000);
        flag=0;
    }
		else GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	}				  
}	


void RCC_Configuration(void)				 //使用任何一个外设时，务必开启其相应的时钟
{
  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO, ENABLE);	  //使能APB2控制外设的时钟，包括GPIOA,ADC1, 功能复用时钟AFIO等，
                                                                              //其他包括的外设，详见固件库手册等资料
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM3, ENABLE); //使能APB1控制外设的时钟，定时器tim3，其他外设详见手册

                
}


void GPIO_Configuration1(void)			 //使用某io口输入输出时，请务必对其初始化配置
{
    GPIO_InitTypeDef GPIO_InitStructure;   //定义格式为GPIO_InitTypeDef的结构体的名字为GPIO_InitStructure  
                                       	  //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;
		GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	


    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//失能STM32 JTAG烧写功能，只能用SWD模式烧写，解放出PA15和PB中部分IO口
}
void GPIO_Configuration2(void)			 //使用某io口输入输出时，请务必对其初始化配置
{
    GPIO_InitTypeDef GPIO_InitStructure;   //定义格式为GPIO_InitTypeDef的结构体的名字为GPIO_InitStructure  
                                       	  //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 //配置IO口的工作模式为上拉输入（该io口内部外接电阻到电源）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //配置IO口最高的输出速率为50M
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  //配置被选中的管脚，|表示同时被选中
    GPIO_Init(GPIOA, &GPIO_InitStructure);			        //初始化GPIOC的相应IO口为上述配置，用于按键检测

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //配置IO口工作模式为	推挽输出（有较强的输出能力）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //配置IO口最高的输出速率为50M
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	 //配置被选中的管脚，|表示同时被选中
    GPIO_Init(GPIOA, &GPIO_InitStructure);		  //初始化GPIOC的相应IO口为上述配置，用于led输出
	
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//失能STM32 JTAG烧写功能，只能用SWD模式烧写，解放出PA15和PB中部分IO口
}


void tim3()							  //配置TIM3为基本定时器模式 ，约100Hz
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  //定义格式为TIM_TimeBaseInitTypeDef的结构体的名字为TIM_TimeBaseStructure  

  	TIM_TimeBaseStructure. TIM_Period =9999;		  //配置计数阈值为9999，超过时，自动清零，并触发中断
		TIM_TimeBaseStructure.TIM_Prescaler =71;	   //	 时钟预分频值，定时器的计数的频率等于主时钟频率除以该预分频值
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	 // 时钟分频倍数(用于数字滤波，暂时无用)
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 // 计数方式为向上计数

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		 //	 初始化tim3
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除TIM3溢出中断标志
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //  使能TIM3的溢出更新中断
    TIM_Cmd(TIM3,ENABLE);					  //		   使能TIM3
}


void nvic()									//配置中断优先级
{	 
     NVIC_InitTypeDef NVIC_InitStructure;  //	 //	  命名一优先级变量

	 
		 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); 
     NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 	
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
	
 
}
void ADC()
{										 
	ADC_InitTypeDef ADC_InitStructure;
	
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;   //设置ADC1和ADC2工作在独立工作模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;				// 设置ADC工作在扫描模式（多通道），如果只需要一个通道，配置为单通道模式 DISABLE
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //设置ADC工作在单次模式（使能一次，转换一次；也可以配置连续模式，自动周期性采样
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //设置转换由软件触发，不用外部触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              //ADC数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 1;                          //设置进行转换的通道数目为5
  ADC_Init(ADC1, &ADC_InitStructure);                              //初始化ADC1

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,ADC_SampleTime_239Cycles5);  //设置ADC1的通道0第一个进行转换，采样时钟周期239.5个

  ADC_Cmd(ADC1, ENABLE);                           //使能ADC1
  
  ADC_ResetCalibration(ADC1);                        //重置ADC1的校准寄存器
  while(ADC_GetResetCalibrationStatus(ADC1));        //等待ADC1校准重置完成

  ADC_StartCalibration(ADC1);                        //开始ADC1校准
  while(ADC_GetCalibrationStatus(ADC1));             //等待ADC1校准完成
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);            //使能ADC1软件开始转换
}

