#include<opencv2/highgui.hpp>
#include<opencv2/core.hpp>

#include<stdio.h>
#include <iostream>
#include<string>

#define Plant 0
#define Water 1
#define Buliding 2
using namespace std;
using namespace cv;

Mat Color(Mat Index,int type);//α��ɫ��
Mat ColorRGB(Mat b, Mat g, Mat r);//���ɫ�ϳ�
Mat Binary(Mat Index);//��ֵ��
Mat RVI(Mat Rn, Mat Rr);//��ֲֵ��ָ��
Mat Normalize(Mat s1, Mat s2);//NDVIֲ����һ��ָ����NDWI��һ��ˮ��ָ����NDBI��һ�����ָ��
Mat SAVI(Mat Rn, Mat Rr);//��������ֲ��ָ����NDWI��һ��ˮ��ָ��
Mat AWEI(Mat G, Mat nR, Mat mR, Mat dR);//�Զ�ˮ����ȡָ��
Mat IBI(Mat NDBI, Mat SAVI, Mat NDWI);//�����õ�ָ��

