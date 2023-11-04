#include "stm32f10x.h"                  // Device header
#include "Encoder.h"
//增量式PID

//float Err = 0,LastErr=0,LastLastErr=0;
//float pwm_CCR = 0,Add_CCR = 0;
//float p = 0.4, i =0.2, d=0;
//float setspeed = 30;   //设置目标速度


//float PID(int16_t speed)
//{
//	Err = setspeed - speed;
//	Add_CCR = p*(Err - LastErr)+i*(Err)+d*(Err+LastLastErr-2*LastErr);
//	if(Add_CCR<-1 || Add_CCR>1)  //限幅
//	{
//		pwm_CCR += Add_CCR;
//	}
//	if(pwm_CCR>999)
//		pwm_CCR=999;
//	if(pwm_CCR<0)
//		pwm_CCR=0;
//	LastLastErr = LastErr;  //上一次误差赋给上上次
//	LastErr = Err;  //最新误差赋给上次误差
//	return pwm_CCR;   //返回PWM新的占空比
//}
// 
typedef struct
{
	float target_val;   //目标值
	float Error;          /*第 k 次偏差 */
	float LastError;     /* Error[-1],第 k-1 次偏差 */
	float PrevError;    /* Error[-2],第 k-2 次偏差 */
	float Kp,Ki,Kd;     //比例、积分、微分系数
	float integral;     //积分值
	float output_val;   //输出值
}PID;

PID addPID;

void PID_param_init()
{
	float pid_temp[3];

	/* 初始化参数(连接上位机后, 可实时修改PID参数) */
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
	addPID.target_val = temp_val;    // 设置当前的目标值
}

float get_pid_target(PID *pid)
{
  return pid->target_val;    // 获取当前的目标值
}

void set_PID(float p, float i, float d)
{
	addPID.Kp = p;    // 设置比例系数 P
	addPID.Ki = i;    // 设置积分系数 I
	addPID.Kd = d;    // 设置微分系数 D
}

float addPID_realize(PID *pid, float actual_val)
{
	/*计算目标值与实际值的误差*/
	pid->Error = pid->target_val - actual_val;
	
	/*积分项*/
	pid->integral += pid->Error;

	/*PID算法实现*/
	pid->output_val = pid->Kp * (pid->Error - pid-> LastError) +
	                  pid->Ki * pid->Error +
	                  pid->Kd *(pid->Error -2*pid->LastError+pid->PrevError);
	/*误差传递*/
	pid-> PrevError = pid->LastError;
	pid-> LastError = pid->Error;

	/*返回当前实际值*/
	return pid->output_val;
}




