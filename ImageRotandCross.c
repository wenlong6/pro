#include "ImageRotandCross.h"
#include "sys.h"

float left_angle[EDGELINE_LENGTH] = { 0 };
float right_angle[EDGELINE_LENGTH] = { 0 };

void box(uchar(*img)[kWidth * 2], int Y, int X) {
    short list[16][2] = { {-2,-2},{-2,-1},{-2,0},{-2,1},{-2,2},
            {-1,-2},{-1,2},{0,-2},{0,2},{1,-2},{1,2},
            {2,-2},{2,-1},{2,0},{2,1},{2,2} };
    if (X > 2 && Y > 2 && X < kWidth * 2 - 2 && Y < kHeight * 2 - 2) {
        for (int i = 0; i < 16; i++) {
            img[Y + list[i][0]][X + list[i][1]] = WHITE;
        }

    }
}

/***********************************************
* @brief : 边线局部角度变化率
* @param : myPoint_f* in_line: 输入边线
*          int num: 输入边线的长度
*          float angle_out[]: 局部角度变化率数组（单位：弧度制）
*          int dist: 三个点求曲率的两点之间的间隔的数量
* @return: 无
* @date  : 2023.1.15
* @author: 上交大开源
************************************************/
void local_angle_points(myPoint_f* in_line, int num, float angle_out[], int dist)
{
    for (int i = 0; i < num; i++)
    {
        if (i <= 0 || i >= num - 1)
        {
            angle_out[i] = 0;
            continue;
        }
        float dx1 = in_line[i].x - in_line[Limit(i - dist, 0, num - 1)].x;
        float dy1 = in_line[i].y - in_line[Limit(i - dist, 0, num - 1)].y;
        float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);
        float dx2 = in_line[Limit(i + dist, 0, num - 1)].x - in_line[i].x;
        float dy2 = in_line[Limit(i + dist, 0, num - 1)].y - in_line[i].y;
        float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);
        float c1 = dx1 / dn1;
        float s1 = dy1 / dn1;
        float c2 = dx2 / dn2;
        float s2 = dy2 / dn2;
        angle_out[i] = atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1);
    }
}

/***********************************************
* @brief : 找十字角点
* @param : corner_id_l:左角点在第几个点
*          corner_id_r:右角点在第几个点
* @return: 0：没有角点
*          1:找到两个角点
*          2:找到一个角点，且另一边丢线
* @date  : 2023.3.21
* @author: L
************************************************/
uchar CrossFindCorner(int* corner_id_l, int* corner_id_r, int lstep_num, int rstep_num)
{
    uchar cross_find_l = FALSE, cross_find_r = FALSE;//是否找到角点标志位

    //找左角点
    if (lstep_num > 30) {
        int max_angle_left_k = 0;
        for (int k = 10; k < lstep_num - 10; k++) {
            if (abs(left_angle[k + 2] - left_angle[k - 2]) > 0.7 && max_angle_left_k < k) {
                max_angle_left_k = k;
            }
        }
        if (max_angle_left_k != 0) {
            cross_find_l = TRUE;
            *corner_id_l = max_angle_left_k;
            printf("l:(%d, %d)\n", (int)left_line_b[max_angle_left_k].y, (int)left_line_b[max_angle_left_k].x);
            box(line_image_turn, (int)left_line_b[max_angle_left_k].y, (int)left_line_b[max_angle_left_k].x);
        }
    }

    //找右角点
    if (rstep_num > 30) {
        int max_angle_right_k = 0;
        for (int k = 10; k < rstep_num - 10; k++) {
            if (abs(right_angle[k + 2] - right_angle[k - 2]) > 0.7 && max_angle_right_k < k) {
                max_angle_right_k = k;
            }
        }
        if (max_angle_right_k != 0) {
            cross_find_r = TRUE;
            *corner_id_r = max_angle_right_k;
            printf("r:(%d, %d)\n", (int)right_line_b[max_angle_right_k].y, (int)right_line_b[max_angle_right_k].x);
            box(line_image_turn, (int)right_line_b[max_angle_right_k].y, (int)right_line_b[max_angle_right_k].x);
        }
    }
    
    //如果两边都找到角点，返回1，一边找到角点且另一边丢线，返回2或3，否则返回0
    if (cross_find_l == TRUE && cross_find_r == TRUE)
    {
        //if ((f_left_line1[per_l_line_count - 1].X < f_left_line1[*corner_id_l].X) || (f_right_line1[per_r_line_count - 1].X > f_right_line1[*corner_id_r].X))
            return 1;
    }
    else if (cross_find_l == TRUE && cross_find_r == FALSE)
    {
        //if (f_left_line1[per_l_line_count - 1].X < f_left_line1[*corner_id_l].X)
            return 2;
    }
    else if (cross_find_r == TRUE && cross_find_l == FALSE)
    {
        //if (f_right_line1[per_r_line_count - 1].X > f_right_line1[*corner_id_r].X)
            return 3;
    }
    return 0;
}

