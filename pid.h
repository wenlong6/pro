#ifndef __PID_H
#define	__PID_H

#include "sys.h"

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

extern PID addPID;

void  PID_param_init(void);
void  set_PID_target(float temp_val);
float get_pid_target(PID *pid);
void  set_PID(float p, float i, float d);
float addPID_realize(PID *pid, float actual_val);

#endif


