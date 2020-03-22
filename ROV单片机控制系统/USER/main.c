/*
 *                             _ooOoo_
 *                            o8888888o
 *                            88" . "88
 *                            (| -_- |)
 *                            O\  =  /O
 *                         ____/`---'\____
 *                       .'  \\|     |//  `.
 *                      /  \\|||  :  |||//  \
 *                     /  _||||| -:- |||||-  \
 *                     |   | \\\  -  /// |   |
 *                     | \_|  ''\---/''  |   |
 *                     \  .-\__  `-`  ___/-. /
 *                   ___`. .'  /--.--\  `. . __
 *                ."" '<  `.___\_<|>_/___.'  >'"".
 *               | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *               \  \ `-.   \_ __\ /__ _/   .-` /  /
 *          ======`-.____`-.___\_____/___.-`____.-'======
 *                             `=---='
 *          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 *                    ���汣��        һ����Ը
*/
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "pid.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "lcd.h"
#include "control.h"
#include "mpu6050.h"
#include "usmart.h"   
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
 

int ROVmode; 
float pitch,roll,yaw; 		//ŷ����
	
/***************************************************************************/
/*                              MPU6050��̬����                            */	
void usart1_send_char(u8 c);
void usart1_niming_report(u8 fun,u8*data,u8 len);
void mpu6050_send_data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz);
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw);
 	

//MPU6050��̬���㴫������λ��
 int main(void)
 {	
	 
	PID_init();
	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(500000);	 	//���ڳ�ʼ��Ϊ500000
	delay_init();	//��ʱ��ʼ�� 
	usmart_dev.init(72);		//��ʼ��USMART
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
	LCD_Init();			   		//��ʼ��LCD  
	MPU_Init();					//��ʼ��MPU6050
 	POINT_COLOR=RED;			//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"STM32");	
	LCD_ShowString(30,70,200,16,16,"MPU6050 PWM PID");	
	LCD_ShowString(30,90,200,16,16,"RUN");
	LCD_ShowString(30,110,200,16,16,"2020/2/4");	

	LCD_ShowString(30,130,200,16,16,"MPU6050 OK");
	LCD_ShowString(30,150,200,16,16,"KEY0:UPLOAD ON/OFF");
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
 	LCD_ShowString(30,170,200,16,16,"UPLOAD ON ");	 
 	LCD_ShowString(30,200,200,16,16," Temp:    . C");	
 	LCD_ShowString(30,220,200,16,16,"Pitch:    . C");	
 	LCD_ShowString(30,240,200,16,16," Roll:    . C");	 
 	LCD_ShowString(30,260,200,16,16," Yaw :    . C");	 
	
	
	
	PID_init(); //PID��ʼ������

	
	/***************************************************************************/
	/*                       PWM��ʼ��&&�������                               */
	TIM3_PWM_Init(1999,71);     //����Ϊ����2ms
	TIM8_PWM_Init(1999,71);
	TIM2_PWM_Init(1999,71);
	
	
	/*��������������һ��ʹ��ʱ�����г�*/
	/*ȫΪ���ϼ����Ҹߵ�ƽ��Ч����ʱ�������*/
	TIM_SetCompare1(TIM3,10);
	TIM_SetCompare2(TIM3,10);
  TIM_SetCompare1(TIM8,10);
  TIM_SetCompare2(TIM8,10);
  TIM_SetCompare2(TIM2,10);
  TIM_SetCompare3(TIM2,10);	 
	delay_ms(1500);       //���ݵ��ʹ��˵������
	TIM_SetCompare1(TIM3,500);
	TIM_SetCompare2(TIM3,500);
  TIM_SetCompare1(TIM8,500);
  TIM_SetCompare2(TIM8,500);
  TIM_SetCompare2(TIM2,500);
  TIM_SetCompare3(TIM2,500);	
  delay_ms(1000);	
	
	
	//TIM7���ڶ�ʱ�ɼ����ݽ���pid�㷨
	time7_init(71,9999);//ÿ10ms�ɼ�pid����һ�μ���
  NVIC_INIT();
	/***************************************************************************/	
	/*                               ���غ���                                  */
	
	while(1)
{
	u8 t=0,report=1;			//Ĭ�Ͽ����ϱ�
	u8 key;
	short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
	short gyrox,gyroy,gyroz;	//������ԭʼ����
	short temp;					//�¶�	
	
	
	//���ñ���ϵ��
	PIDSetKp(12,12,12);
	PIDSetKi(2,2,2);
	PIDSetKd(2,2,2);
		
	while(mpu_dmp_init())
 	{
		LCD_ShowString(30,130,200,16,16,"MPU6050 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}  
	
  /***************************************************************************/
	/*                              ��̬����                                   */
		key=KEY_Scan(0);
	
		if(key==KEY0_PRES)
		{
			report=!report;
			if(report)LCD_ShowString(30,170,200,16,16,"UPLOAD ON ");
			else LCD_ShowString(30,170,200,16,16,"UPLOAD OFF");
		}
		
		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
		{ 
			temp=MPU_Get_Temperature();	//�õ��¶�ֵ
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
			if(report)mpu6050_send_data(aacx,aacy,aacz,gyrox,gyroy,gyroz);//���Զ���֡���ͼ��ٶȺ�������ԭʼ����
			if(report)usart1_report_imu(aacx,aacy,aacz,gyrox,gyroy,gyroz,(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
			if((t%10)==0)
			{ 
				if(temp<0)
				{
					LCD_ShowChar(30+48,200,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,200,' ',16,0);		//ȥ������ 
				LCD_ShowNum(30+48+8,200,temp/100,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,200,temp%10,1,16);		//��ʾС������ 
				temp=pitch*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,220,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,220,' ',16,0);		//ȥ������ 
				LCD_ShowNum(30+48+8,220,temp/10,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,220,temp%10,1,16);		//��ʾС������ 
				temp=roll*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,240,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,240,' ',16,0);		//ȥ������ 
				LCD_ShowNum(30+48+8,240,temp/10,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,240,temp%10,1,16);		//��ʾС������ 
				temp=yaw*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,260,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,260,' ',16,0);		//ȥ������ 
				LCD_ShowNum(30+48+8,260,temp/10,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,260,temp%10,1,16);		//��ʾС������  
				t=0;
				LED0=!LED0;//LED��˸
			}
		}
		t++; 
		
		
/***************************************************************************/		
/*                               ��е�۵Ŀ���                              */	

 TIM1_Init();    //��ʼ��TIM1

 static int MAmode=5;  //��¼��ʱ��е�۶������λ��
 int reset=0;          //���尴��δ���µ�״̬
 int control1=reset;   //�ӽ����� δ����
 int control2=reset;   //�ɿ����� δ����
				
	if(control1!=reset&&MAmode!=9)    //�ж��Ƿ�Ϊ �ӽ�����
{  MAmode++; }                                                 //���������С״̬ʱ������һ������
 else if(control2!=reset&&MAmode!=1)  //�ж��Ƿ�Ϊ �ɿ�����
{  MAmode--; }          
 
   switch(MAmode)
	{
			case(1):
		{
		TIM_SetCompare1(TIM1, 1000);
    delay_ms(500);
		 }   break;
				 
		 	case(2):
		{
		TIM_SetCompare1(TIM1, 1125);
    delay_ms(500);
		 }   break;
				 
		 	case(3):
		{
		TIM_SetCompare1(TIM1, 1250);
    delay_ms(500);
		 }   break;
				 
		 case(4):
		{
		TIM_SetCompare1(TIM1, 1375);
    delay_ms(500);
		 }   break;

		 case(5):
		{
		TIM_SetCompare1(TIM1, 1500);  //��ʼ״̬�£�Ϊ1500��Ϊ�ص��м�
    delay_ms(500);
		 }   break;
		 
		 case(6):
		{
		TIM_SetCompare1(TIM1, 1625);
    delay_ms(500);
		 }   break;

		 case(7):
		{
		TIM_SetCompare1(TIM1, 1750);
    delay_ms(500);
		 }   break;

		 case(8):
		{
		TIM_SetCompare1(TIM1, 1875);
    delay_ms(500);
		 }   break;

		 case(9):
		{
		TIM_SetCompare1(TIM1, 2000);
    delay_ms(500);
		 }   break;
		 
		 default:  break;
	}


	
/***************************************************************************/		
/*                               ��̨�Ŀ���                                */	
	
 TIM1_Init();     //��ʼ��TIM1
 
 static int HDmode=9;  //��¼��ʱ��̨�������λ��
 int control3=reset;   //̧������ δ����
 int control4=reset;   //�½����� δ����

	if(control3!=reset&&HDmode!=17)    //�ж��Ƿ�Ϊ ̧������
{  HDmode++; }                                                 //���������С״̬ʱ������һ������
 else if(control4!=reset&&HDmode!=1)  //�ж��Ƿ�Ϊ �½�����
{  HDmode--; }          
 
   switch(HDmode)
	{
			case(1):
		{
		TIM_SetCompare2(TIM1, 1020);
    delay_ms(300);
		 }   break;
				 
		 	case(2):
		{
		TIM_SetCompare2(TIM1, 1080);
    delay_ms(300);
		 }   break;
				 
		 	case(3):
		{
		TIM_SetCompare2(TIM1, 1140);
    delay_ms(300);
		 }   break;
				 
		 case(4):
		{
		TIM_SetCompare2(TIM1, 1200);
    delay_ms(300);
		 }   break;

		 case(5):
		{
		TIM_SetCompare2(TIM1, 1260);
    delay_ms(300);
		 }   break;
		 
		 case(6):
		{
		TIM_SetCompare2(TIM1, 1320);
    delay_ms(300);
		 }   break;

		 case(7):
		{
		TIM_SetCompare2(TIM1, 1380);
    delay_ms(300);
		 }   break;

		 case(8):
		{
		TIM_SetCompare2(TIM1, 1440);
    delay_ms(300);
		 }   break;

		 case(9):
		{
		TIM_SetCompare2(TIM1, 1500);
    delay_ms(300);
		 }   break;
		 
		 case(10):
		{
		TIM_SetCompare2(TIM1, 1560);
    delay_ms(300);
		 }   break;
				 
		 	case(11):
		{
		TIM_SetCompare2(TIM1, 1620);
    delay_ms(300);
		 }   break;
				 
		 	case(12):
		{
		TIM_SetCompare2(TIM1, 1680);
    delay_ms(300);
		 }   break;
				 
		 case(13):
		{
		TIM_SetCompare2(TIM1, 1740);
    delay_ms(300);
		 }   break;

		 case(14):
		{
		TIM_SetCompare2(TIM1, 1800);
    delay_ms(300);
		 }   break;
		 
		 case(15):
		{
		TIM_SetCompare2(TIM1, 1860);
    delay_ms(300);
		 }   break;

		 case(16):
		{
		TIM_SetCompare2(TIM1, 1920);
    delay_ms(300);
		 }   break;

		 case(17):
		{
		TIM_SetCompare2(TIM1, 1980);
    delay_ms(300);
		 }   break;
		 default:  break;
	}


	/***************************************************************************/
	/*                          ������pid�㷨                                  */	
	ROVmode=1;
	ROV_Control();          //���ƺ���		
 }
}

	