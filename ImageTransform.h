#pragma once
#ifndef _IMAGE_TRANSFORM_H
#define _IMAGE_TRANSFORM_H

#include "sys.h"

#define LINE_BLUR_KERNEL    7               //边线三角滤波核的大小
#define BEGIN_TO_FIND		10

typedef enum kFace {
	kLeft=0, kFont, kRight, kBack
}kFace;

extern uchar line_image[kHeight][kWidth]; //边线图
extern uchar line_image_turn[kHeight * 2][kWidth * 2]; //透视后的边线图
extern uchar binary_image[kHeight][kWidth]; //二值化图
extern uchar gray_image[kHeight][kWidth]; //灰度图
extern myPoint left_line[EDGELINE_LENGTH], right_line[EDGELINE_LENGTH];//左右边线
extern myPoint_f left_line_a[EDGELINE_LENGTH], right_line_a[EDGELINE_LENGTH];//逆透视后的左右边线
extern myPoint_f left_line_b[EDGELINE_LENGTH], right_line_b[EDGELINE_LENGTH];//滤波后的左右边线
extern myPoint_f center_line_l[EDGELINE_LENGTH], center_line_r[EDGELINE_LENGTH];//根据左右边线得出的中线
extern myPoint_f center_line[EDGELINE_LENGTH];//归一化中线

//次要函数
uchar getAbit(uchar(*img)[kWidth], myPoint* p);
void setmyPoint(myPoint* p, int y, int x);
void setmyPoint_f(myPoint_f* p, float y, float x);
void writeABit(uchar(*img)[kWidth], myPoint* p);
void writeABit2(uchar(*img)[kWidth * 2], myPoint_f* p);
void clearLine(uchar(*img)[kWidth]);
void clearLine2(uchar(*img)[kWidth * 2]);

inline int Limit(int x, int low, int up) { //给x设置上下限幅
	return x > up ? up : x < low ? low : x;
}
inline float Min(float a, float b) { return a < b ? a : b; } //求ab最小值
inline float Max(float a, float b) { return a > b ? a : b; } //求ab最大值

//主要函数
int OTSU(uchar(*img_g)[kWidth]);
void threshold(uchar(*img_g)[kWidth], int thr);
void findLinePoints(uchar(*img)[kWidth]);
void steptoLine(uchar(*img)[kWidth], int* lstep_num, int* rstep_num, int lfrom, int rfrom);
void edgeLinePerspective(myPoint* in_line, uchar num, myPoint_f* out_line);
void blurPoints(myPoint_f* in_line, int num, myPoint_f* out_line, uchar kernel);
void track_leftline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist);
void track_rightline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist);
void cornerRefindLine(uchar corner_condition);

#endif
