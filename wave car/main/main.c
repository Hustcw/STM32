#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"
#include "usart1.h"
#include "NixieTube.h"
#include "SPI_NRF.h"

/*
PB0: 电机驱动PWM1,PB6:电机驱动PWM2
PB4: 舵机驱动PWM1,PB5:舵机驱动PWM2
PB13:电机驱动使能，PB14:舵机驱动使能
PA0: 舵机反馈线
舵机第一根接Vcc,第三根接GND

NRF24|01:
1:GND 2:3.3V
3:CE,PB12
4:CSN,PA4
5:SCK,PA5
6:MOSI,PA7
7:MISO,PA6
8:IRQ,PB1



*/

struct PID{  
int SetSpeed;         //定义设定目标电压  
int ActualSpeed;      //定义实际电压
int err;              //定义偏差值
int err_last;         //定义上一个偏差值  
float Kp,Ki,Kd;         //定义比例、积分、微分系数  
int voltage;          //定义电压值（控制执行器的变量）  
int integral;         //定义积分值  
}pid;  


u8 RxData=0;
u8 counter=0; 
u8 pwm=126;
u8 c=0;
u8 v=0;
u8 flag1=0;
u8 flag2=0;
u8 flag3=0;
u8 tag=0;
u16 AD1_Value;
uint8_t receive_direction[32]={50,50};
int dir_flag=1;
int angle=0;
int in1=0;
int in2=0;
float control=0;
int test=1;

void RCC_Configuration(void);	 
void GPIO_Configuration(void);	 
void tim3(void);
void tim2(void);
void tim4(void);				 
void nvic(void);				
void exti(void);	
void move(int dir,int speed);
void ADC(void);
int PID_realize(int speed);
void turn(int value);
void direction(int dir);

void assert_failed(uint8_t* file, uint32_t line)
{
 printf("Wrong parameters value: file %s on line %d\r\n", file, line);
 while(1);
}



 
int main(void)
{	
	int ii=0;
	int di=0;
	int mov1=0,mov2=0;	
	RCC_Configuration();															   		
  GPIO_Configuration();		
	Nixie_Configuration();	
	ADC();
	nvic();
	tim4();
	tim3();
	SPI_NRF_Init();
	TIM_SetCompare1(TIM3,0);
	TIM_SetCompare3(TIM3,0);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);
	NRF_RX_Mode();
	TIM_SetCompare1(TIM4,500);
	delay_nms(500); 	
	while(1)
	{
		if(receive_direction[2]==0){
				GPIO_SetBits(GPIOB,GPIO_Pin_14);
		}
		if(receive_direction[2]==1){
				GPIO_ResetBits(GPIOB,GPIO_Pin_14);
		}
		NRF_Rx_Dat(receive_direction);
		di=-14*receive_direction[0]+2800;
		mov1=(receive_direction[1]-49);
		mov2=mov1*20;
		if(receive_direction[1]-49>0){
				move(1,20*(receive_direction[1]-50));//前进
			}
		else{
			 move(0,20*(50-receive_direction[1]));//后退
		 }
		
		 for(ii=0;ii<2;ii++){
				NumDisplay(AD1_Value);
		}
		
		
		control=(di-(int)AD1_Value);
		if(control>100 || control<-100){
			if(control>0){
				direction(control);
			}
			else{
				direction(control);
			}
		}
		else{
			TIM_SetCompare2(TIM3,0);
			TIM_SetCompare1(TIM3,0);
		}	 
	}
}		



void direction(int speed)
{
	if(speed>0){
		TIM_SetCompare2(TIM3,speed);
		TIM_SetCompare1(TIM3,0);
	}
	else{
		TIM_SetCompare2(TIM3,0);
		TIM_SetCompare1(TIM3,-speed);
	}
}


void move(int dir,int speed)
{//dir正反分别为1和0   speed的值可从0~999  dir_flag为全局变量
	if(dir){//正转
		if(dir_flag){//之前正在正转
			TIM_SetCompare3(TIM3,speed);
			dir_flag=1;
		}
		else{//之前在反转
			TIM_SetCompare1(TIM4,0);
			delay_nms(100);
			TIM_SetCompare3(TIM3,speed);
			dir_flag=1;
		}
	}
	else{//反转
		if(!dir_flag){
			TIM_SetCompare1(TIM4,speed);
			dir_flag=0;
		}
		else{
			TIM_SetCompare3(TIM3,0);
			delay_nms(100);
			TIM_SetCompare1(TIM4,speed);
			dir_flag=0;
		}
	}
}


void RCC_Configuration(void)				 
{
  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO, ENABLE);	                                                                         
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4, ENABLE); 
}


void GPIO_Configuration(void)			
{
    GPIO_InitTypeDef GPIO_InitStructure;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	  //推挽复用输出，用作两路PWM输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_6;//TIM3_CH2对应PA7,TIM2_CH3对应PA2
    GPIO_Init(GPIOA, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	  //推挽复用输出，用作两路PWM输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_0;		//TIM3_CH3对应PB0
    GPIO_Init(GPIOB, &GPIO_InitStructure);		 
  
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_14|GPIO_Pin_13|GPIO_Pin_12;
    GPIO_Init(GPIOB, &GPIO_InitStructure);		 

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;		//模拟输入，ADC采样
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);	
}


void tim3()							 
{   
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	 //初始化计数配置   
	TIM_OCInitTypeDef  TIM_OCInitStructure;//初始化TIM3的外设
	TIM_TimeBaseStructure. TIM_Period =999;    //计数上限为1000
	TIM_TimeBaseStructure.TIM_Prescaler =71;	//计时器预分频为72/(71+1)=1MHZ   
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	 //不分
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 //向上计数
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		
    
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   //TIM脉冲宽度调制模式1  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;//设置了待装入捕获比较寄存器的脉冲值  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM输出比较极性高
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);//使能TIM3在CCR1上的预装载寄存器

	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);//使能TIM3在CCR1上的预装载寄存器

	TIM_OCInitStructure.TIM_Pulse =0;//设置了待装入捕获比较寄存器的脉冲值  
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);//使能TIM3在CCR1上的预装载寄存器
	TIM_Cmd(TIM3,ENABLE);					  
}

void tim4()
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	 
	TIM_OCInitTypeDef  TIM_OCInitStructure;//初始化TIM4的外设

  	TIM_TimeBaseStructure. TIM_Period =999;		  
	TIM_TimeBaseStructure.TIM_Prescaler =71;	  
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	 
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);		 
    TIM_ClearITPendingBit(TIM4,TIM_IT_Update); 
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); 

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   //TIM脉冲宽度调制模式1  
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;//设置了待装入捕获比较寄存器的脉冲值  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM输出比较极性高
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);//使能TIM3在CCR1上的预装载寄存器
    TIM_Cmd(TIM4,ENABLE);					 
}

void TIM4_IRQHandler(void)		
{     
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);	 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); 
	AD1_Value=ADC_GetConversionValue(ADC1);
}			

void nvic()									
{	 
     NVIC_InitTypeDef NVIC_InitStructure;  
		 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
     NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; 
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 	
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
}

void ADC()
{										 
  ADC_InitTypeDef ADC_InitStructure;
	
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;   
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;				
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              
  ADC_InitStructure.ADC_NbrOfChannel = 1;                          
  ADC_Init(ADC1, &ADC_InitStructure);                              

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,ADC_SampleTime_239Cycles5);  

  ADC_Cmd(ADC1, ENABLE);                           
  
  ADC_ResetCalibration(ADC1);                       
  while(ADC_GetResetCalibrationStatus(ADC1));        

  ADC_StartCalibration(ADC1);                        
  while(ADC_GetCalibrationStatus(ADC1));            
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}