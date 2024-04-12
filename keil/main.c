#include <reg52.h>

#define ABS(x) ((x > 0)?(x):(-x))
typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;

sbit LSA = P2^0;
sbit LSB = P2^1;
sbit LSC = P2^2;

u8 display_bit = 1;
u16 rpm = 0;
u16 pwm_num = 0;
u16 last_rpm = 0;
u16 show_rpm = 0;
u8 DisplayData[4] = {0};
u8 code smgduan[17]={0xc0, 0xf9, 0xa4, 0xb0,0x99,0x92,0x82,0xf8,
			0x80, 0x90, 0x88,0x83,0xc6,0xa1,0x86,0x8e};//��ʾ0~F��ֵ

//���尴���˿�
sbit PWR = P3^0; 
sbit SEG = P3^1;	
sbit PWM = P3^2;

void Int0Init(void);
void Timer0Init(void);
void DigDisplay(void);
void datapros(void);
void delay(u16 i);

void main()
{
	Int0Init();
	Timer0Init();
	while (!PWR) {
		datapros();
		if (!SEG) {
			EA = 1;
		} else {
			EA = 0;
		}
		DigDisplay();
	}
}

//����ܶ�̬ɨ�躯����ѭ��ɨ��8���������ʾ
void DigDisplay(void)
{
	u8 i;
	for (i = 0; i < 4; i++) {
		switch(i) {	 //λѡ��ѡ�����������ܣ�
		
			case(0):
				LSA = 0; LSB = 0; LSC = 0; break;//��ʾ��0λ
			case(1):
				LSA = 1; LSB = 0; LSC = 0; break;//��ʾ��1λ
			case(2):
				LSA = 0; LSB = 1; LSC = 0; break;//��ʾ��2λ
			case(3):
				LSA = 1; LSB = 1; LSC = 0; break;//��ʾ��3λ
		}
		P0 = DisplayData[i];//���Ͷ���
		delay(70); //���һ��ʱ��ɨ��
		P0 = 0xff;//����
	}
}

void Int0Init(void)
{
	//����INT0
	IT0 = 1; //�����س�����ʽ���½��أ�
	EX0 = 1; //��INT0���ж�����	
	EA = 1;  //�����ж�	
}

//��ʱ��0��ʼ��
void Timer0Init(void)
{
	TMOD |= 0X01; //ѡ��Ϊ��ʱ��0ģʽ��������ʽ1������TR0��������

	TH0 = 0XFC;   //����ʱ������ֵ����ʱ1ms
	TL0 = 0X18;	
	ET0 = 1;      //�򿪶�ʱ��0�ж�����
	EA  = 1;      //�����ж�
	TR0 = 1;      //�򿪶�ʱ��			
}

//����
void datapros(void)
{
	u8 i, j, k, l;
	if (ABS(last_rpm - rpm) > 240)
		display_bit = 1;
	if (display_bit) {
		i = show_rpm % 10;
		j = show_rpm / 10 % 10;
		k = show_rpm / 100 % 10;
		l = show_rpm / 1000;
		DisplayData[3] = smgduan[i];
		DisplayData[2] = smgduan[j];
		DisplayData[1] = smgduan[k];
		DisplayData[0] = smgduan[l];
//		DisplayData[3] = smgduan[pwm_num % 10];
//		DisplayData[2] = smgduan[pwm_num / 10];
//		DisplayData[1] = smgduan[pwm_num / 100];
//		DisplayData[0] = smgduan[pwm_num / 1000];
		display_bit = 0;
	}
}

//mode=0 ����   1������

void delay(u16 i)
{
	while(i--);	
}

//�������
void Int0(void) interrupt 0		
{
	if (PWM == 0) {
		pwm_num++;
	}
}

void Timer0(void) interrupt 1
{
	static u16 i;
	TH0 = 0XFC;	//����ʱ������ֵ����ʱ1ms
	TL0 = 0X18;
	i++;
	if (i == 1000) {
		i = 0;
		rpm = pwm_num * 60;
		last_rpm = rpm;
		show_rpm = rpm;
		pwm_num = 0;
	}
}
