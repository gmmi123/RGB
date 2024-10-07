#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "main.h"

//����ԭ��MPU9250ͨѶ������
//������ԭ��MPU6050�����޸�



//IO��������
#define MPU_SDA_IN()  {GPIOC->MODER = (GPIOC->MODER & ~(3 << (2*5))) | (0 << (2*5));}
#define MPU_SDA_OUT() {GPIOC->MODER = (GPIOC->MODER & ~(3 << (2*5))) | (1 << (2*5));}

////IO��������	 
#define MPU_IIC_SCL_SET    HAL_GPIO_WritePin(MPU9250_SCL_GPIO_Port,MPU9250_SCL_Pin,GPIO_PIN_SET) 		//SCL
#define MPU_IIC_SCL_RESET    HAL_GPIO_WritePin(MPU9250_SCL_GPIO_Port,MPU9250_SCL_Pin,GPIO_PIN_RESET)
#define MPU_IIC_SDA_SET   HAL_GPIO_WritePin(MPU9250_SDA_GPIO_Port,MPU9250_SDA_Pin,GPIO_PIN_SET) 		//SDA	 
#define MPU_IIC_SDA_RESET    HAL_GPIO_WritePin(MPU9250_SDA_GPIO_Port,MPU9250_SDA_Pin,GPIO_PIN_RESET) 		//SDA
#define MPU_READ_SDA   HAL_GPIO_ReadPin(MPU9250_SDA_GPIO_Port,MPU9250_SDA_Pin) 		//����SDA

//IIC���в�������
void MPU_IIC_Delay(void);				//MPU IIC��ʱ����	
void MPU_IIC_Init(void);                //��ʼ��IIC��IO��				 
void MPU_IIC_Start(void);				//����IIC��ʼ�ź�
void MPU_IIC_Stop(void);	  			//����IICֹͣ�ź�
void MPU_IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t MPU_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t MPU_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void MPU_IIC_Ack(void);					//IIC����ACK�ź�
void MPU_IIC_NAck(void);				//IIC������ACK�ź�

void IMPU_IC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t MPU_IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	  
#endif
















