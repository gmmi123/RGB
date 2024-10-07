#include "RGB.h"
#include "tim.h"
#include "stdlib.h"
#include "math.h"

#define b0 0
#define b1 1
#define b10 2
#define b100 4
#define b1000 8
#define b10000 16
#define Speed(x)  (x<<3)


/*Some Static Colors------------------------------*/
//const RGB_Color_TypeDef RED      = {255,0,0};   //��ɫ
//const RGB_Color_TypeDef GREEN    = {0,255,0};   //��ɫ
//const RGB_Color_TypeDef BLUE     = {0,0,255};   //����ɫ
//const RGB_Color_TypeDef SKY      = {0,255,255};  //����ɫ
//const RGB_Color_TypeDef MAGENTA  = {255,0,220};  //��ɫ
//const RGB_Color_TypeDef YELLOW   = {128,216,0};  //��ɫ
//const RGB_Color_TypeDef OEANGE   = {127,106,0};  //��ɫ
const RGB_Color_TypeDef BLACK    = {0,0,0};    //����ɫ
//const RGB_Color_TypeDef WHITE    = {255,255,255}; //��ɫ

typedef enum
{
	R=b1,//0b1
	G=b10,//0b10
	B=b100//0b100

}Color_TypeDef;//��ɫ����

typedef enum//�仯����
{
	Up=b1,//�����仯0b1
	Dowm=b0,//�½��仯0b0
	loop=b10,//β��ͷ�仯0b10
	Oscillating=b100,//���ر仯0b100
	Speed_Normal=b1000,//�仯����0b1000
	Speed_Max=1024
}Change_TypeDef;


RGB_Color_TypeDef RGB[10];
/*��ά����������PWM������飬ÿһ��24�����ݴ���һ��LED�����һ��24��0���ڸ�λ*/
uint32_t Pixel_Buf[Pixel_NUM+3][24];
//������ɫ����
RGB_Color_TypeDef RGB_0={255,0,0};
RGB_Color_TypeDef RGB_1={0,10,0};
/*
���ܣ����һ��װ��24��0�����24������ռ�ձ�Ϊ0��PWM������Ϊ���reset��ʱ��������ʱ��Ϊ24*1.25=37.5us > 24us(Ҫ�����24us)
//��������޷���λ�������ֻ��Ҫ����������Pixel_Buf[Pixel_NUM+1][24]������������дReset_Load���ɣ����ﲻ����ʾ�ˣ�
*/
static void Reset_Load(void)
{
	uint8_t i;
	for(i=0;i<24;i++)
	{
		Pixel_Buf[Pixel_NUM][i] = 0;

	}
}
 
/*
���ܣ���������Pixel_Buf[Pixel_NUM+1][24]�ڵ����ݣ����͵����ݱ��洢����ʱ��1ͨ��1��CCR�Ĵ��������ڿ���PWMռ�ձ�
������(&htim1)��ʱ��1��(TIM_CHANNEL_1)ͨ��1��((uint32_t *)Pixel_Buf)���������飬
			(Pixel_NUM+1)*24)���͸����������������
*/
static  void RGB_SendArray(void)
{
	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)Pixel_Buf,(Pixel_NUM+1)*24);
}

/*
���ܣ��趨����RGB LED����ɫ���ѽṹ����RGB��24BITת��Ϊ0���1��
������LedIdΪLED��ţ�Color���������ɫ�ṹ��
*/
 //ˢ��WS2812B�ư���ʾ����
static void RGB_Flush(void)
{
    Reset_Load();     //��λ
    RGB_SendArray();  //��������

}

void RGB_SetOne_Color(uint8_t LedId,RGB_Color_TypeDef Color)
{
	uint8_t i; 
	if(LedId > Pixel_NUM)return; //avoid overflow ��ֹд��ID����LED����
	//�����Ƕ� Pixel_Buf[LedId][i]д��һ�������ڸߵ�ƽ�ĳ���ʱ�䣨����˵ʱPWM��ռ�ձȼĴ���CCR1����
	for(i=0;i<8;i++) Pixel_Buf[LedId][i]   = ( ((Color.G) & (1 << (7 -i)))? (CODE_1):CODE_0 );//����ĳһ��0~7ת�����G
	for(i=8;i<16;i++) Pixel_Buf[LedId][i]  = ( ((Color.R) & (1 << (15-i)))? (CODE_1):CODE_0 );//����ĳһ��8~15ת�����R
	for(i=16;i<24;i++) Pixel_Buf[LedId][i] = ( ((Color.B) & (1 << (23-i)))? (CODE_1):CODE_0 );//����ĳһ��16~23ת�����B
}

//����RGB_SetOne_Color��������ɶԶ��LED����ɫ���á�
void RGB_SetMore_Color(uint8_t head, uint8_t heal,RGB_Color_TypeDef color)
{
    uint8_t i=0;
    for(i=head;i<=heal;i++)
    {
        RGB_SetOne_Color(i,color) ;
    }
}

int getBit(unsigned int num, int pos) {
    // ����һ�����룬����ĵ�posλΪ1������λΪ0
    unsigned int mask = 1 << pos;
    // ʹ�ð�λ�������ȡ��posλ��ֵ
    return (num & mask) != 0;
}

// �������ı�ĳһλ��ֵ
void setBit(unsigned int *num, int pos, int value) {
    // ����һ�����룬����ĵ�posλΪ1������λΪ0
    unsigned int mask = 1 << pos;

    if (value) {
        // ����posλ����Ϊ1
        *num |= mask;
    } else {
        // ����posλ����Ϊ0
        *num &= ~mask;
    }
}

void setBit_8t(unsigned char *num, int pos, int value) {
    // ����һ�����룬����ĵ�posλΪ1������λΪ0
    unsigned char mask = 1 << pos;

    if (value) {
        // ����posλ����Ϊ1
        *num |= mask;
    } else {
        // ����posλ����Ϊ0
        *num &= ~mask;
    }
}

unsigned int getBits(unsigned int num, int start, int end) {
    // ��������ĳ���
    int length = end - start + 1;
    // ����һ�����룬����ĵ�start��endλΪ1������λΪ0
    unsigned int mask = (1 << length) - 1;
    // ����������startλ
    mask <<= start;
    // ʹ�ð�λ�������ȡ��start��endλ��ֵ
    unsigned int result = (num & mask) >> start;
    return result;
}






RGB_Color_TypeDef color_Oscillating(RGB_Color_TypeDef* Var,Color_TypeDef Color,int8_t Speed)
{
	//�����ж���Ҫ���ĵ���ɫ
	//����Ǻ�ɫ
	if(getBit(Color,0)==1)
	{
		int i=(int)Var->R;//��
		int direction =0;
		if(getBit(Var->statue,0)==1)//��0
		{
			direction=1;
		}
		else
		{
			direction=-1;
		}
		i += direction * Speed;//���� //��
				// �ж��Ƿ���Ҫ�ı䷽��
		if (i > 255) {
			i = 255 - (i - 255);  // ����255ʱ����255��ʼ�Լ�
			setBit_8t(&Var->statue,0,0);  // �ı䷽��Ϊ�Լ�       //��
		} else if (i < 0) 
		{
			i = -i;  // С��0ʱ����0��ʼ����
			setBit_8t(&Var->statue,0,1);  // �ı䷽��Ϊ����			//��
		}
				
		Var->R=(uint8_t)i;//������ɫ     //��
		
	}
	if(getBit(Color,1)==1)//�������ɫ
	{
		int i=(int)Var->G;//��
		int direction =0;
		if(getBit(Var->statue,1)==1)//��0
		{
			direction=1;
		}
		else
		{
			direction=-1;
		}
		i += direction * Speed;//���� //��
				// �ж��Ƿ���Ҫ�ı䷽��
		if (i > 255) {
			i = 255 - (i - 255);  // ����255ʱ����255��ʼ�Լ�
			setBit_8t(&Var->statue,1,0);  // �ı䷽��Ϊ�Լ�       //��
		} else if (i < 0) 
		{
			i = -i;  // С��0ʱ����0��ʼ����
			setBit_8t(&Var->statue,1,1);  // �ı䷽��Ϊ����			//��
		}
				
		Var->G=(uint8_t)i;//������ɫ     //��
		
	}
	if(getBit(Color,2)==1)//�������ɫ
	{
		int i=(int)Var->B;//��
		int direction =0;
		if(getBit(Var->statue,2)==1)//��0
		{
			direction=1;
		}
		else
		{
			direction=-1;
		}
		i += direction * Speed;//���� //��
				// �ж��Ƿ���Ҫ�ı䷽��
		if (i > 255) {
			i = 255 - (i - 255);  // ����255ʱ����255��ʼ�Լ�
			setBit_8t(&Var->statue,2,0);  // �ı䷽��Ϊ�Լ�       //��
		} else if (i < 0) 
		{
			i = -i;  // С��0ʱ����0��ʼ����
			setBit_8t(&Var->statue,2,1);  // �ı䷽��Ϊ����			//��
		}
				
		Var->B=(uint8_t)i;//������ɫ     //��
		
	}
	return *Var;
}


/*
������ɫ������
ColorΪ��Ҫ�仯����ɫΪ3λbit����(R�Ǻ죬R|G�Ǻ����,BGR):R|G|B
Changing_Mod(Up|loopΪ��ɫ�ӵͱ�ߣ��ٴӵͱ��)
Up   Dowm,//�����仯0b1//�½��仯0b0
loop   Oscillating  //β��ͷ�仯   ���ر仯
Speed_Normal=b1000//�仯����0b1000
*/
RGB_Color_TypeDef Color_Gra(RGB_Color_TypeDef* Var,Color_TypeDef Color,Change_TypeDef Changing_Mod)
{
	if(getBit(Changing_Mod,0)==Up)//Changing_Mod�ĵ�0λ�ж�Ϊ�����仯�����½��仯
	{
		//�����ж���Ҫ�ı����ɫ
		if(getBit(Color,0)==1)//����Ǻ�ɫ
		{
			//�жϱ仯����
			if(getBit(Changing_Mod,1)==1)//�ж�loop����
			{
				//�жϱ仯����
				Var->R+=getBits(Changing_Mod,3,3+7);
			}
			//�ж�Oscillating
			if(getBit(Changing_Mod,2)==1)
			{
				color_Oscillating(Var,R,(int)getBits(Changing_Mod,3,3+7));
			}
		}
		if(getBit(Color,1)==1)//�������ɫ
		{
			if(getBit(Changing_Mod,1)==1)//�ж�loop����
			{
				//�жϱ仯����
				Var->G+=getBits(Changing_Mod,3,3+7);
			}
			//�ж�Oscillating
			if(getBit(Changing_Mod,2)==1)
			{
				color_Oscillating(Var,G,(int)getBits(Changing_Mod,3,3+7));
			}
			
		}
		if(getBit(Color,2)==1)//�������ɫ
		{
			if(getBit(Changing_Mod,1)==1)//�ж�loop����
			{
				//�жϱ仯����
				Var->B+=getBits(Changing_Mod,3,3+7);
			}
			//�ж�Oscillating
			if(getBit(Changing_Mod,2)==1)
			{
				color_Oscillating(Var,B,(int)getBits(Changing_Mod,3,3+7));
			}
		}

	}
	else
	{
		if(getBit(Color,0)==1)//����Ǻ�ɫ
		{
			//�жϱ仯����
			if(getBit(Changing_Mod,1)==1)//�ж�loop����
			{
				//�жϱ仯����
				Var->R-=getBits(Changing_Mod,3,3+7);
			}
						//�ж�Oscillating
			if(getBit(Changing_Mod,2)==1)
			{
				color_Oscillating(Var,R,(int)getBits(Changing_Mod,3,3+7));
			}
		}
		if(getBit(Color,1)==1)//�������ɫ
		{
			if(getBit(Changing_Mod,1)==1)//�ж�loop����
			{
				//�жϱ仯����
				Var->G-=getBits(Changing_Mod,3,3+7);
			}
				//�ж�Oscillating
			if(getBit(Changing_Mod,2)==1)
			{
				color_Oscillating(Var,G,(int)getBits(Changing_Mod,3,3+7));
			}
			
		}
		if(getBit(Color,2)==1)//�������ɫ
		{
			if(getBit(Changing_Mod,1)==1)//�ж�loop����
			{
				//�жϱ仯����
				Var->B-=getBits(Changing_Mod,3,3+7);
			}
			//�ж�Oscillating
			if(getBit(Changing_Mod,2)==1)
			{
				color_Oscillating(Var,B,(int)getBits(Changing_Mod,3,3+7));
			}
		}
	
	}

}



       




RGB_Color_TypeDef GerColorful(int Colorfulx)//�ٲʺ��
{
	RGB_Color_TypeDef temp;
	temp.B=0;
	temp.G=0;
	temp.R=0;
	if(!(Colorfulx>0&&Colorfulx<767))//��Խ��
	{
		return	temp;
	}
	if(Colorfulx>=0&&Colorfulx<=255)
	{
		temp.R=255-Colorfulx;
		temp.G=Colorfulx;
	}
	else if(Colorfulx>=256&&Colorfulx<=511)
	{
		temp.G=511-Colorfulx;
		temp.B=Colorfulx-256;
	}
	else if(Colorfulx>=512&&Colorfulx<=767)
	{
		temp.B=767-Colorfulx;
		temp.R=Colorfulx-512;
	}
	return	temp;
}


RGB_Color_TypeDef hsl_to_rgb(float i ) //�ʺ��
{
	RGB_Color_TypeDef temp;
//	float h = (float)(i % 360);
	
//	int hue = (int)i % 360;
//	float h = (float)hue;
	float h = (int)fmod(i, 360.0);
	float s=1.0f;//���Ͷ�
	float l=0.5f;//����
    float c = (1 - fabs(2 * l - 1)) * s;
    float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
    float m = l - c / 2;

    float r_prime, g_prime, b_prime;

    if (h >= 0 && h < 60) {
        r_prime = c;
        g_prime = x;
        b_prime = 0;
    } else if (h >= 60 && h < 120) {
        r_prime = x;
        g_prime = c;
        b_prime = 0;
    } else if (h >= 120 && h < 180) {
        r_prime = 0;
        g_prime = c;
        b_prime = x;
    } else if (h >= 180 && h < 240) {
        r_prime = 0;
        g_prime = x;
        b_prime = c;
    } else if (h >= 240 && h < 300) {
        r_prime = x;
        g_prime = 0;
        b_prime = c;
    } else { // h >= 300 && h < 360
        r_prime = c;
        g_prime = 0;
        b_prime = x;
    }

    temp.R = (int)((r_prime + m) * 255);
    temp.G = (int)((g_prime + m) * 255);
    temp.B = (int)((b_prime + m) * 255);
	return temp;
}


//����ƺ���

void RGB_Chasing(RGB_Color_TypeDef* RGB,uint16_t count,uint8_t Start,uint8_t Stop,RGB_Color_TypeDef (*RGB_Method)(int ,int))
{

	char j;

	RGB_Color_TypeDef tempRGB=*RGB;
	if(((RGB->statue>>3)&1)==0)//���������ʹ��
	{
		RGB->statue=RGB->statue | (1<<3);
		(RGB->line)=255;
	};
	
	if(Stop>Start)//�����ƶ�
	{
		if((RGB->line)==255)
		{
			(RGB->line)=Start+count-2;//������:-2�е�һ��1������ģ�һ���ǲ����ĵƵ�
		}
		if((RGB->line)<Stop+count&&((RGB->statue>>4)&1)==0)//ȫ����������Ϊ��ʼ��
		{
			(RGB->line)++;//��ʼ��
		}
		else//�ƶ�
		{
			(RGB->line)=Start+count;
		}
		for(j=0;j<count;j++)//��βЧ��
		{
			
			
			if((RGB->line)-j>=0&&(RGB->line)-j<=Stop)//i-j��ʾ���鲻Խ�磬Խ��Ĳ�ִ�У�i-j<=Stop��ʾ������ʾ����Ĳ�ִ��
			{
				tempRGB=RGB_Method(count,j);//�ı���ɫ
				RGB_SetOne_Color(((RGB->line)-j),tempRGB);
				//////////
				
			}
			else if((RGB->line)-Stop-j+Start-1>= 0&&(RGB->line)>Stop)//i-Stop-j��ʾ���鲻Խ�磬Խ��Ĳ�ִ��,i-Stop-j+Start-1:i-Stop��ʾ�����i�Ĳ��֣�+start��ʾ���ò����Ƶ���㣬-j��ʾ��β��-1��ʾȥ������ʾ�Ĳ���
			{
				tempRGB=RGB_Method(count,j);//�ı���ɫ
				/////////
				RGB_SetOne_Color((RGB->line)-Stop-j+Start-1,tempRGB);
			}
			
		}

	}
	else//�����ƶ�
	{
		if((RGB->line)==255)
		{
			(RGB->line)=Start-count+1;//��ʼ������:+2�е�һ��1������ģ�һ���ǲ����ĵƵ�
		}
		if((RGB->line)>Stop-count)//Stop-count��ʾi��û��ֹͣ��
		{
			(RGB->line)--;
		}
		else
		{
			(RGB->line)=Start-count;//���õڶ������ϵĵ�
		}
		for(j=0;j<count;j++)//������ʾ
		{
			
			
			if((RGB->line)+j>=0&&(RGB->line)+j>=Stop&&(RGB->line)+j<Start)//i+j>=0��ʾ���鲻Խ�磬Խ��Ĳ�ִ�У�i+j<=Stop��ʾ������ʾ����Ĳ�ִ��
			{
				////////
				tempRGB=RGB_Method(count,j);//�ı���ɫ
				
				RGB_SetOne_Color(((RGB->line)+j),tempRGB);
			}
			else if(Start-(Stop-(RGB->line)-j)+1>=0&&(RGB->line)<Stop&&Start-(Stop-(RGB->line)-j)+1<=Start)//Start-(Stop-i-j)+1��ʾ���鲻Խ�磬Խ��Ĳ�ִ��
			{
				//////
				tempRGB=RGB_Method(count,j);//�ı���ɫ
				
				RGB_SetOne_Color(Start-(Stop-(RGB->line)-j)+1,tempRGB);//��ʾ����������ĵ�һ���жϵģ���ʼ��Start+j�ĵط�����������
			}
			
		}
	}

}




RGB_Color_TypeDef Method_00(int count,int j)//������ˮ��
{
	RGB_Color_TypeDef temp;
	static uint8_t i=0;
	static uint8_t k=0;
	static uint8_t l=0;
	if(i%10==0)
	{
		if(l==0)
		{
			Color_Gra(&RGB_0,R,Dowm|Oscillating|Speed(1));
			Color_Gra(&RGB_0,G,Up|Oscillating|Speed(1));
		}
		if(l==1)
		{	
			Color_Gra(&RGB_0,G,Up|Oscillating|Speed(1));
			Color_Gra(&RGB_0,B,Up|Oscillating|Speed(1));
		}
		if(l==2)
		{
			Color_Gra(&RGB_0,B,Up|Oscillating|Speed(1));
			Color_Gra(&RGB_0,R,Dowm|Oscillating|Speed(1));
		}

		
		k++;
		if(k==255)
		{
			l++;
		}
		if(l==3)
		{
			l=0;
		}
	}
	i++;

	temp.R=(count-j)*RGB_0.R/count;
	temp.G=(count-j)*RGB_0.G/count;
	temp.B=(count-j)*RGB_0.B/count;
	return temp;
}


RGB_Color_TypeDef Method_01(int count,int j)//���䲻��ˮ��
{
	static char i=0;
	uint8_t speed=0;//��仯�ٶ��йص���
	if(i==0)
	{
		RGB_1.B=0;
		RGB_1.R=0;
		RGB_1.G=255;
		i=1;
	}
	
	if(count-j-2<speed)
	{
		Color_Gra(&RGB_1,R,Up|Oscillating|Speed_Normal);
		Color_Gra(&RGB_1,G,Up|Oscillating|Speed_Normal);
	}

	return RGB_1;
}
RGB_Color_TypeDef Method_02(int count,int j)//�ʺ������ˮ
{
	static int k=0;
	static float i=0;//������ɫ��x������
	RGB_Color_TypeDef temp;

	

	i+=(360/(float)count);
	if(k%count==0)
	{
		i+=1;
	}
		
		temp=hsl_to_rgb(i);
	k++;

	
	
//	temp.R=(count-j)*temp.R/count;
//	temp.G=(count-j)*temp.G/count;
//	temp.B=(count-j)*temp.B/count;
	return temp;
}
RGB_Color_TypeDef Method_03(int count,int j)//�ʺ������ˮ
{
	static uint8_t k=0;
	static int i=0;//������ɫ��x������
	RGB_Color_TypeDef temp;

	
		if(i>16777216){i=0;}
		else
		{
			i+=(360/count);
			if(k%count==0)
			{
				i+=1;
			}
		}
		temp=hsl_to_rgb(i);
	k++;

	
	
	temp.R=(count-j)*temp.R/count;
	temp.G=(count-j)*temp.G/count;
	temp.B=(count-j)*temp.B/count;
	return temp;
}
//��Colorfulxӳ�䵽��ɫ���򣬷��ض�ӦColorfulxλ�õ���ɫ
//Colorfulx��0~767�е�һ��ֵ
void RGB_Show_64(void)   
{
	//����
	static int a=0;

	RGB_SetMore_Color(0,Pixel_NUM,BLACK);
	
//	if(i>=16777216){i=0;}
//	RGB_0=hsl_to_rgb(i);
//	
//       i++; // ����ѭ������
	//

//	RGB_0.G=100;

//	RGB_Chasing(&RGB_1,5,144,0,Method_01);
//	RGB_Chasing(&RGB_0,10,0,120,Method_00);
	setBit_8t(&RGB[2].statue,4,1);
	RGB_Chasing(&RGB[2],144,0,144,Method_02);
	

//	a++;
//	if(a>25)
//	{
//		RGB_Chasing(&RGB[3],20,0,144,Method_03);
//	}
//	RGB_SetMore_Color(0,8,RGB_0);
  //ˢ��WS2812B����ʾ
	RGB_Flush();
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
	
}


