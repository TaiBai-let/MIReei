#ifndef __PID_H
#define __PID_H	

//�������
struct _pid{
    float Setangle;            //���ýǶ�
    float Actualangle;         //ʵ�ʽǶ�
    float err;                 //ƫ��ֵ
    float err_last;            //��һ��ƫ��ֵ
    float Kp,Ki,Kd;            //���������֣�΢��ϵ��
    float control;             //����ִ�����ı���
    float integral;            //����ֵ
    float umax;                //�������ֵ
    float umin;                //������Сֵ
};

void PID_init(void);     //��ʼ������
void PIDSetKp(float Kp1,float Kp2,float Kp3);    //���ñ���ϵ��
void PIDSetKi(float Ki1,float Ki2,float Ki3);    //���û���ϵ��
void PIDSetKd(float Kd1,float Kd2,float Kd3);    //����΢��ϵ��
float PID_realize_roll(float angle_roll);        //ʵ��roll�Ƕȵ�PID����
float PID_realize_pitch(float angle_pitch);      //ʵ��pitch�Ƕȵ�PID����
float PID_realize_yaw(float angle_yaw);          //ʵ��yaw�Ƕȵ�PID����
float Math_fConstrain(float value, float min, float max);   //���ƿ���PWM�źŵķ���
void roll_pitch_yaw_anglePID(float angle_roll,float angle_pitch,float angle_yaw);  //��̬��PID���ƺ���

#endif

