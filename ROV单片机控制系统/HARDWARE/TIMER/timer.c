#include "timer.h"
#include "led.h"
#include "usart.h"
#include "pid.h"
#include "mpu6050.h"

//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//ʹ�õ��Ƕ�ʱ��3
void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
  TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);   //ʹ��ָ����TIM3�ж�
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ�� 
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����			 
}

//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIO���� 
 
   //���TIM3 CH2��PWM���岨��	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //TIM_CH1&CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channel2 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode= TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽΪ����Ƚ�ģʽ
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽΪ����Ƚ�ģʽ
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2

  TIM_CtrlPWMOutputs(TIM3,ENABLE);	
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  
 
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
	TIM_ARRPreloadConfig(TIM3, ENABLE); 
}

//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );
		}
}



//ͨ�ö�ʱ��8�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//ʹ�õ��Ƕ�ʱ��8
void TIM8_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

                                                             
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); 

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OC1Init(TIM8, &TIM_OCInitStructure); 

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OC2Init(TIM8, &TIM_OCInitStructure); 

  TIM_CtrlPWMOutputs(TIM8,ENABLE);	

	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);  

	TIM_ARRPreloadConfig(TIM8, ENABLE); 
	TIM_Cmd(TIM8, ENABLE); 
}


//��ʱ��8�жϷ������
void TIM8_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
		{
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update  );
		}
}



//ͨ�ö�ʱ��2�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//ʹ�õ��Ƕ�ʱ��2
void TIM2_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

                                                             
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OC2Init(TIM2, &TIM_OCInitStructure); 

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OC3Init(TIM2, &TIM_OCInitStructure); 

  TIM_CtrlPWMOutputs(TIM2,ENABLE);	

	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  

	TIM_ARRPreloadConfig(TIM8, ENABLE); 
	TIM_Cmd(TIM8, ENABLE); 
}


//��ʱ��8�жϷ������
void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
		{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );
		}
}



//TIM7���ڸ�PID�����ṩ��ʱ��
//pid���ڲ���
extern float pitch,roll,yaw; 		//ŷ����
extern struct _pid pid_roll,pid_pitch,pid_yaw;

void time7_init(u16 per,u16 pre)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period=per;
    TIM_TimeBaseStructure.TIM_Prescaler=pre;
    TIM_TimeBaseInit(TIM7,&TIM_TimeBaseStructure);

    TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM7,ENABLE);
}
void NVIC_INIT(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM7_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM7,TIM_IT_Update)!=RESET)
    {
        TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
    }
		
		pid_roll.Actualangle=roll;     //��¼��̬��
		pid_pitch.Actualangle=pitch;
		pid_yaw.Actualangle=yaw;
		
		roll_pitch_yaw_anglePID(0,0,0);  //����pid���ƺ�����ֵ
}



/*****************************************************************/
/*     �������Ҫ������TIM1�������PWM������Ϊ����TIM1�Ĵ���     */
//��ʼ������GPIO��
//static void TIM1_GPIO_Config(void) 
//{
//  GPIO_InitTypeDef GPIO_InitStructure;

//  /* TIM1 clock enable */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

//  /* GPIOA and GPIOB clock enable */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
//    
//  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_10;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

//  GPIO_Init(GPIOA, &GPIO_InitStructure);

//  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14 | GPIO_Pin_15;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

//  GPIO_Init(GPIOB, &GPIO_InitStructure);

//}

////��ʼ����ʱ����������
//u16 CCR2_Val = 500;
//u16 CCR3_Val = 500;//ռ�ձ�

///* ����TIM1���PWM��ģʽ */
//void TIM1_Mode_Config(void)
//{
//    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//    TIM_BDTRInitTypeDef      TIM1_BDTRInitStruct;
//    TIM_OCInitTypeDef        TIM_OCInitStructure;

//    /* Time base configuration */
//    TIM_TimeBaseStructure.TIM_Period = 1000-1; 
//    TIM_TimeBaseStructure.TIM_Prescaler = 72-1;//1ms
//    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//��������Ƶ�й�
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
//    
//    /****** ����BDTR�Ĵ�������������ʱ��******/
//    /*
//       ????? 72M   TIM_ClockDivision = TIM_CKD_DIV1?,  Tdts = 13.89ns
//       0 - 1.764us  ????
//       1.778us - 3.505us  ????
//       3.556us - 7.000us  ???? 
//       7.1117us - 14us    ????
//       ??????,??TIM_ClockDivision??
//    */
//    TIM1_BDTRInitStruct.TIM_OSSRState = TIM_OSSRState_Disable;
//    TIM1_BDTRInitStruct.TIM_OSSIState = TIM_OSSIState_Disable;
//    TIM1_BDTRInitStruct.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
//    TIM1_BDTRInitStruct.TIM_DeadTime = 205; //????  72:1us 172:3us 205:5us
//    TIM_BDTRConfig(TIM1,&TIM1_BDTRInitStruct);
// 
////    TIM1->BDTR |= 72;   //��������
//    
//     /* PWM1 Mode configuration: Channel2 */
//     TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
//     TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ��������
//     TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;//�Ƚϻ������ʹ��
//     TIM_OCInitStructure.TIM_Pulse = CCR2_Val;  
//     TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  
//     TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;//�����������
//     TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//����״̬������Ƚϵ�����״̬
//     TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
//     TIM_OC2Init(TIM1, &TIM_OCInitStructure);  
//     TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);      //�Զ���װ��ʹ��


//    /* PWM1 Mode configuration: Channel3 */
//    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
//    TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
//    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
//    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
//    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;      
//    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
//    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);    

//    TIM_ARRPreloadConfig(TIM1, ENABLE);//������Ч

//    /* TIM1 enable counter */
//    TIM_Cmd(TIM1, ENABLE);
//    
//    TIM_CtrlPWMOutputs(TIM1, ENABLE);  
//}

