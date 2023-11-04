#include "ImageTransform.h"
#include <math.h>

uchar line_image[kHeight][kWidth] = { 0 }; //����ͼ

uchar binary_image[kHeight][kWidth] = { 0 }; //��ֵ��ͼ
uchar gray_image[kHeight][kWidth] = { 0 }; //�Ҷ�ͼ
myPoint left_line[EDGELINE_LENGTH] = { 0 }, right_line[EDGELINE_LENGTH] = { 0 };//���ұ���
myPoint_f left_line_a[EDGELINE_LENGTH] = { 0 }, right_line_a[EDGELINE_LENGTH] = { 0 };//��͸�Ӻ�����ұ���

uchar getAbit(uchar(*img)[kWidth], myPoint* p) {
    return img[p->y][p->x];
}

void setmyPoint(myPoint* p, int y, int x) {
    p->y = y; p->x = x;
}

void setmyPoint_f(myPoint_f* p, float y, float x) {
    p->y = y; p->x = x;
}

void writeABit(uchar(*img)[kWidth], myPoint* p) {
    if (p->x >= 0 && p->x < kWidth && p->y >= 0 && p->y < kHeight) {
        img[p->y][p->x] = WHITE;
    }
}

void writeABit2(uchar(*img)[kWidth * 2], myPoint_f* p) {
    if (p->x >= 0 && p->x < kWidth * 2 && p->y >= 0 && p->y < kHeight * 2) {
        img[(int)p->y][(int)p->x] = WHITE;
    }
}

void clearLine(uchar(*img)[kWidth]) {
    for (int i = 0; i < kHeight; i++) {
        for (int j = 0; j < kWidth; j++) {
            img[i][j] = 0;
        }
    }
}

void clearLine2(uchar(*img)[kWidth * 2]) {
    for (int i = 0; i < kHeight * 2; i++) {
        for (int j = 0; j < kWidth * 2; j++) {
            img[i][j] = 0;
        }
    }
}

/***********************************************
* @brief : �����ֵ������ͼƬ��ֵ����ֵ
* @param : img_g �Ҷ�ͼ
* @return: ��ֵ
* @date  : 2023.10.01
* @author: DJJ
************************************************/
int OTSU(uchar(*img_g)[kWidth]) {
    int n = kHeight * kWidth;
    short gray[256] = { 0 };
    for (int i = 0; i < kHeight; i++) {
        for (int j = 0; j < kWidth; j++) {
            gray[img_g[i][j]]++;
        }
    }

    int mink = 30;
    double mint = 9999.0;
    for (int k = 30; k < 128; k++) {
        double t2 = 0; //����ֵ

        int nb = 0, nf = 0;
        double ub = 0.0, uf = 0.0, b2 = 0.0, f2 = 0.0, wb, wf;
        for (int i = 0; i <= k; i++) {
            nb += gray[i];
            ub += i * gray[i];
        }
        for (int i = k + 1; i < 256; i++) {
            nf += gray[i];
            uf += i * gray[i];
        }
        ub /= nb;
        uf /= nf;
        wb = (double)nb / n;
        wf = (double)nf / n;
        for (int i = 0; i <= k; i++) {
            b2 += pow(i - ub, 2) * gray[i];
        }
        for (int i = k + 1; i < 256; i++) {
            f2 += pow(i - uf, 2) * gray[i];
        }
        b2 /= nb;
        f2 /= nf;

        t2 = wb * b2 + wf * f2;
        if (mint > t2) {
            mint = t2;
            mink = k;
        };
    }

    return mink;
}

/***********************************************
* @brief : ��ֵ��
* @param : img_g �Ҷ�ͼ
* @return: ��ֵ
* @date  : 2023.10.26
* @author: DJJ
************************************************/
void threshold(uchar(*img_g)[kWidth], int thr) {
    for (int i = 0; i < kHeight; i++) {
        for (int j = 0; j < kWidth; j++) {
            img_g[i][j] = img_g[i][j] <= thr ? BLACK : WHITE;
        }
    }
}

/***********************************************
* @brief : �ҵ����ұ���
* @param : img ��ֵ��ͼƬ
* @return: ��
* @date  : 2023.10.13
* @author: DJJ
************************************************/
void findLinePoints(uchar(*img)[kWidth]) {
    int mid = kWidth / 2;
    int leftx, rightx, lefty = kHeight - BEGIN_TO_FIND, righty = kHeight - BEGIN_TO_FIND;
    for (leftx = mid; leftx > 3; leftx--) {
        if (img[lefty][leftx - 1] == BLACK && img[lefty][leftx - 3] == BLACK) {
            break;
        };
    }
    if (leftx == 3) {
        while (img[lefty - 1][3] == WHITE && lefty > 0) {
            lefty--;
        }
    }
    for (rightx = mid; rightx < kWidth - 3; rightx++) {
        if (img[righty][rightx + 1] == BLACK && img[righty][rightx + 3] == BLACK) {
            break;
        };
    }
    if (rightx == kWidth - 3) {
        while (img[righty - 1][kWidth - 3] == WHITE && righty > 0) {
            righty--;
        }
    }
    setmyPoint(&left_line[0], lefty, leftx);
    setmyPoint(&right_line[0], righty, rightx);
}

/***********************************************
* @brief : �����Թ���Ѳ��
* @param : img ��ֵ��ͼƬ lstep_num �����Ѳ�߳��� rstep_num �ұ���Ѳ�߳���
* @return: ��
* @date  : 2023.10.13
* @author: DJJ
************************************************/
void steptoLine(uchar(*img)[kWidth], int* lstep_num, int* rstep_num, int lfrom, int rfrom) {
    myPoint left, font, right;
    int leftg, fontg, rightg;
    int f = kFont;
    //�����Ѳ��
    for (int k = lfrom+1; k < EDGELINE_LENGTH && left_line[k - 1].x > 1 && left_line[k - 1].x < kWidth - 1 &&
        left_line[k - 1].y > 15 && left_line[k - 1].y < kHeight - 1; k++) {
        int count = 0;
        while (count < 4) {
            if (f == kFont) {
                setmyPoint(&left, left_line[k - 1].y - 1, left_line[k - 1].x - 1);
                setmyPoint(&font, left_line[k - 1].y - 1, left_line[k - 1].x);
                setmyPoint(&right, left_line[k - 1].y - 1, left_line[k - 1].x + 1);
            }
            if (f == kRight) {
                setmyPoint(&left, left_line[k - 1].y - 1, left_line[k - 1].x + 1);
                setmyPoint(&font, left_line[k - 1].y, left_line[k - 1].x + 1);
                setmyPoint(&right, left_line[k - 1].y + 1, left_line[k - 1].x + 1);
            }
            if (f == kLeft) {
                setmyPoint(&left, left_line[k - 1].y + 1, left_line[k - 1].x - 1);
                setmyPoint(&font, left_line[k - 1].y, left_line[k - 1].x - 1);
                setmyPoint(&right, left_line[k - 1].y - 1, left_line[k - 1].x - 1);
            }
            if (f == kBack) {
                setmyPoint(&left, left_line[k - 1].y + 1, left_line[k - 1].x + 1);
                setmyPoint(&font, left_line[k - 1].y + 1, left_line[k - 1].x);
                setmyPoint(&right, left_line[k - 1].y + 1, left_line[k - 1].x - 1);
            }
            leftg = getAbit(img, &left);
            fontg = getAbit(img, &font);
            rightg = getAbit(img, &right);
            if (fontg == 0) {
                f++;
                f = f == 4 ? 0 : f;
                count++;
                continue;
            }
            if (fontg == 255 && leftg == 255) {
                f--;
                f = f == -1 ? 3 : f;
                left_line[k] = left;
                break;
            }
            if (fontg == 255 && leftg == 0) {
                left_line[k] = font;
                break;
            }
        }
        *lstep_num = k;
    }

    //�ұ���Ѳ��
    f = kFont;
    for (int k = rfrom+1; k < EDGELINE_LENGTH && right_line[k - 1].x > 1 && right_line[k - 1].x < kWidth - 1 &&
        right_line[k - 1].y > 15 && right_line[k - 1].y < kHeight - 1; k++) {
        int count = 0;
        while (count < 4) {
            if (f == kFont) {
                setmyPoint(&left, right_line[k - 1].y - 1, right_line[k - 1].x - 1);
                setmyPoint(&font, right_line[k - 1].y - 1, right_line[k - 1].x);
                setmyPoint(&right, right_line[k - 1].y - 1, right_line[k - 1].x + 1);
            }
            if (f == kRight) {
                setmyPoint(&left, right_line[k - 1].y - 1, right_line[k - 1].x + 1);
                setmyPoint(&font, right_line[k - 1].y, right_line[k - 1].x + 1);
                setmyPoint(&right, right_line[k - 1].y + 1, right_line[k - 1].x + 1);
            }
            if (f == kLeft) {
                setmyPoint(&left, right_line[k - 1].y + 1, right_line[k - 1].x - 1);
                setmyPoint(&font, right_line[k - 1].y, right_line[k - 1].x - 1);
                setmyPoint(&right, right_line[k - 1].y - 1, right_line[k - 1].x - 1);
            }
            if (f == kBack) {
                setmyPoint(&left, right_line[k - 1].y + 1, right_line[k - 1].x + 1);
                setmyPoint(&font, right_line[k - 1].y + 1, right_line[k - 1].x);
                setmyPoint(&right, right_line[k - 1].y + 1, right_line[k - 1].x - 1);
            }
            leftg = getAbit(img, &left);
            fontg = getAbit(img, &font);
            rightg = getAbit(img, &right);
            if (fontg == 0) {
                f--;
                f = f == -1 ? 3 : f;
                count++;
                continue;
            }
            if (fontg == 255 && rightg == 255) {
                f++;
                f = f == 4 ? 0 : f;
                right_line[k] = right;
                break;
            }
            if (fontg == 255 && rightg == 0) {
                right_line[k] = font;
                break;
            }
        }
        *rstep_num = k;
    }
}

/***********************************************
* @brief : ������͸��
* @param : myPoint* in_line: ԭͼɨ�ߵõ��ı���
*          uchar num: �������鳤��
*          myPoint_f* out_line: ͸��֮��ı���
* @return: ��
* @date  : 2023.10.21
* @author: DJJ
************************************************/
void edgeLinePerspective(myPoint* in_line, uchar num, myPoint_f* out_line)
{
    double change_inverse_Mat[3][3] = { {0.086854,-0.072352,1.464790},{0.002694,-0.000090,2.511884},{0.000029,-0.000810,0.101123} }; //��Ҫ����

    for (uchar count = 0; count < num; count++) {
        float i = in_line[count].x; float j = in_line[count].y; //�任
        float solve_x = ((i * change_inverse_Mat[2][2] - change_inverse_Mat[0][2]) * (change_inverse_Mat[1][1] - j * change_inverse_Mat[2][1])
            - (change_inverse_Mat[0][1] - i * change_inverse_Mat[2][1]) * (j * change_inverse_Mat[2][2] - change_inverse_Mat[1][2]))
            / ((change_inverse_Mat[0][0] - i * change_inverse_Mat[2][0]) * (change_inverse_Mat[1][1] - j * change_inverse_Mat[2][1])
                - (change_inverse_Mat[0][1] - i * change_inverse_Mat[2][1]) * (change_inverse_Mat[1][0] - j * change_inverse_Mat[2][0]));
        float solve_y = ((i * change_inverse_Mat[2][2] - change_inverse_Mat[0][2]) * (change_inverse_Mat[1][0] - j * change_inverse_Mat[2][0])
            - (change_inverse_Mat[0][0] - i * change_inverse_Mat[2][0]) * (j * change_inverse_Mat[2][2] - change_inverse_Mat[1][2]))
            / ((change_inverse_Mat[0][1] - i * change_inverse_Mat[2][1]) * (change_inverse_Mat[1][0] - j * change_inverse_Mat[2][0])
                - (change_inverse_Mat[0][0] - i * change_inverse_Mat[2][0]) * (change_inverse_Mat[1][1] - j * change_inverse_Mat[2][1]));

        out_line[count].x = solve_x + kWidth / 2;//����λ��
        out_line[count].y = solve_y + kHeight / 2 + 50;
    }
}

/***********************************************
* @brief : �Ա��߽�������ƽ���˲�
* @param : myPoint* in_line:ɨ�ߵõ�����ֵΪ���εı�������
*         int num: ��������ĳ���
*         myPoint_f* out_line: �˲�֮�󸡵����ݾ��ȵı�������
*         uint8 kernel: �˲�һά���Ծ���˴�С������Ϊ������
* @return: ��
* @date  : 2023.1.10
* @author: �Ͻ���Դ
************************************************/
void blurPoints(myPoint_f* in_line, int num, myPoint_f* out_line, uchar kernel) {
    int half = kernel / 2;
    float x = 0, y = 0;
    for (int i = 0; i < num; i++)
    {
        x = y = 0;
        for (int j = -half; j <= half; j++)
        {
            x += in_line[Limit(i + j, 0, num - 1)].x * (half + 1 - abs(j));
            y += in_line[Limit(i + j, 0, num - 1)].y * (half + 1 - abs(j));
        }
        x /= (2 * half + 2) * (half + 1) / 2;
        y /= (2 * half + 2) * (half + 1) / 2;
        out_line[i].x = x;
        out_line[i].y = y;
    }
}

/***********************************************
* @brief : ����߰���ߵõ��ұ���
* @param : myPoint_f* in_line: ����ı���
*          int num: ������ߵĳ���
*          myPoint_f* out_line: ���������
*          int approx_num:����б��ʹ�õ�ǰ���ĸ���
*          float dist:���������
* @return: ��
* @date  : 2023.1.15
* @author: �Ͻ���Դ
************************************************/
void track_leftline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist)
{
    for (int i = 0; i < num; i++)
    {
        float dx = in_line[Limit(i + approx_num, 0, num - 1)].x - in_line[Limit(i - approx_num, 0, num - 1)].x;
        float dy = in_line[Limit(i + approx_num, 0, num - 1)].y - in_line[Limit(i - approx_num, 0, num - 1)].y;
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        out_line[i].x = in_line[i].x - dy * dist;
        out_line[i].y = in_line[i].y + dx * dist;
    }
}
// �ұ��߸�������
void track_rightline(myPoint_f* in_line, int num, myPoint_f* out_line, int approx_num, float dist)
{
    for (int i = 0; i < num; i++)
    {
        float dx = in_line[Limit(i + approx_num, 0, num - 1)].x - in_line[Limit(i - approx_num, 0, num - 1)].x;
        float dy = in_line[Limit(i + approx_num, 0, num - 1)].y - in_line[Limit(i - approx_num, 0, num - 1)].y;
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        out_line[i].x = in_line[i].x + dy * dist;
        out_line[i].y = in_line[i].y - dx * dist;
    }
}

/***********************************************
* @brief : �ǵ��ٴ�Ѳ��
* @param : corner_condition ״̬
* @return: void
* @date  : 2023.11.2
* @author: DJJ
************************************************/
void cornerRefindLine(uchar corner_condition, uchar(*img)[kWidth], int current_lstep, int current_rstep) {
    int lstep_num, rstep_num, lefty, righty;
    switch (corner_condition) {
    case 1:
        if (left_line[current_lstep].x > 2) {
            return;
        }
        for (lefty = left_line[current_lstep].y; lefty > 3; lefty--) {
            if (img[lefty - 1][2] == BLACK) {
                break;
            };
        }
        setmyPoint(&left_line[current_lstep], lefty, 2);
        steptoLine(binary_image, &lstep_num, &rstep_num, current_lstep, 1);
        edgeLinePerspective(left_line, lstep_num, left_line_a);
        blurPoints(left_line_a, lstep_num, left_line_b, 7);
        for (int k = current_lstep; k < lstep_num; k++) {
            writeABit(line_image, &left_line[k]);
        }
        for (int k = current_lstep; k < lstep_num; k++) {
            writeABit2(line_image_turn, &left_line_b[k]);
        }
        if (right_line[current_rstep].x < kWidth - 2) {
            return;
        }
        for (righty = right_line[current_rstep].y; righty > 3; righty--) {
            if (img[righty - 1][kWidth - 2] == BLACK) {
                break;
            };
        }
        setmyPoint(&right_line[current_rstep], righty, kWidth - 2);
        steptoLine(binary_image, &lstep_num, &rstep_num, 1, current_rstep);
        edgeLinePerspective(right_line, rstep_num, right_line_a);
        blurPoints(right_line_a, rstep_num, right_line_b, 7);
        for (int k = current_rstep; k < rstep_num; k++) {
            writeABit(line_image, &right_line[k]);
        }
        for (int k = current_rstep; k < rstep_num; k++) {
            writeABit2(line_image_turn, &right_line_b[k]);
        }
        break;
    case 2:
        if (left_line[current_lstep].x > 2) {
            return;
        }
        for (lefty = left_line[current_lstep].y; lefty > 3; lefty--) {
            if (img[lefty - 1][2] == BLACK) {
                break;
            };
        }
        setmyPoint(&left_line[current_lstep], lefty, 2);
        steptoLine(binary_image, &lstep_num, &rstep_num, current_lstep, 1);
        edgeLinePerspective(left_line, lstep_num, left_line_a);
        blurPoints(left_line_a, lstep_num, left_line_b, 7);
        for (int k = current_lstep; k < lstep_num; k++) {
            writeABit(line_image, &left_line[k]);
        }
        for (int k = current_lstep; k < lstep_num; k++) {
            writeABit2(line_image_turn, &left_line_b[k]);
        }
        break;
    case 3:
        if (right_line[current_rstep].x < kWidth - 2) {
            return;
        }
        for (righty = right_line[current_rstep].y; righty > 3; righty--) {
            if(img[righty - 1][kWidth - 2] == BLACK) {
                break;
            };
        }
        setmyPoint(&right_line[current_rstep], righty, kWidth - 2);
        steptoLine(binary_image, &lstep_num, &rstep_num, 1, current_rstep);
        edgeLinePerspective(right_line, rstep_num, right_line_a);
        blurPoints(right_line_a, rstep_num, right_line_b, 7);
        for (int k = current_rstep; k < rstep_num; k++) {
            writeABit(line_image, &right_line[k]);
        }
        for (int k = current_rstep; k < rstep_num; k++) {
            writeABit2(line_image_turn, &right_line_b[k]);
        }
        break;
    }
}

