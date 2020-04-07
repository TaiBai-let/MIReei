#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "pid.h"
#include "control.h"
#include "usart.h"
#include "delay.h"
#include "timer.h"


//�������ñ���
extern int ROVmode;
//float PID_dt;
extern struct _pid pid_roll,pid_pitch,pid_yaw;
extern float pitch,roll,yaw; 		//ŷ����

//�˶�����
void ROV_Control(void)
{

		switch(ROVmode)           //ѡ���˶�ģʽ
	{
		case 1:        //ǰ��
		{
    PWM_Forward_OUT();	 		
    } break;

		case 2:       //����
	  {		
     PWM_Back_OUT();
    }break;

		case 3:       //��ת
	  {		
		 PWM_Left_OUT();
    }break;

		case 4:       //��ת
	  {		
		 PWM_Right_OUT();
    }break;

		case 5:       //����
	  {		
     PWM_Up_OUT();
    }break;
	
		case 6:       //�½�
	  {		
     PWM_Down_OUT();
    }break;
		
		case 7:       //����
		{		
     PWM_Leftmove_OUT();
    }break;
		
		case 8:       //����
		{		
     PWM_Rightmove_OUT();
    }break;
		
		case 9:       //��
		{		
     PWM_Leftturn_OUT();
    }break;
		
		case 10:      //�ҷ�
		{		
     PWM_Rightturn_OUT();
    }break;
		
		default : break;
		
	}
}



//---------------����PWM���------------------------------------ 

//ǰ��
void PWM_Forward_OUT(void)	 			 //TIM3��TIM8����
{	
			static int yaw_go_left1=100;
			static int yaw_go_left2=1500;
			static int yaw_go_right1=100;
			static int yaw_go_right2=1500;
		/*TIM3�Ķ�ʱ������ǰ��������ת*/
      TIM_SetCompare1(TIM3,yaw_go_left1);
	    TIM_SetCompare2(TIM3,yaw_go_right1);
		/*TIM8�Ķ�ʱ�����ƺ���������ת*/
	    TIM_SetCompare1(TIM8,yaw_go_left2);
	    TIM_SetCompare2(TIM8,yaw_go_right2);
	
	do{
		  roll_pitch_yaw_anglePID(0,0,0);
		if(pid_yaw.Actualangle<-200)
		{
			//��������ƫ�ƣ���Ҫ����������
			yaw_go_left1+=pid_yaw.control;
			yaw_go_left2+=pid_yaw.control;
		  TIM_SetCompare1(TIM3,yaw_go_left1);    //ͷ��������
	    TIM_SetCompare1(TIM8,yaw_go_left2);    //β��������
		}
		else if(pid_yaw.Actualangle>+200)
		{
			//��������ƫ�ƣ���Ҫ�Ҳ���������
			yaw_go_right1+=pid_yaw.control;
			yaw_go_right2+=pid_yaw.control;
		  TIM_SetCompare2(TIM3,yaw_go_right1);    //ͷ���Ҳ����
	    TIM_SetCompare2(TIM8,yaw_go_right2);    //β���Ҳ����		
		}
	}while(-200<pid_yaw.Actualangle<+200);
}


//����
void PWM_Back_OUT(void)
{
			static int yaw_back_right1=1500;
			static int yaw_back_right2=100;
			static int yaw_back_left1=1500;
			static int yaw_back_left2=100;
		/*TIM3�Ķ�ʱ������ǰ��������ת*/
      TIM_SetCompare1(TIM3,yaw_back_left1);
	    TIM_SetCompare2(TIM3,yaw_back_right1);
		/*TIM8�Ķ�ʱ������ǰ��������ת*/
	 	  TIM_SetCompare1(TIM8,yaw_back_left2);
	    TIM_SetCompare2(TIM8,yaw_back_right2); 	
	
	
	do{
		  roll_pitch_yaw_anglePID(0,0,0);
		if(pid_yaw.Actualangle<-200)
		{
			//��������ƫ�ƣ���Ҫ�Ҳ���������
      yaw_back_right1+=pid_yaw.control;
			yaw_back_right2+=pid_yaw.control;
		  TIM_SetCompare1(TIM3,yaw_back_right1);    //ͷ��������
	    TIM_SetCompare1(TIM8,yaw_back_right2);    //β��������
		}
		else if(pid_yaw.Actualangle>+200)
		{
			//��������ƫ�ƣ���Ҫ����������
      yaw_back_left1+=pid_yaw.control;
			yaw_back_left2+=pid_yaw.control;
		  TIM_SetCompare2(TIM3,yaw_back_left1);    //ͷ���Ҳ����
	    TIM_SetCompare2(TIM8,yaw_back_left2);    //β���Ҳ����		
		}
	}while(-200<pid_yaw.Actualangle<+200);
}


//����
void PWM_Up_OUT(void)
{
			static int roll_up_left=1500;
			static int roll_up_right=100;

      TIM_SetCompare2(TIM2,roll_up_left);
      TIM_SetCompare3(TIM2,roll_up_right);
	
	
	do{
		roll_pitch_yaw_anglePID(0,0,0);
		if(pid_roll.Actualangle<-200)
		{
			//����������б����Ҫ��������������
      roll_up_left+=pid_roll.control;
			roll_up_right-=pid_roll.control;
		  TIM_SetCompare2(TIM2,roll_up_left);    //��ֱ������
	    TIM_SetCompare3(TIM2,roll_up_right);    //��ֱ�Ҳ����
		}
		else if(pid_roll.Actualangle>+200)
		{
			//����������б����Ҫ�Ҳ�������������
			roll_up_left-=pid_roll.control;
      roll_up_right+=pid_roll.control;
		  TIM_SetCompare2(TIM2,roll_up_left);    //��ֱ������
	    TIM_SetCompare3(TIM2,roll_up_right);    //��ֱ�Ҳ����
		}
	}while(-200<pid_roll.Actualangle<+200);
}


//�½�
void PWM_Down_OUT(void)
{
			static int roll_down_left=100;
			static int roll_down_right=1500;
      TIM_SetCompare2(TIM2,roll_down_left);
      TIM_SetCompare3(TIM2,roll_down_right);
	
	
	do{
		  roll_pitch_yaw_anglePID(0,0,0);
		if(pid_roll.Actualangle<-200)
		{
			//����������б����Ҫ���½�������С
      roll_down_left-=pid_roll.control;
			roll_down_right+=pid_roll.control;
		  TIM_SetCompare2(TIM2,roll_down_left);    //��ֱ������
	    TIM_SetCompare3(TIM2,roll_down_right);    //��ֱ�Ҳ����
		}
		else if(pid_roll.Actualangle>+200)
		{
			//����������б����Ҫ�Ҳ��½�������С
      roll_down_left+=pid_roll.control;
			roll_down_right-=pid_roll.control;
		  TIM_SetCompare2(TIM2,roll_down_left);    //��ֱ������
	    TIM_SetCompare3(TIM2,roll_down_right);    //��ֱ�Ҳ����
		}
	}while(-200<pid_roll.Actualangle<+200);
}


//��ת
void PWM_Left_OUT(void)
{
	TIM_SetCompare1(TIM3,100);
	TIM_SetCompare2(TIM3,1500);
  TIM_SetCompare1(TIM8,1500);
  TIM_SetCompare2(TIM8,100);
}


//��ת
void PWM_Right_OUT(void)
{
	TIM_SetCompare1(TIM3,1500);
	TIM_SetCompare2(TIM3,100);
  TIM_SetCompare1(TIM8,100);
  TIM_SetCompare2(TIM8,1500);
}


//����
void PWM_Leftmove_OUT(void)	 			 //TIM3��TIM8����
{	
			static int yaw_leftmove_left1=100;
			static int yaw_leftmove_left2=100;
			static int yaw_leftmove_right1=1500;
			static int yaw_leftmove_right2=1500;

      TIM_SetCompare1(TIM3,yaw_leftmove_left1);
	    TIM_SetCompare2(TIM3,yaw_leftmove_right1);

	    TIM_SetCompare1(TIM8,yaw_leftmove_left2);
	    TIM_SetCompare2(TIM8,yaw_leftmove_right2);
	
	do{
		  roll_pitch_yaw_anglePID(0,0,0);
		if(pid_yaw.Actualangle<-200)
		{
			//��������ƫ�ƣ���Ҫ����������
			yaw_leftmove_left1-=pid_yaw.control;
			yaw_leftmove_right1-=pid_yaw.control;
		  TIM_SetCompare1(TIM3,yaw_leftmove_left1);    //ͷ��������
	    TIM_SetCompare2(TIM3,yaw_leftmove_right1);    //ͷ���Ҳ����
		}
		else if(pid_yaw.Actualangle>+200)
		{
			//��������ƫ�ƣ���Ҫ�Ҳ���������
			yaw_leftmove_left2-=pid_yaw.control;
			yaw_leftmove_right2-=pid_yaw.control;
		  TIM_SetCompare1(TIM8,yaw_leftmove_left2);    //β��������	
	    TIM_SetCompare2(TIM8,yaw_leftmove_right2);    //β���Ҳ����		
		}
	}while(-200<pid_yaw.Actualangle<+200);
}

//����
void PWM_Rightmove_OUT(void)	 			 //TIM3��TIM8����
{	
			static int yaw_rightmove_left1=1500;
			static int yaw_rightmove_left2=1500;
			static int yaw_rightmove_right1=100;
			static int yaw_rightmove_right2=100;

      TIM_SetCompare1(TIM3,yaw_rightmove_left1);
	    TIM_SetCompare2(TIM3,yaw_rightmove_right1);

	    TIM_SetCompare1(TIM8,yaw_rightmove_left2);
	    TIM_SetCompare2(TIM8,yaw_rightmove_right2);
	
	do{
		  roll_pitch_yaw_anglePID(0,0,0);
		if(pid_yaw.Actualangle<-200)
		{
			//��������ƫ�ƣ���Ҫ����������
			yaw_rightmove_left2-=pid_yaw.control;
			yaw_rightmove_right2-=pid_yaw.control;
		  TIM_SetCompare1(TIM3,yaw_rightmove_left2);      //β��������	
	    TIM_SetCompare2(TIM3,yaw_rightmove_right2);     //β���Ҳ����	
		}
		else if(pid_yaw.Actualangle>+200)
		{
			//��������ƫ�ƣ���Ҫ�Ҳ���������
			yaw_rightmove_left1-=pid_yaw.control;
			yaw_rightmove_right1-=pid_yaw.control;
		  TIM_SetCompare1(TIM8,yaw_rightmove_left1);     //ͷ��������	
	    TIM_SetCompare2(TIM8,yaw_rightmove_right1);    //ͷ���Ҳ����
		}
	}while(-200<pid_yaw.Actualangle<+200);
}

//��
void PWM_Leftturn_OUT(void)
{
			static int roll_leftturn_left=100;
			static int roll_leftturn_right=100;

      TIM_SetCompare2(TIM2,roll_leftturn_left);
      TIM_SetCompare3(TIM2,roll_leftturn_right);
}	

//�ҷ�
void PWM_Rightturn_OUT(void)
{
			static int roll_rightturn_left=1500;
			static int roll_rightturn_right=1500;

      TIM_SetCompare2(TIM2,roll_rightturn_left);
      TIM_SetCompare3(TIM2,roll_rightturn_right);
}	

