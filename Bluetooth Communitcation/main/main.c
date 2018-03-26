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

void RCC_Configuration(void);	 //ʱ�ӳ�ʼ������������ʱ��
void GPIO_Configuration(void);	 //IO�ڳ�ʼ���������书��
void tim3(void);				 //��ʱ��tim4��ʼ������
void tim4(void);				 //��ʱ��tim4��ʼ������
void nvic(void);				 //�ж����ȼ�������
void exti(void);	

void TIM3_IRQHandler(void)		//	  //TIM3����������ж���Ӧ����������pwm��
{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	//	 //	 ���TIM3����ж���Ӧ������־λ

		if(counter==255)            //counter ��0��255�ۼ�ѭ��������ÿ��һ���жϣ�counter��һ
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
{	//�ó���Ϊ����ͨ��ʾ�����򣬵�Ƭ�����յ���������֮ǰ��һֱ��A���յ����ݺ󣬷�������
	
	USART1_Configuration();	//��ʼ������STM32����1ͨ��
	RCC_Configuration();															   		
    GPIO_Configuration();							
	nvic(); 
	tim3();
						
	
	while(1)
	{
	}	
}
void RCC_Configuration(void)				 //ʹ���κ�һ������ʱ����ؿ�������Ӧ��ʱ��
{
  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);	  //ʹ��APB2���������ʱ�ӣ�����GPIOC, ���ܸ���ʱ��AFIO�ȣ�
                                                                              //�������������裬����̼����ֲ������
  
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʹ��APB1���������ʱ�ӣ���ʱ��tim3��4��������������ֲ�

                
}


void GPIO_Configuration(void)			 //ʹ��ĳio���������ʱ������ض����ʼ������
{
    GPIO_InitTypeDef GPIO_InitStructure;   //�����ʽΪGPIO_InitTypeDef�Ľṹ�������ΪGPIO_InitStructure  
                                       	  //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //����IO�ڹ���ģʽΪ	����������н�ǿ�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //����IO����ߵ��������Ϊ50M
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;	 //���ñ�ѡ�еĹܽţ�|��ʾͬʱ��ѡ��
    GPIO_Init(GPIOC, &GPIO_InitStructure);		  //��ʼ��GPIOC����ӦIO��Ϊ�������ã�����led���
	
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//ʧ��STM32 JTAG��д���ܣ�ֻ����SWDģʽ��д����ų�PA15��PB�в���IO��
}


void tim3()							  //����TIM3Ϊ������ʱ��ģʽ ��Լ10us����һ�Σ�����Ƶ��Լ100kHz
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  //�����ʽΪTIM_TimeBaseInitTypeDef�Ľṹ�������ΪTIM_TimeBaseStructure  

  	TIM_TimeBaseStructure. TIM_Period =9;		  //���ü�����ֵΪ9������ʱ���Զ����㣬�������ж�
	  TIM_TimeBaseStructure.TIM_Prescaler =71;	   //	 ʱ��Ԥ��Ƶֵ�����Զ���
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	 // ʱ�ӷ�Ƶ����
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 // ������ʽΪ���ϼ���

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		 //	 ��ʼ��tim3
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //���TIM3����жϱ�־
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //  ʹ��TIM3����������ж�
    TIM_Cmd(TIM3,ENABLE);					  //		   ʹ��TIM3
}


void nvic()									//�����ж����ȼ�
{	 
     NVIC_InitTypeDef NVIC_InitStructure;  //	 //	  ����һ���ȼ�����

 	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);    //	   �����ȼ����鷽ʽ����Ϊgroup1,��2����ռ����ϣ����ȼ���8����Ӧ���ȼ�
     NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; //���ж�ΪTIM4��������ж�
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//������ȼ�Ϊ1���ڸ�����Ϊ�ϵ͵ģ�0���ȼ����
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // ��Ӧ���ȼ�0��������ȼ�һ��ʱ��0���
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	 	//	����ʹ��
     NVIC_Init(&NVIC_InitStructure);					   	//	��ʼ��

	 
 	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //Ҫ��ͬһ��Group
     NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3	��������ж�
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//	������ȼ�Ϊ1������һ����ͬ����ϣ���ж��໥��϶Է�
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 	//	��Ӧ���ȼ�1��������һ�����������ж�ͬʱ��ʱ����һ����ִ��
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
 
}
