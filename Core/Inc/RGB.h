#ifndef __RGB_H__
#define __RGB_H__
 
#include "main.h"
 //1.1us 139 39
//0���1��Ķ��壬���õ�ʱCCR�Ĵ�����ֵ
//����ʹ�õ�˼PWM���ģʽ1������ֵ<CCRʱ�������Ч��ƽ-�ߵ�ƽ��CubeMX����Ĭ����Ч��ƽΪ�ߵ�ƽ��
#define CODE_1       (143)       //1�붨ʱ����������������ռ�ձ�Ϊ84/125 = 66%
#define CODE_0       (67)       //0�붨ʱ����������������ռ�ձ�Ϊ42/125 = 33%
 
//����LED����ɫ���ƽṹ��
//statue˵����statue��ÿһλ����һ�������ı�־λ��0λΪ����ʱ��״̬��0Ϊ������1λ�½���
typedef struct
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint8_t statue;//012����Ʊ�־λ��3Ϊ����Ƴ�ʼ��,4Ϊ������Ƿ�����
	int line;
}RGB_Color_TypeDef;


 
#define Pixel_NUM 144  //LED�����궨�壬���ǵư�����64����

static void Reset_Load(void); //�ú������ڽ��������24�����ݱ�Ϊ0������RESET_code

    //������������
static void RGB_SendArray(void); 

static void RGB_Flush(void);  //ˢ��RGB��ʾ

void RGB_SetOne_Color(uint8_t LedId,RGB_Color_TypeDef Color);//��һ��LEDװ��24����ɫ�����루0���1�룩
     

//void RGB_RED(uint16_t Pixel_Len);  //��ʾ���

//���ƶ��LED��ʾ��ͬ����ɫ
void RGB_SetMore_Color(uint8_t head, uint8_t heal,RGB_Color_TypeDef color);


void RGB_Show_64(void); //RGBд�뺯��
 
#endif


