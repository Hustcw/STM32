#include "stm32f10x.h"
#include "math.h"
#include "delay.h"

void RCC_Configuration(void);	 
void GPIO_Configuration(void);	

void num(int n)    
 {
   switch(n)
   {
    case 0: GPIOC->ODR&=0xffffff00; GPIOC->ODR|=0x000000c0;break; 
	  case 1: GPIOC->ODR&=0xffffff00; GPIOC->ODR|=0x000000f9;break;
	  case 2: GPIOC->ODR&=0xffffff00; GPIOC->ODR|=0x000000a4;break;
	  case 3: GPIOC->ODR&=0xffffff00; GPIOC->ODR|=0x000000b0;break;
	  case 4: GPIOC->ODR&=0xffffff00; GPIOC->ODR|=0x00000099;break;
	  case 5: GPIOC->ODR&=0xffffff00; GPIOC->ODR|=0x00000092;break;
	  case 6: GPIOC->ODR&=0xffffff00; GPIOC->ODR|=0x00000082;break;
	  case 7: GPIOC->ODR&=0xffffff00; GPIOC->ODR|=0x000000f8;break;
	  case 8: GPIOC->ODR&=0xffffff00; GPIOC->ODR|=0x00000080;break;
	  case 9: GPIOC->ODR&=0xffffff00; GPIOC->ODR|=0x00000090;break;
   }

 }

void NumDisplay(int i) 
{
	int s[4];
  s[0]=i/1000;
  s[1]=(i-s[0]*1000)/100;
  s[2]=(i-s[0]*1000-s[1]*100)/10;
	s[3]=(i-s[0]*1000-s[1]*100-s[2]*10);
  if(i<0){
	GPIO_SetBits(GPIOC,GPIO_Pin_8);
	GPIO_ResetBits(GPIOC,GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11);
  num(-s[3]);
  delay_nms(1);
	
	GPIO_SetBits(GPIOC,GPIO_Pin_9);
	GPIO_ResetBits(GPIOC,GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_11);
  num(-s[2]);
	GPIO_ResetBits(GPIOC,GPIO_Pin_7);
  delay_nms(1);
	
	GPIO_SetBits(GPIOC,GPIO_Pin_10);
	GPIO_ResetBits(GPIOC,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11);
  num(-s[1]);
  delay_nms(1);
	
	GPIO_SetBits(GPIOC,GPIO_Pin_11);
	GPIO_ResetBits(GPIOC,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10);
	GPIO_SetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
  GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	delay_nms(1);
}
	else{
	GPIO_SetBits(GPIOC,GPIO_Pin_8);
	GPIO_ResetBits(GPIOC,GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11);
  num(s[3]);
  delay_nms(1);
	
	GPIO_SetBits(GPIOC,GPIO_Pin_9);
	GPIO_ResetBits(GPIOC,GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_11);
  num(s[2]);
	GPIO_ResetBits(GPIOC,GPIO_Pin_7);
  delay_nms(1);
	
	GPIO_SetBits(GPIOC,GPIO_Pin_10);
	GPIO_ResetBits(GPIOC,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11);
  num(s[1]);
  delay_nms(1);
	
	GPIO_SetBits(GPIOC,GPIO_Pin_11);
	GPIO_ResetBits(GPIOC,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10);
	GPIO_SetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
	delay_nms(1);
	}
}


void Nixie_Configuration(void)				 
{
    GPIO_InitTypeDef GPIO_InitStructure;     
                                       	  
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	  
                                                                            

	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;	 //���ñ�ѡ�еĹܽţ�|��ʾͬʱ��ѡ��
    GPIO_Init(GPIOC, &GPIO_InitStructure);		  

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	
}
