
#include "stm32f10x.h"
#include "math.h"
#include "stdio.h"

u8  counter=0; 
int  pwm=100;
int flag=0;
int mode =0;
int velocity =0;
int turning=1;

void RCC_Configuration(void);	 //ʱ�ӳ�ʼ������������ʱ��
void GPIO_Configuration(void);	 //IO�ڳ�ʼ���������书��
void tim3(void);				 //��ʱ��tim4��ʼ������
void tim4(void);				 //��ʱ��tim4��ʼ������
void nvic(void);				 //�ж����ȼ�������
void exti(void);				 //�ⲿ�ж�����
void delay_nus(u32);           //72Mʱ���£�Լ��ʱus
void delay_nms(u32);            //72Mʱ���£�Լ��ʱms
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



void TIM4_IRQHandler(void)	 //TIM4����������ж���Ӧ���� ����ȡ��������ֵ�������������pwm��ռ�ձ�
{
	  
		u8 key_in1=0x01,key_in2=0x01;
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//	 ���TIM4����ж���Ӧ������־λ
   

		key_in1= GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12);	// ��PC12��״̬
		key_in2= GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13);// ��PC13��״̬
	
		if(key_in1 && key_in2) turning =1;
	
		breathing(velocity);
		

		if(key_in1==0 && turning){
				turning =0;
        velocity = (velocity + 1) % 3;
    }//���ٶ�
	
	
    if(key_in2==0 && turning){
				turning =0;
        mode = (mode + 1) % 3;
    }//����ɫ
}	

void TIM3_IRQHandler(void)		//	  //TIM3����������ж���Ӧ����������pwm��
{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	//	 //	 ���TIM3����ж���Ӧ������־λ

		if(counter==255)            //counter ��0��255�ۼ�ѭ��������ÿ��һ���жϣ�counter��һ
			counter = 0;
		else 
			counter +=1;

        if(mode == 0){
            if(counter < pwm)              //��counterֵС��pwmֵʱ����IO����Ϊ�ߣ���counterֵ���ڵ���pwmʱ����IO���õ�
		        GPIO_SetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1); //��PC14 PC15��Ϊ�ߵ�ƽ
            else 
						GPIO_ResetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1);	 //	��PC14 PC15��Ϊ�͵�ƽ
        }

        if(mode == 1)
        {
            if(counter < pwm)              //��counterֵС��pwmֵʱ����IO����Ϊ�ߣ���counterֵ���ڵ���pwmʱ����IO���õ�
		        GPIO_SetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_2); //��PC14 PC15��Ϊ�ߵ�ƽ
            else 
						GPIO_ResetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_2);	 //	��PC14 PC15��Ϊ�͵�ƽ
        
        }  
            
        if(mode ==2){
            if(counter < pwm)              //��counterֵС��pwmֵʱ����IO����Ϊ�ߣ���counterֵ���ڵ���pwmʱ����IO���õ�
		        GPIO_SetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_0); //��PC14 PC15��Ϊ�ߵ�ƽ
            else 
			    GPIO_ResetBits(GPIOA,GPIO_Pin_2|GPIO_Pin_0); //	��PC14 PC15��Ϊ�͵�ƽ
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


void delay_nus(u32 n)		//72Mʱ���£�Լ��ʱus
{
  u8 i;
  while(n--)
  {
    i=7;
    while(i--);
  }
}

void delay_nms(u32 n)	  //72Mʱ���£�Լ��ʱms
{
    while(n--)
      delay_nus(1000);
}


void RCC_Configuration(void)				 //ʹ���κ�һ������ʱ����ؿ�������Ӧ��ʱ��
{
  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);	  //ʹ��APB2���������ʱ�ӣ�����GPIOC, ���ܸ���ʱ��AFIO�ȣ�
                                                                              //�������������裬����̼����ֲ������
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM3, ENABLE); //ʹ��APB1���������ʱ�ӣ���ʱ��tim3��4��������������ֲ�

                
}


void GPIO_Configuration(void)			 //ʹ��ĳio���������ʱ������ض����ʼ������
{
    GPIO_InitTypeDef GPIO_InitStructure;   //�����ʽΪGPIO_InitTypeDef�Ľṹ�������ΪGPIO_InitStructure  
                                       	  //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 //����IO�ڵĹ���ģʽΪ�������루��io���ڲ���ӵ��赽��Դ��
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����IO����ߵ��������Ϊ50M
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;  //���ñ�ѡ�еĹܽţ�|��ʾͬʱ��ѡ��
    GPIO_Init(GPIOC, &GPIO_InitStructure);			        //��ʼ��GPIOC����ӦIO��Ϊ�������ã����ڰ������

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	   //����IO�ڹ���ģʽΪ	����������н�ǿ�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //����IO����ߵ��������Ϊ50M
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;	 //���ñ�ѡ�еĹܽţ�|��ʾͬʱ��ѡ��
    GPIO_Init(GPIOA, &GPIO_InitStructure);		  //��ʼ��GPIOA����ӦIO��Ϊ��������
	
	



    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//ʧ��STM32 JTAG��д���ܣ�ֻ����SWDģʽ��д����ų�PA15��PB�в���IO��
}

void tim4()							  //����TIM4Ϊ������ʱ��ģʽ,Լ10ms����һ�Σ�����Ƶ��Լ100Hz
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  //�����ʽΪTIM_TimeBaseInitTypeDef�Ľṹ�������ΪTIM_TimeBaseStructure  

  	TIM_TimeBaseStructure. TIM_Period =9999;		  // ���ü�����ֵΪ9999������ʱ���Զ����㣬�������ж�
		TIM_TimeBaseStructure.TIM_Prescaler =71;		 //	 ʱ��Ԥ��Ƶֵ�����Զ���
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	// ʱ�ӷ�Ƶ����
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	// ������ʽΪ���ϼ���

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);		 //	 ��ʼ��tim4
    TIM_ClearITPendingBit(TIM4,TIM_IT_Update); //���TIM4����жϱ�־
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);   //  ʹ��TIM4����������ж�
    TIM_Cmd(TIM4,ENABLE);				 //		   ʹ��TIM4
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
