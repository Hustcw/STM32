
#include "stm32f10x.h"
#include "math.h"
#include "stdio.h"

u8  counter=0; 
int  pwm=100;
int flag=0;
int mode =0;
int velocity =0;
int turning=1;

void RCC_Configuration(void);	 //时钟初始化，开启外设时钟
void GPIO_Configuration(void);	 //IO口初始化，配置其功能
void tim3(void);				 //定时器tim4初始化配置
void tim4(void);				 //定时器tim4初始化配置
void nvic(void);				 //中断优先级等配置
void exti(void);				 //外部中断配置
void delay_nus(u32);           //72M时钟下，约延时us
void delay_nms(u32);            //72M时钟下，约延时ms
void breathing(int velocity){
		switch(velocity){
				case 0:
					if(flag)
							pwm +=1;
							if(pwm>240) flag=0;
					if(flag == 0){
							pwm -=1;
							if(pwm<10) flag=1;
					}
					break;
					
				case 1:
					if(flag)
							pwm +=2;
							if(pwm>240) flag=0;
					if(flag == 0){
							pwm -=2;
							if(pwm<10) flag=1;
					}
					break;
				
				case 2:
					if(flag)
							pwm +=3;
							if(pwm>240) flag=0;
					if(flag == 0){
							pwm -=3;
							if(pwm<10) flag=1;
					}
					break;
		}
}

void assert_failed(uint8_t* file, uint32_t line)
{
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
    while(1);
}



void TIM4_IRQHandler(void)	 //TIM4的溢出更新中断响应函数 ，读取按键输入值，根据输入控制pwm波占空比
{
	  
		u8 key_in1=0x01,key_in2=0x01;
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//	 清空TIM4溢出中断响应函数标志位
   

		key_in1= GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12);	// 读PC12的状态
		key_in2= GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13);// 读PC13的状态
	
		if(key_in1 && key_in2) turning =1;
	
		breathing(velocity);
		

		if(key_in1==0 && turning){
				turning =0;
        velocity = (velocity + 1) % 3;
    }//调速度
	
	
    if(key_in2==0 && turning){
				turning =0;
        mode = (mode + 1) % 3;
    }//调颜色
}	

void TIM3_IRQHandler(void)		//	  //TIM3的溢出更新中断响应函数，产生pwm波
{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	//	 //	 清空TIM3溢出中断响应函数标志位

		if(counter==255)            //counter 从0到255累加循环计数，每进一次中断，counter加一
			counter = 0;
		else 
			counter +=1;

        if(mode == 0){
            if(counter < pwm)              //当counter值小于pwm值时，将IO口设为高；当counter值大于等于pwm时，将IO口置低
		        GPIO_SetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1); //将PC14 PC15置为高电平
            else 
						GPIO_ResetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1);	 //	将PC14 PC15置为低电平
        }

        if(mode == 1)
        {
            if(counter < pwm)              //当counter值小于pwm值时，将IO口设为高；当counter值大于等于pwm时，将IO口置低
		        GPIO_SetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_2); //将PC14 PC15置为高电平
            else 
						GPIO_ResetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_2);	 //	将PC14 PC15置为低电平
        
        }  
            
        if(mode ==2){
            if(counter < pwm)              //当counter值小于pwm值时，将IO口设为高；当counter值大于等于pwm时，将IO口置低
		        GPIO_SetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_0); //将PC14 PC15置为高电平
            else 
			    GPIO_ResetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_0); //	将PC14 PC15置为低电平
        }
}	


	


int main(void)
{

	RCC_Configuration();															   		
  GPIO_Configuration();							
	tim4();
	tim3();
  nvic(); 
						

	while(1)
	{ 
	}	

}	


void delay_nus(u32 n)		//72M时钟下，约延时us
{
  u8 i;
  while(n--)
  {
    i=7;
    while(i--);
  }
}

void delay_nms(u32 n)	  //72M时钟下，约延时ms
{
    while(n--)
      delay_nus(1000);
}


void RCC_Configuration(void)				 //使用任何一个外设时，务必开启其相应的时钟
{
  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);	  //使能APB2控制外设的时钟，包括GPIOC, 功能复用时钟AFIO等，
                                                                              //其他包括的外设，详见固件库手册等资料
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM3, ENABLE); //使能APB1控制外设的时钟，定时器tim3、4，其他外设详见手册

                
}


void GPIO_Configuration(void)			 //使用某io口输入输出时，请务必对其初始化配置
{
    GPIO_InitTypeDef GPIO_InitStructure;   //定义格式为GPIO_InitTypeDef的结构体的名字为GPIO_InitStructure  
                                       	  //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 //配置IO口的工作模式为上拉输入（该io口内部外接电阻到电源）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //配置IO口最高的输出速率为50M
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;  //配置被选中的管脚，|表示同时被选中
    GPIO_Init(GPIOC, &GPIO_InitStructure);			        //初始化GPIOC的相应IO口为上述配置，用于按键检测

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	   //配置IO口工作模式为	推挽输出（有较强的输出能力）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //配置IO口最高的输出速率为50M
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;	 //配置被选中的管脚，|表示同时被选中
    GPIO_Init(GPIOA, &GPIO_InitStructure);		  //初始化GPIOA的相应IO口为上述配置
	
	



    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//失能STM32 JTAG烧写功能，只能用SWD模式烧写，解放出PA15和PB中部分IO口
}

void tim4()							  //配置TIM4为基本定时器模式,约10ms触发一次，触发频率约100Hz
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  //定义格式为TIM_TimeBaseInitTypeDef的结构体的名字为TIM_TimeBaseStructure  

  	TIM_TimeBaseStructure. TIM_Period =9999;		  // 配置计数阈值为9999，超过时，自动清零，并触发中断
		TIM_TimeBaseStructure.TIM_Prescaler =71;		 //	 时钟预分频值，除以多少
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	// 时钟分频倍数
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	// 计数方式为向上计数

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);		 //	 初始化tim4
    TIM_ClearITPendingBit(TIM4,TIM_IT_Update); //清除TIM4溢出中断标志
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);   //  使能TIM4的溢出更新中断
    TIM_Cmd(TIM4,ENABLE);				 //		   使能TIM4
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
