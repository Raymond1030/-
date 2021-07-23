#include"ShadowDetect.h"
void RGBToHSV(Mat r, Mat g,Mat b, Mat* h, Mat* s, Mat* v) {
	//HSV�ռ�ת��
	r.convertTo(r, CV_64FC1);
	g.convertTo(g, CV_64FC1);
	b.convertTo(b, CV_64FC1);

	Mat temp1(r.size(), CV_64FC1), temp2(r.size(), CV_64FC1);

	//  V=1/3 *(R+G+B)
	add(r, g, temp1);
	add(temp1, b, temp1);
	divide(temp1, 3, *v);

	//  S=1-min(R,G,B)/V
	for (int i = 0; i < r.rows; i++)
		for (int j = 0; j < r.cols; j++)
		{
			double val = 0;
			val = min(r.at<double>(i, j), g.at<double>(i, j));
			temp2.at<double>(i, j) = min(val, b.at<double>(i, j));
		}

	divide(temp2, *v, temp2);
	subtract(1, temp2, *s);


	//  H = ��   360-��
	for (int i = 0; i < r.rows; i++)
		for (int j = 0; j < r.cols; j++)
		{
			double valr = r.at<double>(i, j);
			double valg = g.at<double>(i, j);
			double valb = b.at<double>(i, j);

			if (valb <= valg)
			{
				h->at<double>(i, j) = 180 / Pi * acos(0.5 * (valr - valg + valr - valb)
					/ sqrt((valr - valg) * (valr - valg) + (valr - valb) * (valg - valb)));
			}
			else
			{
				h->at<double>(i, j) = 360 - 180 / Pi * acos(0.5 * (valr - valg + valr - valb)
					/ sqrt((valr - valg) * (valr - valg) + (valr - valb) * (valg - valb)));
			}

		}

}

Mat Shadow(Mat h, Mat s, Mat v)
{
	Mat Img;

	Mat res1, res2;  // ���ӷ�ĸ

	subtract(s, v, res1);
	add(h, s, res2);
	add(res2, v, res2);

	divide(res1, res2, Img);

	return Img;
}

Mat Segment(Mat img, int type)
{

	Mat out(img.size(), CV_8UC1);

	double temp = 0;
	double max = 0;
	double min = 255;

	//��������

	//�Ҷ����� ��0-255
	for (int i = 0; i < img.rows; i++)
		for (int j = 0; j < img.cols; j++)
		{
			temp = img.at<double>(i, j);
			if (temp > max)
			{
				max = temp;
			}
			if (temp < min)
			{
				min = temp;
			}
		}
	double scale = 255 / (max - min);

	for (int i = 0; i < img.rows; i++)
		for (int j = 0; j < img.cols; j++)
		{
			out.at<uchar>(i, j) = (unsigned char)((int)((img.at<double>(i, j) - min) * scale));
		}

	//��̬������ֵ
	int thre = 0;
	double f1 = 0; double f2 = 0;         //ǰ�󾰻Ҷ�Ƶ��
	double s_temp = 0; double s = 0;     //����
	double avg1 = 0; double avg2 = 0;    //ǰ�󾰷ֱ��ֵ

	for (int t = 0; t < 256; t++)          //��ÿһ����ֵ
	{
		f1 = 0;
		f2 = 0;
		avg1 = 0;
		avg2 = 0;

		for (int i = 0; i < out.rows; i++)
		{
			for (int j = 0; j < out.cols; j++)
			{
				int val = (int)(out.at<uchar>(i, j));
				if (val < t)
				{
					f1++;
					avg1 += val;
				}
				else
				{
					f2++;
					avg2 += val;
				}
			}
		}
		avg1 /= f1;
		avg2 /= f2;
		s_temp = f1 * f2 * pow((avg1 - avg2), 2) / pow((out.rows - out.cols), 2);

		if (s_temp > s)    //�ҵ���̬�滮�����ֵ
		{
			thre = t;
			s = s_temp;
		}
	}


	Mat res(out.size(), out.type());

	//��ֵ��
	if (type == HSV)
	{
		for (int i = 0; i < res.rows; i++)
			for (int j = 0; j < res.cols; j++)
			{
				int val = (int)(out.at<uchar>(i, j));
				if (val > thre + T1)   //////////��ֵ����
				{
					res.at<uchar>(i, j) = 255;
				}
				else
				{
					res.at<uchar>(i, j) = 0;
				}
			}
	
	}
	else if (type == C1C2C3)
	{
		for (int i = 0; i < res.rows; i++)
			for (int j = 0; j < res.cols; j++)
			{
				int val = (int)(out.at<uchar>(i, j));
				if (val > thre + T2)   //////////��ֵ����
				{
					res.at<uchar>(i, j) = 255;
				}
				else
				{
					res.at<uchar>(i, j) = 0;
				}
			}
	
	}
	return res;
}

Mat Merge(Mat img, int type)
{
	Mat temp = img.clone();

	if (type == HSV)
	{
		//��ʴ�����Ͳ����γ���ͨ��
		Mat element1 = getStructuringElement(MORPH_ELLIPSE, Size(2, 2));
		Mat element2 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));

		dilate(temp, temp, element1, Point(-1, -1), 6);
		dilate(temp, temp, element2, Point(-1, -1), 3);
		erode(temp, temp, element1, Point(-1, -1), 1);

		//Ѱ����ͨ����
		Mat res = temp.clone();
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(res, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

		for (int i = 0; i < contours.size(); i++)
		{
			if (contourArea(contours[i]) < 100)    //ѡȡ��ͨ�������ֵ
			{
				drawContours(res, contours, i, Scalar(0), -1);
			}
		}

	
		return res;
	}
	else if (type == C1C2C3)
	{
		//��ʴ�����Ͳ����γ���ͨ��
		Mat element1 = getStructuringElement(MORPH_ELLIPSE, Size(2, 2));
		Mat element2 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));

		dilate(temp, temp, element1, Point(-1, -1), 2);
		dilate(temp, temp, element2, Point(-1, -1), 1);
		erode(temp, temp, element1, Point(-1, -1), 1);

		//Ѱ����ͨ����
		Mat res = temp.clone();
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(res, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

		for (int i = 0; i < contours.size(); i++)
		{
			if (contourArea(contours[i]) < 100)    //ѡȡ��ͨ�������ֵ
			{
				drawContours(res, contours, i, Scalar(0), -1);
			}
		}

		return res;
	}


}

Mat RGBToC1C2C3(Mat r, Mat g, Mat b)
{
	r.convertTo(r, CV_64FC1);
	g.convertTo(g, CV_64FC1);
	b.convertTo(b, CV_64FC1);

	Mat C3(r.size(), CV_64FC1);

	for (int i = 0; i < r.rows; i++)
		for (int j = 0; j < r.cols; j++)
		{
			C3.at<double>(i, j) = atan(b.at<double>(i, j) / max(r.at<double>(i, j), g.at<double>(i, j)));
		}

	return C3;
}
