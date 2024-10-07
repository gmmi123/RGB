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
//const RGB_Color_TypeDef RED      = {255,0,0};   //红色
//const RGB_Color_TypeDef GREEN    = {0,255,0};   //绿色
//const RGB_Color_TypeDef BLUE     = {0,0,255};   //深蓝色
//const RGB_Color_TypeDef SKY      = {0,255,255};  //天蓝色
//const RGB_Color_TypeDef MAGENTA  = {255,0,220};  //粉色
//const RGB_Color_TypeDef YELLOW   = {128,216,0};  //黄色
//const RGB_Color_TypeDef OEANGE   = {127,106,0};  //橘色
const RGB_Color_TypeDef BLACK    = {0,0,0};    //无颜色
//const RGB_Color_TypeDef WHITE    = {255,255,255}; //白色

typedef enum
{
	R=b1,//0b1
	G=b10,//0b10
	B=b100//0b100

}Color_TypeDef;//颜色定义

typedef enum//变化定义
{
	Up=b1,//上增变化0b1
	Dowm=b0,//下降变化0b0
	loop=b10,//尾接头变化0b10
	Oscillating=b100,//来回变化0b100
	Speed_Normal=b1000,//变化速率0b1000
	Speed_Max=1024
}Change_TypeDef;


RGB_Color_TypeDef RGB[10];
/*二维数组存放最终PWM输出数组，每一行24个数据代表一个LED，最后一行24个0用于复位*/
uint32_t Pixel_Buf[Pixel_NUM+3][24];
//渐变颜色变量
RGB_Color_TypeDef RGB_0={255,0,0};
RGB_Color_TypeDef RGB_1={0,10,0};
/*
功能：最后一行装在24个0，输出24个周期占空比为0的PWM波，作为最后reset延时，这里总时长为24*1.25=37.5us > 24us(要求大于24us)
//如果出现无法复位的情况，只需要在增加数组Pixel_Buf[Pixel_NUM+1][24]的行数，并改写Reset_Load即可，这里不做演示了，
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
功能：发送数组Pixel_Buf[Pixel_NUM+1][24]内的数据，发送的数据被存储到定时器1通道1的CCR寄存器，用于控制PWM占空比
参数：(&htim1)定时器1，(TIM_CHANNEL_1)通道1，((uint32_t *)Pixel_Buf)待发送数组，
			(Pixel_NUM+1)*24)发送个数，数组行列相乘
*/
static  void RGB_SendArray(void)
{
	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)Pixel_Buf,(Pixel_NUM+1)*24);
}

/*
功能：设定单个RGB LED的颜色，把结构体中RGB的24BIT转换为0码和1码
参数：LedId为LED序号，Color：定义的颜色结构体
*/
 //刷新WS2812B灯板显示函数
static void RGB_Flush(void)
{
    Reset_Load();     //复位
    RGB_SendArray();  //发送数据

}

void RGB_SetOne_Color(uint8_t LedId,RGB_Color_TypeDef Color)
{
	uint8_t i; 
	if(LedId > Pixel_NUM)return; //avoid overflow 防止写入ID大于LED总数
	//这里是对 Pixel_Buf[LedId][i]写入一个周期内高电平的持续时间（或者说时PWM的占空比寄存器CCR1），
	for(i=0;i<8;i++) Pixel_Buf[LedId][i]   = ( ((Color.G) & (1 << (7 -i)))? (CODE_1):CODE_0 );//数组某一行0~7转化存放G
	for(i=8;i<16;i++) Pixel_Buf[LedId][i]  = ( ((Color.R) & (1 << (15-i)))? (CODE_1):CODE_0 );//数组某一行8~15转化存放R
	for(i=16;i<24;i++) Pixel_Buf[LedId][i] = ( ((Color.B) & (1 << (23-i)))? (CODE_1):CODE_0 );//数组某一行16~23转化存放B
}

//调用RGB_SetOne_Color函数，完成对多个LED的颜色设置。
void RGB_SetMore_Color(uint8_t head, uint8_t heal,RGB_Color_TypeDef color)
{
    uint8_t i=0;
    for(i=head;i<=heal;i++)
    {
        RGB_SetOne_Color(i,color) ;
    }
}

int getBit(unsigned int num, int pos) {
    // 创建一个掩码，掩码的第pos位为1，其余位为0
    unsigned int mask = 1 << pos;
    // 使用按位与操作提取第pos位的值
    return (num & mask) != 0;
}

// 函数：改变某一位的值
void setBit(unsigned int *num, int pos, int value) {
    // 创建一个掩码，掩码的第pos位为1，其余位为0
    unsigned int mask = 1 << pos;

    if (value) {
        // 将第pos位设置为1
        *num |= mask;
    } else {
        // 将第pos位设置为0
        *num &= ~mask;
    }
}

void setBit_8t(unsigned char *num, int pos, int value) {
    // 创建一个掩码，掩码的第pos位为1，其余位为0
    unsigned char mask = 1 << pos;

    if (value) {
        // 将第pos位设置为1
        *num |= mask;
    } else {
        // 将第pos位设置为0
        *num &= ~mask;
    }
}

unsigned int getBits(unsigned int num, int start, int end) {
    // 计算掩码的长度
    int length = end - start + 1;
    // 创建一个掩码，掩码的第start到end位为1，其余位为0
    unsigned int mask = (1 << length) - 1;
    // 将掩码左移start位
    mask <<= start;
    // 使用按位与操作提取第start到end位的值
    unsigned int result = (num & mask) >> start;
    return result;
}






RGB_Color_TypeDef color_Oscillating(RGB_Color_TypeDef* Var,Color_TypeDef Color,int8_t Speed)
{
	//依次判断需要更改的颜色
	//如果是红色
	if(getBit(Color,0)==1)
	{
		int i=(int)Var->R;//改
		int direction =0;
		if(getBit(Var->statue,0)==1)//改0
		{
			direction=1;
		}
		else
		{
			direction=-1;
		}
		i += direction * Speed;//速率 //改
				// 判断是否需要改变方向
		if (i > 255) {
			i = 255 - (i - 255);  // 超过255时，从255开始自减
			setBit_8t(&Var->statue,0,0);  // 改变方向为自减       //改
		} else if (i < 0) 
		{
			i = -i;  // 小于0时，从0开始自增
			setBit_8t(&Var->statue,0,1);  // 改变方向为自增			//改
		}
				
		Var->R=(uint8_t)i;//设置颜色     //改
		
	}
	if(getBit(Color,1)==1)//如果是绿色
	{
		int i=(int)Var->G;//改
		int direction =0;
		if(getBit(Var->statue,1)==1)//改0
		{
			direction=1;
		}
		else
		{
			direction=-1;
		}
		i += direction * Speed;//速率 //改
				// 判断是否需要改变方向
		if (i > 255) {
			i = 255 - (i - 255);  // 超过255时，从255开始自减
			setBit_8t(&Var->statue,1,0);  // 改变方向为自减       //改
		} else if (i < 0) 
		{
			i = -i;  // 小于0时，从0开始自增
			setBit_8t(&Var->statue,1,1);  // 改变方向为自增			//改
		}
				
		Var->G=(uint8_t)i;//设置颜色     //改
		
	}
	if(getBit(Color,2)==1)//如果是蓝色
	{
		int i=(int)Var->B;//改
		int direction =0;
		if(getBit(Var->statue,2)==1)//改0
		{
			direction=1;
		}
		else
		{
			direction=-1;
		}
		i += direction * Speed;//速率 //改
				// 判断是否需要改变方向
		if (i > 255) {
			i = 255 - (i - 255);  // 超过255时，从255开始自减
			setBit_8t(&Var->statue,2,0);  // 改变方向为自减       //改
		} else if (i < 0) 
		{
			i = -i;  // 小于0时，从0开始自增
			setBit_8t(&Var->statue,2,1);  // 改变方向为自增			//改
		}
				
		Var->B=(uint8_t)i;//设置颜色     //改
		
	}
	return *Var;
}


/*
传入颜色变量，
Color为需要变化的颜色为3位bit数据(R是红，R|G是红和绿,BGR):R|G|B
Changing_Mod(Up|loop为颜色从低变高，再从低变高)
Up   Dowm,//上增变化0b1//下降变化0b0
loop   Oscillating  //尾接头变化   来回变化
Speed_Normal=b1000//变化速率0b1000
*/
RGB_Color_TypeDef Color_Gra(RGB_Color_TypeDef* Var,Color_TypeDef Color,Change_TypeDef Changing_Mod)
{
	if(getBit(Changing_Mod,0)==Up)//Changing_Mod的第0位判断为上升变化还是下降变化
	{
		//依次判断需要改变的颜色
		if(getBit(Color,0)==1)//如果是红色
		{
			//判断变化类型
			if(getBit(Changing_Mod,1)==1)//判断loop类型
			{
				//判断变化速率
				Var->R+=getBits(Changing_Mod,3,3+7);
			}
			//判断Oscillating
			if(getBit(Changing_Mod,2)==1)
			{
				color_Oscillating(Var,R,(int)getBits(Changing_Mod,3,3+7));
			}
		}
		if(getBit(Color,1)==1)//如果是绿色
		{
			if(getBit(Changing_Mod,1)==1)//判断loop类型
			{
				//判断变化速率
				Var->G+=getBits(Changing_Mod,3,3+7);
			}
			//判断Oscillating
			if(getBit(Changing_Mod,2)==1)
			{
				color_Oscillating(Var,G,(int)getBits(Changing_Mod,3,3+7));
			}
			
		}
		if(getBit(Color,2)==1)//如果是蓝色
		{
			if(getBit(Changing_Mod,1)==1)//判断loop类型
			{
				//判断变化速率
				Var->B+=getBits(Changing_Mod,3,3+7);
			}
			//判断Oscillating
			if(getBit(Changing_Mod,2)==1)
			{
				color_Oscillating(Var,B,(int)getBits(Changing_Mod,3,3+7));
			}
		}

	}
	else
	{
		if(getBit(Color,0)==1)//如果是红色
		{
			//判断变化类型
			if(getBit(Changing_Mod,1)==1)//判断loop类型
			{
				//判断变化速率
				Var->R-=getBits(Changing_Mod,3,3+7);
			}
						//判断Oscillating
			if(getBit(Changing_Mod,2)==1)
			{
				color_Oscillating(Var,R,(int)getBits(Changing_Mod,3,3+7));
			}
		}
		if(getBit(Color,1)==1)//如果是绿色
		{
			if(getBit(Changing_Mod,1)==1)//判断loop类型
			{
				//判断变化速率
				Var->G-=getBits(Changing_Mod,3,3+7);
			}
				//判断Oscillating
			if(getBit(Changing_Mod,2)==1)
			{
				color_Oscillating(Var,G,(int)getBits(Changing_Mod,3,3+7));
			}
			
		}
		if(getBit(Color,2)==1)//如果是蓝色
		{
			if(getBit(Changing_Mod,1)==1)//判断loop类型
			{
				//判断变化速率
				Var->B-=getBits(Changing_Mod,3,3+7);
			}
			//判断Oscillating
			if(getBit(Changing_Mod,2)==1)
			{
				color_Oscillating(Var,B,(int)getBits(Changing_Mod,3,3+7));
			}
		}
	
	}

}



       




RGB_Color_TypeDef GerColorful(int Colorfulx)//假彩虹灯
{
	RGB_Color_TypeDef temp;
	temp.B=0;
	temp.G=0;
	temp.R=0;
	if(!(Colorfulx>0&&Colorfulx<767))//防越界
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


RGB_Color_TypeDef hsl_to_rgb(float i ) //彩虹灯
{
	RGB_Color_TypeDef temp;
//	float h = (float)(i % 360);
	
//	int hue = (int)i % 360;
//	float h = (float)hue;
	float h = (int)fmod(i, 360.0);
	float s=1.0f;//饱和度
	float l=0.5f;//亮度
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


//跑马灯函数

void RGB_Chasing(RGB_Color_TypeDef* RGB,uint16_t count,uint8_t Start,uint8_t Stop,RGB_Color_TypeDef (*RGB_Method)(int ,int))
{

	char j;

	RGB_Color_TypeDef tempRGB=*RGB;
	if(((RGB->statue>>3)&1)==0)//设置跑马灯使能
	{
		RGB->statue=RGB->statue | (1<<3);
		(RGB->line)=255;
	};
	
	if(Stop>Start)//正向移动
	{
		if((RGB->line)==255)
		{
			(RGB->line)=Start+count-2;//出发点:-2中的一个1是数组的，一个是不亮的灯的
		}
		if((RGB->line)<Stop+count&&((RGB->statue>>4)&1)==0)//全部超过设置为起始点
		{
			(RGB->line)++;//起始点
		}
		else//移动
		{
			(RGB->line)=Start+count;
		}
		for(j=0;j<count;j++)//拖尾效果
		{
			
			
			if((RGB->line)-j>=0&&(RGB->line)-j<=Stop)//i-j表示数组不越界，越界的不执行，i-j<=Stop表示超过显示区间的不执行
			{
				tempRGB=RGB_Method(count,j);//改变颜色
				RGB_SetOne_Color(((RGB->line)-j),tempRGB);
				//////////
				
			}
			else if((RGB->line)-Stop-j+Start-1>= 0&&(RGB->line)>Stop)//i-Stop-j表示数组不越界，越界的不执行,i-Stop-j+Start-1:i-Stop表示多出的i的部分，+start表示将该部分移到起点，-j表示拖尾，-1表示去除不显示的部分
			{
				tempRGB=RGB_Method(count,j);//改变颜色
				/////////
				RGB_SetOne_Color((RGB->line)-Stop-j+Start-1,tempRGB);
			}
			
		}

	}
	else//负向移动
	{
		if((RGB->line)==255)
		{
			(RGB->line)=Start-count+1;//初始出发点:+2中的一个1是数组的，一个是不亮的灯的
		}
		if((RGB->line)>Stop-count)//Stop-count表示i还没到停止点
		{
			(RGB->line)--;
		}
		else
		{
			(RGB->line)=Start-count;//设置第二轮以上的点
		}
		for(j=0;j<count;j++)//颗粒显示
		{
			
			
			if((RGB->line)+j>=0&&(RGB->line)+j>=Stop&&(RGB->line)+j<Start)//i+j>=0表示数组不越界，越界的不执行，i+j<=Stop表示超过显示区间的不执行
			{
				////////
				tempRGB=RGB_Method(count,j);//改变颜色
				
				RGB_SetOne_Color(((RGB->line)+j),tempRGB);
			}
			else if(Start-(Stop-(RGB->line)-j)+1>=0&&(RGB->line)<Stop&&Start-(Stop-(RGB->line)-j)+1<=Start)//Start-(Stop-i-j)+1表示数组不越界，越界的不执行
			{
				//////
				tempRGB=RGB_Method(count,j);//改变颜色
				
				RGB_SetOne_Color(Start-(Stop-(RGB->line)-j)+1,tempRGB);//表示不满足上面的第一个判断的，开始在Start+j的地方继续画粒子
			}
			
		}
	}

}




RGB_Color_TypeDef Method_00(int count,int j)//渐变流水灯
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


RGB_Color_TypeDef Method_01(int count,int j)//渐变不流水灯
{
	static char i=0;
	uint8_t speed=0;//与变化速度有关的量
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
RGB_Color_TypeDef Method_02(int count,int j)//彩虹灯珠流水
{
	static int k=0;
	static float i=0;//计算颜色的x轴数据
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
RGB_Color_TypeDef Method_03(int count,int j)//彩虹灯珠流水
{
	static uint8_t k=0;
	static int i=0;//计算颜色的x轴数据
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
//将Colorfulx映射到彩色区域，返回对应Colorfulx位置的颜色
//Colorfulx是0~767中的一个值
void RGB_Show_64(void)   
{
	//清屏
	static int a=0;

	RGB_SetMore_Color(0,Pixel_NUM,BLACK);
	
//	if(i>=16777216){i=0;}
//	RGB_0=hsl_to_rgb(i);
//	
//       i++; // 更新循环变量
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
  //刷新WS2812B的显示
	RGB_Flush();
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
	
}


