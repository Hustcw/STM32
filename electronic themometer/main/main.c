
#include "stm32f10x.h"
#include "stdio.h"
#include "delay.h"
#include "NixieTube.h"


#define ADC1_DR_Address    ((uint32_t)0x4001244C)

u16 AD1_Value;
float resistence=0;


void RCC_Configuration(void);	 //ʱ�ӳ�ʼ������������ʱ��
void GPIO_Configuration(void);	 //IO�ڳ�ʼ���������书��
void tim3(void);				 //��ʱ��tim4��ʼ������
void nvic(void);				 //�ж����ȼ�������
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
  5.5405,5.3534,5.1725,4.9976,4.8286}; //-10��45�����������ֵ


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



void RCC_Configuration(void)				 //ʹ���κ�һ������ʱ����ؿ�������Ӧ��ʱ��
{
  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO, ENABLE);	  //ʹ��APB2���������ʱ�ӣ�����GPIOA,ADC1, ���ܸ���ʱ��AFIO�ȣ�
                                                                              //�������������裬����̼����ֲ������
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʹ��APB1���������ʱ�ӣ���ʱ��tim3��������������ֲ�

                
}


void GPIO_Configuration(void)			 //ʹ��ĳio���������ʱ������ض����ʼ������
{
    GPIO_InitTypeDef GPIO_InitStructure;   //�����ʽΪGPIO_InitTypeDef�Ľṹ�������ΪGPIO_InitStructure  
                                       	  //typedef struct { u16 GPIO_Pin; GPIOSpeed_TypeDef GPIO_Speed; GPIOMode_TypeDef GPIO_Mode; } GPIO_InitTypeDef;
		 GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


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


void TIM3_IRQHandler(void)		//TIM3����������ж���Ӧ����
{     
			float Vx;
	    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	 //���TIM3����ж���Ӧ������־λ
			ADC_SoftwareStartConvCmd(ADC1, ENABLE); //��ʼ����ģʽ����ת��
	    AD1_Value=ADC_GetConversionValue(ADC1);
      Vx = (AD1_Value * 3.3) / 4095;
      resistence =(10*Vx) / (3.3-Vx);	//�����ʱ����
}			


void nvic()									//�����ж����ȼ�
{	 
     NVIC_InitTypeDef NVIC_InitStructure;  //	 //	  ����һ���ȼ�����

	 
		 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); //Ҫ��ͬһ��Group
     NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //TIM3	��������ж�
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//	������ȼ�Ϊ1������һ����ͬ����ϣ���ж��໥��϶Է�
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 	//	��Ӧ���ȼ�1��������һ�����������ж�ͬʱ��ʱ����һ����ִ��
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

