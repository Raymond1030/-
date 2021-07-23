#pragma once

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<stdio.h>
#include <iostream>
#include<algorithm>
using namespace std;
using namespace cv;
#define Pi 3.1415926

#define T1  80  //�����������ߵ���ֵ ����HSVȥ��
#define T2  40  //�����������ߵ���ֵ ����C1C2C3ȥ��
//���
#define HSV 0            
#define C1C2C3 1
void RGBToHSV(Mat r, Mat g, Mat b,Mat* h, Mat* s, Mat* v);//HSV�ռ�ת��
Mat Shadow(Mat h, Mat s, Mat v);//HSV��Ӱʶ��
Mat Segment(Mat img, int type);//��ֵ������
Mat Merge(Mat img, int type);//�ϲ�
Mat RGBToC1C2C3(Mat r, Mat g, Mat b);//C1C2C3�ռ�ת��

