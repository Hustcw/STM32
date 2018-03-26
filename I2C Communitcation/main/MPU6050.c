
#include "stm32f10x.h"
#include "math.h"
#include "MPU6050.h"
#include "I2C.h"


void MPU6050_Init() //��MPU6050���г�ʼ������
{
   
	I2C();//ʹ��STM32��Ӳ��I2C���ȶ�����г�ʼ������
	I2C_WriteByte(MPU6050_Addr,PWR_MGMT_1, 0x00);//���õ�Դ����Ĵ���ֵ������ֵ��0x00(��������)
	I2C_WriteByte(MPU6050_Addr,CONFIG, 0x06);   //�����Ǻͼ��ٶȼƵ�ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
	I2C_WriteByte(MPU6050_Addr,ACCEL_CONFIG, 0x01);//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2g��5Hz)
}

ACCEL MPU6050_GetValue_ACCEL(void) //��ȡ������ٶ�
{
	ACCEL AccelValue;
	
	AccelValue.x = (u16)I2C_ReadByte(MPU6050_Addr,ACCEL_XOUT_H); //��ȡx���ϼ��ٶ��еĸ�8λ��������ǿ��ת��Ϊ16λ

	AccelValue.x <<= 8;																					//����8λ������8λת����16λ�еĸ�λ
	AccelValue.x += (u16)I2C_ReadByte(MPU6050_Addr,ACCEL_XOUT_L); //��ȡx����ٶ��еĵ�8λ�����������8λ�ϲ����õ�16λ�����X����ٶ�

	
	if(AccelValue.x > 0x8000)						//���޷��ŵ�16λ����ת��Ϊint
	{
		AccelValue.x = AccelValue.x-0xFFFF;
		
	}


	AccelValue.z = (u16)I2C_ReadByte(MPU6050_Addr,ACCEL_ZOUT_H); //��ȡx���ϼ��ٶ��еĸ�8λ��������ǿ��ת��Ϊ16λ

	AccelValue.z <<= 8;																					//����8λ������8λת����16λ�еĸ�λ
	AccelValue.z += (u16)I2C_ReadByte(MPU6050_Addr,ACCEL_ZOUT_L); //��ȡx����ٶ��еĵ�8λ�����������8λ�ϲ����õ�16λ�����X����ٶ�

	
	if(AccelValue.z > 0x8000)						//���޷��ŵ�16λ����ת��Ϊint
	{
		AccelValue.z = AccelValue.z-0xFFFF;
	}

	AccelValue.y = (u16)I2C_ReadByte(MPU6050_Addr,ACCEL_YOUT_H); //��ȡx���ϼ��ٶ��еĸ�8λ��������ǿ��ת��Ϊ16λ

	AccelValue.y <<= 8;																					//����8λ������8λת����16λ�еĸ�λ
	AccelValue.y += (u16)I2C_ReadByte(MPU6050_Addr,ACCEL_YOUT_L); //��ȡx����ٶ��еĵ�8λ�����������8λ�ϲ����õ�16λ�����X����ٶ�

	if(AccelValue.y > 0x8000)						//���޷��ŵ�16λ����ת��Ϊint
	{
		AccelValue.y = AccelValue.y-0xFFFF;
	}
	
  	return AccelValue;
}
