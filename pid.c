#include "stm32f10x.h"                  // Device header
#include "Encoder.h"
//����ʽPID

//float Err = 0,LastErr=0,LastLastErr=0;
//float pwm_CCR = 0,Add_CCR = 0;
//float p = 0.4, i =0.2, d=0;
//float setspeed = 30;   //����Ŀ���ٶ�


//float PID(int16_t speed)
//{
//	Err = setspeed - speed;
//	Add_CCR = p*(Err - LastErr)+i*(Err)+d*(Err+LastLastErr-2*LastErr);
//	if(Add_CCR<-1 || Add_CCR>1)  //�޷�
//	{
//		pwm_CCR += Add_CCR;
//	}
//	if(pwm_CCR>999)
//		pwm_CCR=999;
//	if(pwm_CCR<0)
//		pwm_CCR=0;
//	LastLastErr = LastErr;  //��һ���������ϴ�
//	LastErr = Err;  //���������ϴ����
//	return pwm_CCR;   //����PWM�µ�ռ�ձ�
//}
// 
typedef struct
{
	float target_val;   //Ŀ��ֵ
	float Error;          /*�� k ��ƫ�� */
	float LastError;     /* Error[-1],�� k-1 ��ƫ�� */
	float PrevError;    /* Error[-2],�� k-2 ��ƫ�� */
	float Kp,Ki,Kd;     //���������֡�΢��ϵ��
	float integral;     //����ֵ
	float output_val;   //���ֵ
}PID;

PID addPID;

void PID_param_init()
{
	float pid_temp[3];

	/* ��ʼ������(������λ����, ��ʵʱ�޸�PID����) */
	addPID.target_val=60;				
	addPID.output_val=0.0;
	addPID.Error=0.0;
	addPID.LastError=0.0;
	addPID.PrevError=0.0;
	addPID.integral=0.0;

	addPID.Kp = 10;
	addPID.Ki = 1.0;
	addPID.Kd = 0.0;

	pid_temp[0] = addPID.Kp;
	pid_temp[1] = addPID.Ki;
	pid_temp[2] = addPID.Kd;
}


void set_PID_target(float temp_val)
{  
	addPID.target_val = temp_val;    // ���õ�ǰ��Ŀ��ֵ
}

float get_pid_target(PID *pid)
{
  return pid->target_val;    // ��ȡ��ǰ��Ŀ��ֵ
}

void set_PID(float p, float i, float d)
{
	addPID.Kp = p;    // ���ñ���ϵ�� P
	addPID.Ki = i;    // ���û���ϵ�� I
	addPID.Kd = d;    // ����΢��ϵ�� D
}

float addPID_realize(PID *pid, float actual_val)
{
	/*����Ŀ��ֵ��ʵ��ֵ�����*/
	pid->Error = pid->target_val - actual_val;
	
	/*������*/
	pid->integral += pid->Error;

	/*PID�㷨ʵ��*/
	pid->output_val = pid->Kp * (pid->Error - pid-> LastError) +
	                  pid->Ki * pid->Error +
	                  pid->Kd *(pid->Error -2*pid->LastError+pid->PrevError);
	/*����*/
	pid-> PrevError = pid->LastError;
	pid-> LastError = pid->Error;

	/*���ص�ǰʵ��ֵ*/
	return pid->output_val;
}




