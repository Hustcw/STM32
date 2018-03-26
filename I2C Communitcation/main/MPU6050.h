#include "stm32f10x.h"

#define	MPU6050_Addr    0xD0 //MPU6050������������ַ
#define	SMPLRT_DIV		0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIG			0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define ACCEL_YOUT_H    0x3D
#define ACCEL_YOUT_L    0x3E
#define ACCEL_ZOUT_H    0x3F
#define ACCEL_ZOUT_L    0x40
#define	PWR_MGMT_1		0x6B	//��Դ��������ֵ��0x00(��������)


typedef struct
{
	int x;
	int y;
	int z;
}ACCEL; //�����¼������ٶȵĽṹ��

ACCEL MPU6050_GetValue_ACCEL(void);   //��ȡ������ٶ�����
void MPU6050_Init(void); //��MPU6050���г�ʼ��

