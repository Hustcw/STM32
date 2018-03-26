#include "stm32f10x.h"
#include	"I2C.h"


void I2C()
{
	 GPIO_InitTypeDef GPIO_InitStructure;

	 I2C_InitTypeDef I2C_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);  //¿ªÆôGPIOBºÍAFIO¸´ÓÃÊ±ÖÓ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);											//¿ªÆôI2C1µÄÊ±ÖÓ
	/* Configure IO connected to IIC*********************/
	 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;                	//¶ÔI2C1¶ÔÓ¦µÄIO¿Ú½øÐÐ³õÊ¼»¯
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;                          //×¢ÒâÉèÎª¸´ÓÃ¿ªÂ©Êä³ö
 	 GPIO_Init(GPIOB, &GPIO_InitStructure);

	 I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;																//ÉèÖÃÄ£Ê½ÎªI2CÄ£Ê½
 	 I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                       //ÉèÖÃI2CÕ¼¿Õ±È
 	 I2C_InitStructure.I2C_OwnAddress1 = I2C1_SLAVE_ADDRESS7;									//ÉèÖÃ±¾»úI2CµÄµØÖ·
 	 I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;															//¿ªÆôI2CµÄÓ¦´ð¹¦ÄÜ
 	 I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //ÉèÖÃI2CÓ¦´ðµØÖ·Îª7Î»
 	 I2C_InitStructure.I2C_ClockSpeed = 20000;                                  //ÉèÖÃI2C¹¤×÷Ê±ÖÓÆµÂÊ
	 
	 I2C_Init(I2C1, &I2C_InitStructure);																					//¶ÔI2C1½øÐÐ³õÊ¼»¯
	 I2C_Cmd(I2C1, ENABLE);   																									//Ê¹I2C1
}
/**********************************************************************/
/*IICÐ´Ò»¸ö×Ö½Ú	         				                              */
/*																	  */
/**********************************************************************/
void I2C_WriteByte(unsigned char id,unsigned char write_address,unsigned char byte)
{	
	I2C_GenerateSTART(I2C1,ENABLE);
	//²úÉúÆðÊ¼Ìõ¼þ
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	//µÈ´ýACK£¬¼ì²éÖ÷»úÊÇ·ñ»ñµÃI2C×ÜÏß
	I2C_Send7bitAddress(I2C1,id,I2C_Direction_Transmitter);
	//ÏòI2C×ÜÏßÉÏµÄ´ÓÉè±¸·¢ËÍ×¼±¸Í¨ÐÅµÄÉè±¸µØÖ·£¬²¢±íÊ¾ÒªÐ´Êý¾Ý
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	//µÈ´ýACK£¬µÈ´ýÊÇ·ñÓÐ¸ÃµØÖ·µÄI2CÉè±¸»ØÓ¦
	I2C_SendData(I2C1, write_address);
	//¼Ä´æÆ÷µØÖ·£¬Èô»ØÓ¦£¬·¢ËÍ×¼±¸Ð´ÈëÊý¾ÝµÄ¼Ä´æÆ÷µÄµØÖ·
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//µÈ´ýACK£¬µÈ´ý´ÓÉè±¸»ØÓ¦
	I2C_SendData(I2C1, byte);
	//·¢ËÍÊý¾Ý£¬·¢ËÍ×¼±¸Ð´Èë¼Ä´æÆ÷µÄÊý¾Ý
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	//·¢ËÍÍê³É£¬µÈ´ý´ÓÉè±¸»ØÓ¦£¬±íÃ÷·¢ËÍ³É¹¦
	I2C_GenerateSTOP(I2C1, ENABLE);
	//²úÉú½áÊøÐÅºÅ
}
/**********************************************************************/
/*IIC¶ÁÊý¾Ý	         				                              	  */
/*																	  */
/**********************************************************************/
unsigned char I2C_ReadByte(unsigned char  id, unsigned char read_address)
{
	unsigned char temp; 	
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
  	//µÈ´ýI2C£¬µÈ´ýI2C×ÜÏß»Øµ½¿ÕÏÐ
  	I2C_GenerateSTART(I2C1, ENABLE);
  	//²úÉúÆðÊ¼ÐÅºÅ
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    //EV5£¬µÈ´ýACK
  	I2C_Send7bitAddress(I2C1, id, I2C_Direction_Transmitter);
	//·¢ËÍ×¼±¸Í¨ÐÅµÄ´ÓÉè±¸µØÖ·£¬²¢±íÊ¾ÒªÐ´Êý¾Ý
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  	//EV6£¬µÈ´ýACK
  	I2C_Cmd(I2C1, ENABLE);
 	//ÖØÐÂÉèÖÃ¿ÉÒÔÇå³ýEV6
  	I2C_SendData(I2C1, read_address);  
	//·¢ËÍ×¼±¸¶ÁÈ¡µÄ¼Ä´æÆ÷µÄµØÖ·
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  	//EV8 £¬µÈ´ýACK
  	I2C_GenerateSTART(I2C1, ENABLE);
	//ÖØÐÂ²úÉúÆðÊ¼ÐÅºÅ£¬×¼±¸¶ÁÊý¾Ý
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  	//EV5
  	I2C_Send7bitAddress(I2C1, id, I2C_Direction_Receiver);
  	//·¢ËÍ×¼±¸¶ÁÈ¡µÄÉè±¸µÄµØÖ·
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  	//EV6  
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    
	//¹Ø±ÕÓ¦´ðºÍÍ£Ö¹Ìõ¼þ²úÉú
    while(!(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)));
		//µÈ´ý½ÓÊÕµ½Êý¾Ý
	      
    temp = I2C_ReceiveData(I2C1);
		//¶ÁÈ¡½ÓÊÜµ½µÄÊý¾Ý
		
		I2C_GenerateSTOP(I2C1, ENABLE);
		//²úÉú½áÊøÌõ¼þ
   
  	I2C_AcknowledgeConfig(I2C1, ENABLE);
		//ÖØÐÂ¿ªÆôÓ¦´ð

	return temp;
}
