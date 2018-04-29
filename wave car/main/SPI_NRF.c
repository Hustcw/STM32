/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * �ļ���  ��SPI_NRF.c
 * ����    ��SPI_NRF24L01+����ģ��Ӧ�ú�����         
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ�-----------------------------|
 *          |                			  |
 *          |  	PA1            : NRF- CSN |A4
			|	PA5-SPI1-SCK   : NRF -SCK |
			|	PA6-SPI1-MISO  : NRF -MISO|
			|	PA7-SPI1-MOSI  : NRF -MOSI|
			|	PA3		   	   : NRF-IRQ  |B1
			|	PA2		   	   : NRF-CE   |B12
 *          |                			  |
 *           -----------------------------
 * ��汾  ��ST3.5.0
 *
 * ��汾  ��ST3.5.0
 *
 * ����    ��wildfire team 
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/
#include "SPI_NRF.h"


 u8 RX_BUF[RX_PLOAD_WIDTH];		//�������ݻ���
 u8 TX_BUF[TX_PLOAD_WIDTH];		//�������ݻ���
 u8 TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};  // ����һ����̬���͵�ַ
 u8 RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};



void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 


/*void Motor_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//ʧ��STM32 JTAG��д���ܣ�ֻ����SWDģʽ��д����ų�PA15��PB�в���IO��
	
		/ * Time base configuration * /
	TIM_TimeBaseStructure.TIM_Period = 1000-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 
	TIM_TimeBaseStructure.TIM_Prescaler =36-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	/ * Output Compare Active Mode configuration * /
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ը�

	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); 

	TIM_OC3Init(TIM2, &TIM_OCInitStructure);  
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);   

	TIM_OC4Init(TIM2, &TIM_OCInitStructure);  
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable); 
	
	TIM_ARRPreloadConfig(TIM2, ENABLE); 

	TIM_Cmd(TIM2, ENABLE);
	TIM_CtrlPWMOutputs(TIM2, ENABLE);
	
}*/


/*
 * ��������SPI_NRF_Init
 * ����  ��SPI�� I/O����
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */

void SPI_NRF_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  
  // RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM4, ENABLE); //ʹ��APB1���������ʱ�ӣ���ʱ��tim3��4��������������ֲ�
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;  //NRF2401�Ŀ�
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//nrf2401��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  //nrf2401��
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
		 GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;//nrf2401��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;  //����ָʾled��
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
		
	

//    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//ʧ��STM32 JTAG��д���ܣ�ֻ����SWDģʽ��д����ų�PA15��PB�в���IO��
	
	NRF_CSN_HIGH();
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; 
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;   
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
	SPI_InitStructure.SPI_CRCPolynomial = 7;  
	SPI_Init(SPI1, &SPI_InitStructure);   
	
	SPI_Cmd(SPI1, ENABLE);
		
		
		//Motor_Config();
}




/*
 * ��������SPI_NRF_RW
 * ����  ��������NRF��/дһ�ֽ�����
 * ����  ��д�������
 * ���  ����ȡ�õ�����
 * ����  ���ڲ�����
 */
u8 SPI_NRF_RW(u8 dat)
{  	
   /* �� SPI���ͻ������ǿ�ʱ�ȴ� */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  
   /* ͨ�� SPI2����һ�ֽ����� */
  SPI_I2S_SendData(SPI1, dat);		
 
   /* ��SPI���ջ�����Ϊ��ʱ�ȴ� */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}

/*
 * ��������SPI_NRF_WriteReg
 * ����  ��������NRF�ض��ļĴ���д������
 * ����  ��reg:NRF������+�Ĵ�����ַ��
 		   dat:��Ҫ��Ĵ���д�������
 * ���  ��NRF��status�Ĵ�����״̬
 * ����  ���ڲ�����
 */
u8 SPI_NRF_WriteReg(u8 reg,u8 dat)
{
 	u8 status;
	 NRF_CE_LOW();
	/*�õ�CSN��ʹ��SPI����*/
    NRF_CSN_LOW();
				
	/*��������Ĵ����� */
	status = SPI_NRF_RW(reg);
		 
	 /*��Ĵ���д������*/
    SPI_NRF_RW(dat); 
	          
	/*CSN���ߣ����*/	   
  	NRF_CSN_HIGH();	
		
	/*����״̬�Ĵ�����ֵ*/
   	return(status);
}


/*
 * ��������SPI_NRF_ReadReg
 * ����  �����ڴ�NRF�ض��ļĴ�����������
 * ����  ��reg:NRF������+�Ĵ�����ַ��
 * ���  ���Ĵ����е�����
 * ����  ���ڲ�����
 */
u8 SPI_NRF_ReadReg(u8 reg)
{
 	u8 reg_val;

	NRF_CE_LOW();
	/*�õ�CSN��ʹ��SPI����*/
 	NRF_CSN_LOW();
				
  	 /*���ͼĴ�����*/
	SPI_NRF_RW(reg); 

	 /*��ȡ�Ĵ�����ֵ */
	reg_val = SPI_NRF_RW(NOP);
	            
   	/*CSN���ߣ����*/
	NRF_CSN_HIGH();		
   	
	return reg_val;
}	


/*
 * ��������SPI_NRF_ReadBuf
 * ����  �����ڴ�NRF�ļĴ����ж���һ������
 * ����  ��reg:NRF������+�Ĵ�����ַ��
 		   pBuf�����ڴ洢���������ļĴ������ݵ����飬�ⲿ����
		   bytes: pBuf�����ݳ���	
 * ���  ��NRF��status�Ĵ�����״̬
 * ����  ���ⲿ����
 */
u8 SPI_NRF_ReadBuf(u8 reg,u8 *pBuf,u8 bytes)
{
 	u8 status, byte_cnt;

	  NRF_CE_LOW();
	/*�õ�CSN��ʹ��SPI����*/
	NRF_CSN_LOW();
		
	/*���ͼĴ�����*/		
	status = SPI_NRF_RW(reg); 

 	/*��ȡ����������*/
	 for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)		  
	   pBuf[byte_cnt] = SPI_NRF_RW(NOP); //��NRF24L01��ȡ����  

	 /*CSN���ߣ����*/
	NRF_CSN_HIGH();	
		
 	return status;		//���ؼĴ���״ֵ̬
}



/*
 * ��������SPI_NRF_WriteBuf
 * ����  ��������NRF�ļĴ�����д��һ������
 * ����  ��reg:NRF������+�Ĵ�����ַ��
 		   pBuf���洢�˽�Ҫд��д�Ĵ������ݵ����飬�ⲿ����
		   bytes: pBuf�����ݳ���	
 * ���  ��NRF��status�Ĵ�����״̬
 * ����  ���ⲿ����
 */
u8 SPI_NRF_WriteBuf(u8 reg ,u8 *pBuf,u8 bytes)
{
	 u8 status,byte_cnt;
	 NRF_CE_LOW();
   	 /*�õ�CSN��ʹ��SPI����*/
	 NRF_CSN_LOW();			

	 /*���ͼĴ�����*/	
  	 status = SPI_NRF_RW(reg); 
 	
  	  /*�򻺳���д������*/
	 for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
		SPI_NRF_RW(*pBuf++);	//д���ݵ������� 	 
	  	   
	/*CSN���ߣ����*/
	NRF_CSN_HIGH();			
  
  	return (status);	//����NRF24L01��״̬ 		
}

/*
 * ��������NRF_RX_Mode
 * ����  �����ò��������ģʽ
 * ����  ����	
 * ���  ����
 * ����  ���ⲿ����
 */
void NRF_RX_Mode(void)

{
	NRF_CE_LOW();	

   SPI_NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��    

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ    

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);      //����RFͨ��Ƶ��    

   SPI_NRF_WriteReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ��      

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f); //����TX�������,0db����,2Mbps,���������濪��   

   SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG, 0x0f);  //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 

/*CE���ߣ��������ģʽ*/	
  NRF_CE_HIGH();

}    

/*
 * ��������NRF_TX_Mode
 * ����  �����÷���ģʽ
 * ����  ����	
 * ���  ����
 * ����  ���ⲿ����
 */
void NRF_TX_Mode(void)
{  
	NRF_CE_LOW();		

   SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);    //дTX�ڵ��ַ 

   SPI_NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK   

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  

   SPI_NRF_WriteReg(NRF_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);       //����RFͨ��ΪCHANAL

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
	
   SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�

/*CE���ߣ����뷢��ģʽ*/	
  NRF_CE_HIGH();
    Delay(0xffff); //CEҪ����һ��ʱ��Ž��뷢��ģʽ
}



/*
 * ��������NRF_Check
 * ����  ����Ҫ����NRF��MCU�Ƿ���������
 * ����  ����	
 * ���  ��SUCCESS/ERROR ��������/����ʧ��
 * ����  ���ⲿ����
 */
u8 NRF_Check(void)
{
	u8 buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2};
	u8 buf1[5];
	u8 i; 
	 
	/*д��5���ֽڵĵ�ַ.  */  
	SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,buf,5);

	/*����д��ĵ�ַ */
	SPI_NRF_ReadBuf(TX_ADDR,buf1,5); 
	 
	/*�Ƚ�*/               
	for(i=0;i<5;i++)
	{
		if(buf1[i]!=0xC2)
		break;
	} 
	       
	if(i==5)
		return SUCCESS ;        //MCU��NRF�ɹ����� 
	else
		return ERROR ;        //MCU��NRF����������
}


 /*
 * ��������NRF_Tx_Dat
 * ����  ��������NRF�ķ��ͻ�������д������
 * ����  ��txBuf���洢�˽�Ҫ���͵����ݵ����飬�ⲿ����	
 * ���  �����ͽ�����ɹ�����TXDS,ʧ�ܷ���MAXRT��ERROR
 * ����  ���ⲿ����
 */ 
u8 NRF_Tx_Dat(u8 *txbuf)
{
	u8 state;  

	 /*ceΪ�ͣ��������ģʽ1*/
	NRF_CE_LOW();

	/*д���ݵ�TX BUF ��� 32���ֽ�*/						
   SPI_NRF_WriteBuf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);

      /*CEΪ�ߣ�txbuf�ǿգ��������ݰ� */   
 	 NRF_CE_HIGH();
	  	
	  /*�ȴ���������ж� */                            
	while(NRF_Read_IRQ()!=0); 	
	
	/*��ȡ״̬�Ĵ�����ֵ */                              
	state = SPI_NRF_ReadReg(STATUS);

	 /*���TX_DS��MAX_RT�жϱ�־*/                  
	SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,state); 	

	SPI_NRF_WriteReg(FLUSH_TX,NOP);    //���TX FIFO�Ĵ��� 

	 /*�ж��ж�����*/    
	if(state&MAX_RT)                     //�ﵽ����ط�����
			 return MAX_RT; 

	else if(state&TX_DS)                  //�������
		 	return TX_DS;
	 else						  
			return ERROR;                 //����ԭ����ʧ��
} 


 /*
 * ��������NRF_Rx_Dat
 * ����  �����ڴ�NRF�Ľ��ջ������ж�������
 * ����  ��rxBuf�����ڽ��ո����ݵ����飬�ⲿ����	
 * ���  �����ս����
 * ����  ���ⲿ����
 */ 
u8 NRF_Rx_Dat(u8 *rxbuf)
{
	u8 state; 
	NRF_CE_HIGH();	 //�������״̬
	 /*�ȴ������ж�*/
	while(NRF_Read_IRQ()!=0); 
	
	NRF_CE_LOW();  	 //�������״̬
	/*��ȡstatus�Ĵ�����ֵ  */               
	state=SPI_NRF_ReadReg(STATUS);
	 
	/* ����жϱ�־*/      
	SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,state);

	/*�ж��Ƿ���յ�����*/
	if(state&RX_DR)                                 //���յ�����
	{
	  SPI_NRF_ReadBuf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
	     SPI_NRF_WriteReg(FLUSH_RX,NOP);          //���RX FIFO�Ĵ���
	  return RX_DR; 
	}
	else    
		return ERROR;                    //û�յ��κ�����
}

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
