
#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"
#include "NixieTube.h"


#define ADC1_DR_Address    ((uint32_t)0x4001244C)

u16 AD1_Value;
float resistence=0;


void RCC_Configuration(void);	 //时钟初始化，开启外设时钟
void GPIO_Configuration(void);	 //IO口初始化，配置其功能
void tim3(void);				 //定时器tim4初始化配置
void nvic(void);				 //中断优先级等配置
void ADC(void);

void assert_failed(uint8_t* file, uint32_t line)
{
 printf("Wrong parameters value: file %s on line %d\r\n", file, line);
 while(1);
}


 
float RESLIST[150] = {44.1201,42.1180,40.2121,38.3988,36.6746,35.0362,33.4802,32.0035,30.6028,29.2750,28.0170,
	26.8255,25.6972,24.6290,23.6176,22.6597,21.7522,20.8916,20.0749,19.2988,18.5600,
	18.4818,17.6316,16.9917,16.2797,15.5350,14.7867,14.0551,13.3536,12.6900,
	12.0684,11.4900,10.9539,10.4582,10.0000,9.5762,9.1835,8.8186,8.4784,8.1600,
	7.5785,7.3109,7.0564,6.8133,6.5806,6.3570,6.1418,5.9343,5.7340,
  5.5405,5.3534,5.1725,4.9976,4.8286}; //-10至45度热敏电阻阻值


int main(void)
{
    int kk=0;
    RCC_Configuration();															   		
    GPIO_Configuration();	
		ADC();
		nvic(); 
		tim3();
	
		Nixie_Configuration();
		delay_nms(500); 
		  
	while(1)
	{
			int ii,jj,ans=0,count=0;
			float deta,origin;
			for(ii=0;ii<55;ii++){		
				count =0;
				if(resistence <= RESLIST[ii] && resistence >= RESLIST[ii+1])
					{
            ans =(ii-10)*10 + 10*(RESLIST[ii] - resistence) /(RESLIST[ii] - RESLIST[ii+1]);
						break;
					}
			}
      for(kk=0;kk<200;kk++){
          NumDisplay(ans);
      } 
  }		
}	



void RCC_Configuration(void)				 //使用任何一个外设时，务必开启其相应的时钟
{
  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO, ENABLE);	  //使能APB2控制外设的时钟，包括GPIOA,ADC1, 功能复用时钟AFIO等，
                                                                              //其他包括的外设，详见固件库手册等资料
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //使能APB1控制外设的时钟，定时器tim3，其他外设详见手册

                
}


void GPIO_Configuration(void)			 //使用某io口输入输出时，请务必对其初始化配置
{
    GPIO_InitTypeDef GPIO_InitStructure;   //定义格式为GPIO_InitTypeDef的结构体的名字为GPIO_InitStructure  
                                       	  //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;
		 GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


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


void TIM3_IRQHandler(void)		//TIM3的溢出更新中断响应函数
{     
			float Vx;
	    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	 //清空TIM3溢出中断响应函数标志位
			ADC_SoftwareStartConvCmd(ADC1, ENABLE); //开始单次模式数据转换
	    AD1_Value=ADC_GetConversionValue(ADC1);
      Vx = (AD1_Value * 3.3) / 4095;
      resistence =(10*Vx) / (3.3-Vx);	//计算此时电阻
}			


void nvic()									//配置中断优先级
{	 
     NVIC_InitTypeDef NVIC_InitStructure;  //	 //	  命名一优先级变量

	 
		 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); //要用同一个Group
     NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3	溢出更新中断
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//	打断优先级为1，与上一个相同，不希望中断相互打断对方
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 	//	响应优先级1，低于上一个，当两个中断同时来时，上一个先执行
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

