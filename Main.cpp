#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <windows.h>
#include <iostream>
#include <math.h>

#include "ImageTransform.c"
#include "ImageRotandCross.c"
using namespace std;
using namespace cv;

/***********************************************
* @brief : 十字扫线状态机
* @param : corner_condition 状态
* @return: void
* @date  : 2023.11.2
* @author: DJJ
************************************************/
void edgeDetectionCross(uchar corner_condition, int lstep_num, int rstep_num) {
    switch (corner_condition) {
    case 0: //两边都没找到
        if (lstep_num >= rstep_num) {
            track_leftline(left_line_b, lstep_num, center_line_l, 10, 22);
            for (int k = 0; k < lstep_num; k++) {
                writeABit2(line_image_turn, &center_line_l[k]);
            }
        }
        else {
            track_rightline(right_line_b, rstep_num, center_line_r, 10, 22);
            for (int k = 0; k < rstep_num; k++) {
                writeABit2(line_image_turn, &center_line_r[k]);
            }
        }
        break;
    case 1: //两边都找到
        /*
        track_rightline(right_line_b, rstep_num, center_line_r, 10, 22);
        for (int k = 0; k < rstep_num; k++) {
            writeABit2(line_image_turn, &center_line_r[k]);
        }*/
        cornerRefindLine(1, binary_image, lstep_num, rstep_num);
        break;
    case 2: //找到左边
        track_rightline(right_line_b, rstep_num, center_line_r, 10, 22);
        for (int k = 0; k < rstep_num; k++) {
            writeABit2(line_image_turn, &center_line_r[k]);
        }
        cornerRefindLine(2, binary_image, lstep_num, 66);
        break;
    case 3: //找到右边
        track_leftline(left_line_b, lstep_num, center_line_l, 10, 22);
        for (int k = 0; k < lstep_num; k++) {
            writeABit2(line_image_turn, &center_line_l[k]);
        }
        cornerRefindLine(3, binary_image, 66, rstep_num);
        break;
    }
}

void test()
{
    Mat srcImg, img_g, img_o;
    char name[20];
    namedWindow("Original image", CV_WINDOW_AUTOSIZE);
    namedWindow("Output image", CV_WINDOW_AUTOSIZE);
    namedWindow("OS image", CV_WINDOW_AUTOSIZE);
    namedWindow("OS image2", CV_WINDOW_AUTOSIZE);
    /*
    if (srcImg.empty())
    {
        cout << "could not load image...\n" << endl;
    }*/

    //赛道大津阈值法
    //P00223 ~ P00797
    for (int i = 223; i <= 797; i++) {
        sprintf_s(name, "test/P00%d.BMP", i);
        srcImg = imread(name);
        
        cvtColor(srcImg, img_g, cv::COLOR_BGR2GRAY);
        for (int i = 0; i < kHeight; i++) {
            for (int j = 0; j < kWidth; j++) {
                gray_image[i][j] = img_g.ptr<uchar>(i)[j];
            }
        }

        imshow("Original image", srcImg);

        //大津阈值法
        threshold(img_g, img_o, OTSU(gray_image), 255, cv::THRESH_BINARY);

        for (int i = 0; i < kHeight; i++) {
            for (int j = 0; j < kWidth; j++) {
                binary_image[i][j] = img_o.ptr<uchar>(i)[j];
            }
        }
        imshow("Output image", img_o);

        findLinePoints(binary_image);
        
        int lstep_num, rstep_num;
        steptoLine(binary_image, &lstep_num, &rstep_num, 0, 0);
        edgeLinePerspective(left_line, lstep_num, left_line_a);
        edgeLinePerspective(right_line, rstep_num, right_line_a);
        blurPoints(left_line_a, lstep_num, left_line_b, 7);
        blurPoints(right_line_a, rstep_num, right_line_b, 7);
        local_angle_points(left_line_b, lstep_num, left_angle, 5);
        
        local_angle_points(right_line_b, rstep_num, right_angle, 5);
        
        int corner_id_l, corner_id_r;
        uchar corner_condition = CrossFindCorner(&corner_id_l, &corner_id_r, lstep_num, rstep_num);
        
        for (int k = 0; k < EDGELINE_LENGTH; k++) {
            left_angle[k] = 0;
        }
        
        for (int k = 0; k < EDGELINE_LENGTH; k++) {
            right_angle[k] = 0;
        }
        
        for (int k = 0; k < lstep_num; k++) {
            writeABit(line_image, &left_line[k]);
        }
        for (int k = 0; k < rstep_num; k++) {
            writeABit(line_image, &right_line[k]);
        }

        for (int k = 0; k < lstep_num; k++) {
            writeABit2(line_image_turn, &left_line_b[k]);
        }
        for (int k = 0; k < rstep_num; k++) {
            writeABit2(line_image_turn, &right_line_b[k]);
        }

        edgeDetectionCross(corner_condition, lstep_num, rstep_num);

        Mat img_os(kHeight, kWidth, CV_8UC1, line_image);
        Mat img_os2(kHeight * 2, kWidth * 2, CV_8UC1, line_image_turn);
        imshow("OS image", img_os);
        imshow("OS image2", img_os2);

        clearLine(line_image);
        clearLine2(line_image_turn);
        waitKey(0);
    }
}
int main()
{
    test();
    return 0;
}
