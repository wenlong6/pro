#ifndef _IMAGE_ROTAND_CROSS_H
#define _IMAGE_ROTAND_CROSS_H

#include "sys.h"

extern float left_angle[EDGELINE_LENGTH];
extern float right_angle[EDGELINE_LENGTH];

//次要函数
void box(uchar(*img)[kWidth * 2], int Y, int X);

//主要函数
uchar CrossFindCorner(int* corner_id_l, int* corner_id_r, int lstep_num, int rstep_num);
void local_angle_points(myPoint_f* in_line, int num, float angle_out[], int dist);

#endif
