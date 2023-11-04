#ifndef _SYS_H
#define _SYS_H

#define EDGELINE_LENGTH 125  //Ѳ�߳���
#define kWidth			188
#define kHeight			120
#define FALSE			0
#define TRUE			1
#define BLACK			0
#define WHITE			255

typedef unsigned char uchar;

typedef struct myPoint {
	int x;
	int y;
}myPoint;

typedef struct myPoint_f {
	float x;
	float y;
}myPoint_f;

uchar line_image_turn[kHeight * 2][kWidth * 2];

myPoint_f left_line_b[EDGELINE_LENGTH], right_line_b[EDGELINE_LENGTH];//�˲�������ұ���
myPoint_f center_line_l[EDGELINE_LENGTH], center_line_r[EDGELINE_LENGTH];//�������ұ��ߵó�������
myPoint_f center_line[EDGELINE_LENGTH];//��һ������

#endif