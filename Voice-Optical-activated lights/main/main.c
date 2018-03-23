
#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"

#define ADC1_DR_Address    ((uint32_t)0x4001244C)

u16 AD1_Value;
int flag=0;
int key=1;

void RCC_Configuration(void);	 //ʱ�ӳ�ʼ������������ʱ��
void GPIO_Configuration1(void);	 //IO�ڳ�ʼ���������书��
void GPIO_Configuration2(void);
void tim3(void);				 //��ʱ��tim4��ʼ������
void nvic(void);				 //�ж����ȼ�������
void ADC(void);

void assert_failed(uint8_t* file, uint32_t line)
{
 printf("Wrong parameters value: file %s on line %d\r\n", file, line);
 while(1);
}


 


void TIM3_IRQHandler(void)		//	  //TIM3����������ж���Ӧ����
{
	    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	//	 //	 ���TIM3����ж���Ӧ������־λ
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


void RCC_Configuration(void)				 //ʹ���κ�һ������ʱ����ؿ�������Ӧ��ʱ��
{
  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO, ENABLE);	  //ʹ��APB2���������ʱ�ӣ�����GPIOA,ADC1, ���ܸ���ʱ��AFIO�ȣ�
                                                                              //�������������裬����̼����ֲ������
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM3, ENABLE); //ʹ��APB1���������ʱ�ӣ���ʱ��tim3��������������ֲ�

                
}


void GPIO_Configuration1(void)			 //ʹ��ĳio���������ʱ������ض����ʼ������
{
    GPIO_InitTypeDef GPIO_InitStructure;   //�����ʽΪGPIO_InitTypeDef�Ľṹ�������ΪGPIO_InitStructure  
                                       	  //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;
		GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	


    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//ʧ��STM32 JTAG��д���ܣ�ֻ����SWDģʽ��д����ų�PA15��PB�в���IO��
}
void GPIO_Configuration2(void)			 //ʹ��ĳio���������ʱ������ض����ʼ������
{
    GPIO_InitTypeDef GPIO_InitStructure;   //�����ʽΪGPIO_InitTypeDef�Ľṹ�������ΪGPIO_InitStructure  
                                       	  //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 //����IO�ڵĹ���ģʽΪ�������루��io���ڲ���ӵ��赽��Դ��
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����IO����ߵ��������Ϊ50M
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  //���ñ�ѡ�еĹܽţ�|��ʾͬʱ��ѡ��
    GPIO_Init(GPIOA, &GPIO_InitStructure);			        //��ʼ��GPIOC����ӦIO��Ϊ�������ã����ڰ������

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //����IO�ڹ���ģʽΪ	����������н�ǿ�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //����IO����ߵ��������Ϊ50M
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	 //���ñ�ѡ�еĹܽţ�|��ʾͬʱ��ѡ��
    GPIO_Init(GPIOA, &GPIO_InitStructure);		  //��ʼ��GPIOC����ӦIO��Ϊ�������ã�����led���
	
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//ʧ��STM32 JTAG��д���ܣ�ֻ����SWDģʽ��д����ų�PA15��PB�в���IO��
}


void tim3()							  //����TIM3Ϊ������ʱ��ģʽ ��Լ100Hz
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  //�����ʽΪTIM_TimeBaseInitTypeDef�Ľṹ�������ΪTIM_TimeBaseStructure  

  	TIM_TimeBaseStructure. TIM_Period =9999;		  //���ü�����ֵΪ9999������ʱ���Զ����㣬�������ж�
		TIM_TimeBaseStructure.TIM_Prescaler =71;	   //	 ʱ��Ԥ��Ƶֵ����ʱ���ļ�����Ƶ�ʵ�����ʱ��Ƶ�ʳ��Ը�Ԥ��Ƶֵ
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	 // ʱ�ӷ�Ƶ����(���������˲�����ʱ����)
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	 // ������ʽΪ���ϼ���

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		 //	 ��ʼ��tim3
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //���TIM3����жϱ�־
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //  ʹ��TIM3����������ж�
    TIM_Cmd(TIM3,ENABLE);					  //		   ʹ��TIM3
}


void nvic()									//�����ж����ȼ�
{	 
     NVIC_InitTypeDef NVIC_InitStructure;  //	 //	  ����һ���ȼ�����

	 
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
	
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;   //����ADC1��ADC2�����ڶ�������ģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;				// ����ADC������ɨ��ģʽ����ͨ���������ֻ��Ҫһ��ͨ��������Ϊ��ͨ��ģʽ DISABLE
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //����ADC�����ڵ���ģʽ��ʹ��һ�Σ�ת��һ�Σ�Ҳ������������ģʽ���Զ������Բ���
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //����ת������������������ⲿ����
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;              //ADC�����Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 1;                          //���ý���ת����ͨ����ĿΪ5
  ADC_Init(ADC1, &ADC_InitStructure);                              //��ʼ��ADC1

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,ADC_SampleTime_239Cycles5);  //����ADC1��ͨ��0��һ������ת��������ʱ������239.5��

  ADC_Cmd(ADC1, ENABLE);                           //ʹ��ADC1
  
  ADC_ResetCalibration(ADC1);                        //����ADC1��У׼�Ĵ���
  while(ADC_GetResetCalibrationStatus(ADC1));        //�ȴ�ADC1У׼�������

  ADC_StartCalibration(ADC1);                        //��ʼADC1У׼
  while(ADC_GetCalibrationStatus(ADC1));             //�ȴ�ADC1У׼���
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);            //ʹ��ADC1�����ʼת��
}

